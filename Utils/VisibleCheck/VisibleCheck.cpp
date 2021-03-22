#include "VisibleCheck.h"

#pragma optimize("", off)
typedef int(__fastcall* UTIL_TraceLine_t)(const Vec3&, const Vec3&, unsigned int fMask, const DWORD basePlayer, int, CGameTrace* trace);
bool WINAPI hkCreateMove(float flInputSampleTime, CUserCmd* pCmd) {
	// hkCreateMove + 0x9 = pointer to Vars; 
	CVisibleCheck::ICreateMoveVars* Vars = (CVisibleCheck::ICreateMoveVars*)dwCreateMoveVars; // Initialize in InitCreateMoveHook()
	DWORD localPlayer = *(DWORD*)(Vars->dwLocalPlayer);
	bool isInGame = *(int*)(Vars->dwClientState + 0x108) == 6;
	bool deltaTick = *(int*)(Vars->dwClientState + Vars->m_nDeltaTick);
	UTIL_TraceLine_t TraceLine = (UTIL_TraceLine_t)Vars->dwTraceLine;

	if (!pCmd->commandNumber || !isInGame || !localPlayer || deltaTick == -1) {
		return false;
	}

	bool localIsAlive = !*(bool*)(localPlayer + Vars->m_lifeState);
	if (!localIsAlive) {
		return false;
	}

	Vars->localOrigin = *(Vec3*)(localPlayer + Vars->m_vecOrigin);
	Vars->localViewOffset = *(Vec3*)(localPlayer + Vars->m_vecViewOffset);
	Vars->localEyePosition.x = Vars->localOrigin.x + Vars->localViewOffset.x;
	Vars->localEyePosition.y = Vars->localOrigin.y + Vars->localViewOffset.y;
	Vars->localEyePosition.z = Vars->localOrigin.z + Vars->localViewOffset.z;

	for (int i = 0; i < 64; i++) {
		Vars->visibleStruct->player[i] = false;

		DWORD pEntity = *(DWORD*)(Vars->dwEntityList + i * 0x10);

		if (!pEntity || pEntity == localPlayer) {
			continue;
		}

		bool lifeState = *(bool*)(pEntity + Vars->m_lifeState);
		bool dormant = *(bool*)(pEntity + Vars->m_bDormant);
		int  health = *(int*)(pEntity + Vars->m_iHealth);

		if (lifeState || dormant || health <= 0) {
			continue;
		}

		// Get Enemy Head Position
		DWORD boneMatrix = *(DWORD*)(pEntity + Vars->m_dwBoneMatrix);
		if (!boneMatrix) {
			continue;
		}

		bool isVisible = false;
		for (int j = 0; j < 17; j++) {
			auto bone = Vars->all_bones[j];

			Vars->enemyBonePosition.x = *(float*)(boneMatrix + 0x30 * bone + 0x0C);
			Vars->enemyBonePosition.y = *(float*)(boneMatrix + 0x30 * bone + 0x1C);
			Vars->enemyBonePosition.z = *(float*)(boneMatrix + 0x30 * bone + 0x2C);

			// Visible check
			TraceLine(Vars->localEyePosition, Vars->enemyBonePosition, MASK_SHOT | CONTENTS_GRATE, localPlayer, 0, Vars->traceOutput);

			if (Vars->traceOutput->fraction > Vars->min_fraction || Vars->traceOutput->hitEntity == pEntity) {
				isVisible = true;
				Vars->visibleStruct->playerBones[i][bone] = true;
			}
			else {
				Vars->visibleStruct->playerBones[i][bone] = false;
			}
		}
		Vars->visibleStruct->player[i] = isVisible;
	}

	return false;
}
DWORD WINAPI hkCreateMoveEnd(){ return 0; }
#pragma optimize("", on)

CVisibleCheck::CVisibleCheck(){
	hooked = false;
	
	for (int i = 0; i < 64; i++){
		visibleStruct.player[i] = false;

		for (int j = 0; j < 80; j++) {
			visibleStruct.playerBones[i][j] = false;
		}
	}
}

CVisibleCheck::~CVisibleCheck() {
	if (allocatedAddressTraceOutput) {
		mem.free(allocatedAddressTraceOutput);
	}
	if (allocatedAddressVisibleStruct) {
		mem.free(allocatedAddressVisibleStruct);
	}
	if (allocatedAddressHookCode) {
		mem.free(allocatedAddressHookCode);
	}
}
	
bool CVisibleCheck::updateVisibleStruct(){
	if (!hooked) {
		return false;
	}

	if (!dwVisibleStruct) {
		return false;
	}

	if (!ReadProcessMemory(mem._process, (LPVOID)dwVisibleStruct, &visibleStruct, sizeof(IVisible), NULL)) {
		return false;
	}
	
	return true;
}

bool CVisibleCheck::isVisible(int entityId){
	if (!hooked) {
		return false;
	}

	if (!dwVisibleStruct) {
		return false;
	}

	if (entityId >= 64) {
		return false;
	}
	
	return visibleStruct.player[entityId];
}

