#pragma once

#define   CONTENTS_SOLID                0x1  
#define   CONTENTS_WINDOW               0x2
#define   CONTENTS_GRATE                0x8
#define   CONTENTS_MOVEABLE             0x4000
#define   CONTENTS_MONSTER              0x2000000   
#define   CONTENTS_DEBRIS               0x4000000
#define   CONTENTS_HITBOX               0x40000000
#define		MASK_SHOT											(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX)