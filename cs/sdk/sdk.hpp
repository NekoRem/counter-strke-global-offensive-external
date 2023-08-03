#pragma once

#include "classes/engine.hpp"
#include "classes/client.hpp"
#include "classes/weapon.hpp"
#include "classes/entity.hpp"

namespace cs
{
	namespace sdk
	{
		bool world_to_screen(vector pos, vector2d& screen);
	}
}