bool CVisibleCheck::isVisible(int entityId, int bone) {
	if (!hooked) {
		return false;
	}

	if (!dwVisibleStruct) {
		return false;
	}

	if (entityId >= 64) {
		return false;
	}

	if (bone >= 80) {
		return false;
	}

	return visibleStruct.playerBones[entityId][bone];
}

bool CVisibleCheck::init(){
	if (hooked) {
		return true;
	}
		
	// Set CreateMove local Vars
	ICreateMoveVars hkCreateMoveVars;
	#pragma region hkCreateMoveVars initialization
	hkCreateMoveVars.dwClientState 		= mem.read<DWORD>(engineDll.dwBase + Offsets::signatures::dwClientState);
	hkCreateMoveVars.dwEntityList 		= clientDll.dwBase + Offsets::signatures::dwEntityList;
	hkCreateMoveVars.dwLocalPlayer 		= clientDll.dwBase + Offsets::signatures::dwLocalPlayer;
	hkCreateMoveVars.m_nDeltaTick 		= mem.read<DWORD>(engineDll.dwBase + Offsets::signatures::clientstate_delta_ticks);
	hkCreateMoveVars.m_dwBoneMatrix 	= Offsets::netvars::m_dwBoneMatrix;
	hkCreateMoveVars.m_vecViewOffset 	= Offsets::netvars::m_vecViewOffset;
	hkCreateMoveVars.m_vecOrigin 			= Offsets::netvars::m_vecOrigin;
	hkCreateMoveVars.m_lifeState 			= Offsets::netvars::m_lifeState;
	hkCreateMoveVars.m_iTeamNum 			= Offsets::netvars::m_iTeamNum;
	hkCreateMoveVars.m_iHealth 				= Offsets::netvars::m_iHealth;
	hkCreateMoveVars.m_bDormant 			= Offsets::netvars::m_bDormant;
	hkCreateMoveVars.dwTraceLine 			= Offsets::signatures::dwTraceLine;
	hkCreateMoveVars.min_fraction			= 0.97f;
	hkCreateMoveVars.all_bones[0] = 3;
	hkCreateMoveVars.all_bones[1] = 4;
	hkCreateMoveVars.all_bones[2] = 6;
	hkCreateMoveVars.all_bones[3] = 7;
	hkCreateMoveVars.all_bones[4] = 8;
	hkCreateMoveVars.all_bones[5] = 11;
	hkCreateMoveVars.all_bones[6] = 12;
	hkCreateMoveVars.all_bones[7] = 13;
	hkCreateMoveVars.all_bones[8] = 41;
	hkCreateMoveVars.all_bones[9] = 42;
	hkCreateMoveVars.all_bones[10] = 43;
	hkCreateMoveVars.all_bones[11] = 70;
	hkCreateMoveVars.all_bones[12] = 71;
	hkCreateMoveVars.all_bones[13] = 72;
	hkCreateMoveVars.all_bones[14] = 77;
	hkCreateMoveVars.all_bones[15] = 78;
	hkCreateMoveVars.all_bones[16] = 79;
	#pragma endregion
	
	// Allocate memory for result TraceLine function
	allocatedAddressTraceOutput = mem.allocate();
	hkCreateMoveVars.traceOutput = (CGameTrace*)allocatedAddressTraceOutput;
	
	// Allocate memory for visibleStruct
	allocatedAddressVisibleStruct = mem.allocate(sizeof(IVisible));
	hkCreateMoveVars.visibleStruct = (IVisible*)allocatedAddressVisibleStruct;
	
	// Allocate memory for Hook
	allocatedAddressHookCode = mem.allocate();
	LPVOID hookCodeAddress = allocatedAddressHookCode;
		
	if (!hkCreateMoveVars.traceOutput || !hkCreateMoveVars.visibleStruct || !(DWORD)hookCodeAddress)
		return false;
	
	// Write hkCreateMove function in to CSGO
	if (!WriteProcessMemory(mem._process, hookCodeAddress, hkCreateMove, (DWORD)hkCreateMoveEnd - (DWORD)hkCreateMove, NULL))
		return false;
	
	// Write hkCreateMoveVars(Local Variables for CreateMove hook)
	DWORD dw_hkCreateMoveVars = (DWORD)hookCodeAddress + (DWORD)hkCreateMoveEnd - (DWORD)hkCreateMove;
	if (!WriteProcessMemory(mem._process, (LPVOID)dw_hkCreateMoveVars, &hkCreateMoveVars, sizeof(ICreateMoveVars), NULL))
		return false;
	
	// Set pointer to hkCreateMoveVars
	if (!WriteProcessMemory(mem._process, (LPVOID)((DWORD)hookCodeAddress + 0x9), &dw_hkCreateMoveVars, sizeof(DWORD), NULL))
		return false;

	DWORD dwClientMode = mem.read<DWORD>(mem.read<DWORD>(Offsets::signatures::dwClientMode));

	VirtualFunction vfClientMode(dwClientMode);
	vfClientMode.hook(24, (DWORD)hookCodeAddress);
	
	dwVisibleStruct = (DWORD)hkCreateMoveVars.visibleStruct;
	hooked = true;

	return hooked;
}