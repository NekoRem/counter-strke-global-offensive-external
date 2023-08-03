#include "../../includes.hpp"

void cs::features::misc::bunnyhop()
{
	do {
		if (!engine->in_game())
			continue;

		static int last_tick = {};
		static int last_frame = {};
		static float function_elapsed = {};

		sdk::structs::globalvars_t globalvars = engine->get_globalvars();
		const bool update = (globalvars.iTickCount != last_tick || globalvars.iFrameCount != last_frame);
		if (!update)
		{
			timer::sleep(globalvars.flIntervalPerTick);
			continue;
		}

		const float frametime = globalvars.flAbsFrameTime;
		const float delay = (function_elapsed - (frametime < globalvars.flIntervalPerTick ? (frametime * 0.5f) : frametime));
		const auto sleep = std::min(delay, (frametime * 1000));
		timer::sleep(sleep);

		const auto start = std::chrono::high_resolution_clock::now();
		if (!(memory::target == LI_FN(GetForegroundWindow).safe_cached()()))
			continue;

		if (!(LI_FN(GetAsyncKeyState).safe_cached()(VK_SPACE) & 0x8000))
			continue;

		if (client->in_menu())
			continue;

		if (!engine->in_game())
			continue;

		if (!local.get_entity())
			continue;

		int32_t move_type = local.move_type();
		if (move_type == sdk::structs::e_move_type::MOVETYPE_LADDER ||
			move_type == sdk::structs::e_move_type::MOVETYPE_NOCLIP ||
			move_type == sdk::structs::e_move_type::MOVETYPE_OBSERVER)
			continue;

		if ((local.get_flags() & sdk::structs::e_flags::FL_ONGROUND))
			client->force_jump(5);
		else
			if (client->get_force_jump() == 5)
				client->force_jump(4);

		const auto end = std::chrono::high_resolution_clock::now();

		// update last frame and last tick
		last_tick = globalvars.iTickCount;
		last_frame = globalvars.iFrameCount;

		std::chrono::duration<float, std::milli> elapsed = end - start;
		function_elapsed = elapsed.count();
	} while (vars::is_running);
}

void cs::features::misc::edgejump()
{
	do {
		if (!(LI_FN(GetAsyncKeyState).safe_cached()(VK_MENU) & 0x8000))
		{
			timer::sleep(1);
			continue;
		}

		if (!engine->in_game())
			continue;

		if (!local.get_entity())
			continue;

		if ((local.get_flags() & sdk::structs::e_flags::FL_ONGROUND))
			continue;

		vector origin = local.origin();
		vector origin_copy = origin;
		origin_copy.z -= 1;

		// cast a ray trace onto the object
		static rn::valve::trace_t ray = {};
		vars::bsp::bp->trace_ray(rn::vector3(origin.x, origin.y, origin.z),
			rn::vector3(origin_copy.x, origin_copy.y, origin_copy.z), &ray);

		if (ray.fraction == 1.0f) {
			client->force_jump(6); // +jump
		}
	} while (vars::is_running);
}
