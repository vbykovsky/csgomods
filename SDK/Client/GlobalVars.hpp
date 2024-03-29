#pragma once

#include "../../Utils/Memory/Memory.hpp"
#include "../../Utils/Memory/Modules.hpp"
#include "../../Utils/Offsets/Offsets.hpp"

#include "../Vars.hpp"

class c_global_vars {
public:
  c_global_vars() {};

  int get() {
    return g_mem.read<int>(g_client_dll.base + c_offsets::signatures::dw_global_vars);
  }

public:
  PROP_(float,  real_time,                          get(), 0x0000, float, value)
  PROP_(int,    frame_count,                        get(), 0x0004, int,   value)
  PROP_(float,  absolute_frame_time,                get(), 0x0008, float, value)
  PROP_(float,  absolute_frame_start_time_std_dev,  get(), 0x000C, float, value)
  PROP_(float,  cur_time,                           get(), 0x0010, float, value)
  PROP_(float,  frame_time,                         get(), 0x0014, float, value)
  PROP_(int,    max_clients,                        get(), 0x0018, int,   value)
  PROP_(int,    tick_count,                         get(), 0x001C, int,   value)
  PROP_(float,  interval_per_tick,                  get(), 0x0020, float, value)
  PROP_(float,  interpolation_amount,               get(), 0x0024, float, value)
  PROP_(int,    sim_ticks_this_frame,               get(), 0x0028, int,   value)
  PROP_(int,    network_protocol,                   get(), 0x002C, int,   value)
};