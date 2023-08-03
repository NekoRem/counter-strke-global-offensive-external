#pragma once

#define NOMINMAX

#include <string_view>
#include <functional>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <random>
#include <thread>
#include <vector>
#include <future>
#include <string>
#include <format>
#include <array>
#include <map>

#include <windows.h>
#include <psapi.h>
#include <string.h>
#include <tlhelp32.h>

#include "thirdparty/lazy_importer.hpp"
#include "thirdparty/xorstr.hpp"
#include "thirdparty/offsets.hpp"

#include <aero-overlay/overlay.hpp>
#include <valve-bsp-parser/bsp_parser.hpp>

#include "utils/winapi/winapi.hpp"
#include "utils/memory.hpp"
#include "utils/timer.hpp"
#include "utils/vector.hpp"
#include "utils/matrix.hpp"
#include "utils/math.hpp"
#include "utils/input.hpp"

#include "sdk/structs.hpp"
#include "sdk/offsets.hpp"
#include "sdk/sdk.hpp"

#include "core/vars.hpp"
#include "core/features/features.hpp"
#include "core/cache/on_world.hpp"
#include "core/cache/on_entity.hpp"