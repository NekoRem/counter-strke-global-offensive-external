#include "../../includes.hpp"

void cs::features::aimbot::run()
{
	do {
		if (!vars::aimbot::enabled)
			continue;

		if (!(memory::target == LI_FN(GetForegroundWindow).safe_cached()()))
			continue;

		if (client->in_menu())
			continue;

		if (!engine->in_game())
			continue;

		static int last_tick = {};
		static int last_frame = {};
		static float function_elapsed = {};

		if (!(LI_FN(GetAsyncKeyState).safe_cached()(VK_LBUTTON) & 0x8000))
		{
			aim_target = sdk::c_entity();
			aim_bone = vector();
			timer::sleep(1);
			continue;
		}

		// only update each tick
		const auto global_vars = engine->get_globalvars();
		const auto update = (global_vars.iTickCount != last_tick || global_vars.iFrameCount != last_frame);
		if (!update)
		{
			timer::sleep(global_vars.flIntervalPerTick);
			continue;
		}

		const auto frametime = global_vars.flAbsFrameTime;
		const auto delay = (function_elapsed - (frametime < global_vars.flIntervalPerTick ? (frametime * 0.5f) : frametime));
		const auto sleep = std::min(delay, (frametime * 1000));

		timer::sleep(sleep);
		const auto start = std::chrono::high_resolution_clock::now();

		aim_target = find_target();
		if (!aim_target.get_entity())
			continue;

		vector local_eye = local.eye_pos();
		vector view_angles = engine->get_view_angles();
		vector aim_angle = calculate_angle(local_eye, aim_bone, view_angles);
		aim_angle /= vars::aimbot::smoothing;
		aim_angle = view_angles += aim_angle;

		math::angle_sanitize(aim_angle);

		mouse_aimbot(aim_angle);

		const auto end = std::chrono::high_resolution_clock::now();

		// update last frame and last tick
		last_tick = global_vars.iTickCount;
		last_frame = global_vars.iFrameCount;

		std::chrono::duration<float, std::milli> elapsed = end - start;
		function_elapsed = elapsed.count();
	} while (vars::is_running);
}

cs::sdk::c_entity cs::features::aimbot::find_target()
{
	float best_fov = vars::aimbot::fov;
	cs::sdk::c_entity best_target{};
	int32_t local_team = local.get_team();

	for (int32_t idx = 1; idx < engine->get_max_player_count(); idx++)
	{
		uintptr_t ent = client->get_entity_from_index(idx);
		sdk::c_entity entity(ent, idx);

		bool alive = entity.is_alive();
		bool immunity = entity.has_immunity();
		int32_t team = entity.get_team();

		if (immunity || !alive || team == local_team)
			continue;

		vector local_eye_pos = local.eye_pos();
		vector view_angles = engine->get_view_angles();
		vector bone = entity.bone_matrix(find_nearest_bone(entity, local_eye_pos, view_angles));
		vector aim_angles = calculate_angle(local_eye_pos, bone, view_angles);
		math::angle_normalize(aim_angles);

		float fov = std::hypotf(aim_angles.x, aim_angles.y);
		if (fov < best_fov)
		{
			best_fov = fov;
			best_target = entity;
			aim_bone = bone;
		}
	}
	return best_target;
}

int32_t cs::features::aimbot::find_nearest_bone(sdk::c_entity entity, vector local_eye_pos, vector viewangles)
{
	float best_dist = 360.f;
	int aim_bone = 8;

	std::vector<uint32_t> target_bones{ 8, 7, 6, 5, 3 };

	for (int i : target_bones)
	{
		vector target_pos = entity.bone_matrix(i);
		auto angle = calculate_angle(local_eye_pos, target_pos, viewangles);
		math::angle_normalize(angle);
		auto this_dist = std::hypotf(angle.x, angle.y);

		if (best_dist > this_dist)
		{
			best_dist = this_dist;
			aim_bone = i;
			continue;
		}
	}

	return aim_bone;
}

vector cs::features::aimbot::calculate_angle(vector& source, vector& destination, vector& view_angles)
{
	vector delta = source - destination;
	auto radians_to_degrees = [](float radians)
	{
		return radians * 180 / static_cast<float>(3.14159265358979323846);
	};

	vector angles = {};
	angles.x = radians_to_degrees(atanf(delta.z / std::hypot(delta.x, delta.y))) - view_angles.x;
	angles.y = radians_to_degrees(atanf(delta.y / delta.x)) - view_angles.y;
	angles.z = 0.0f;

	if (delta.x >= 0.0f)
		angles.y += 180.0f;

	sdk::c_weapon weapon = local.get_weapon();
	int32_t weaponid = weapon.get_weapon_index();

	bool recoil_control = true;
	switch (weaponid)
	{
	case sdk::structs::WEAPON_AWP:
	case sdk::structs::WEAPON_DEAGLE:
	case sdk::structs::WEAPON_SSG08:
		recoil_control = false;
	}

	if (recoil_control)
	{
		vector delta = {};
		vector punch = {};
		
		punch = local.aim_punch() * 2.0f;

		delta.x = punch.x;
		delta.y = punch.y;

		angles.x -= delta.x * (vars::aimbot::rcs_x / 100);
		angles.y -= delta.y * (vars::aimbot::rcs_y / 100) + 0.05f;
	}

	math::angle_normalize(angles);
	return angles;
}

void cs::features::aimbot::mouse_aimbot(vector aim_angle)
{
	float sens = vars::aimbot::sens;
	vector view_angles = engine->get_view_angles();

	vector delta = {};
	delta.x = view_angles.x - aim_angle.x;
	delta.y = view_angles.y - aim_angle.y;

	vector mouse_angle = {};
	mouse_angle.x = (delta.x * -1.0f) / (0.022f * sens);
	mouse_angle.y = (delta.y) / (0.022f * sens);

	math::angle_sanitize(mouse_angle);

	input::move_mouse((int)mouse_angle.y, (int)mouse_angle.x);
}
