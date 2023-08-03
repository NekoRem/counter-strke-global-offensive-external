#include "../../includes.hpp"

std::int32_t cs::sdk::c_entity::get_health() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_iHealth);
}

std::int32_t cs::sdk::c_entity::get_team() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_iTeamNum);
}

std::int32_t cs::sdk::c_entity::get_flags() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_fFlags);
}

std::int32_t cs::sdk::c_entity::glow_index() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_iGlowIndex);
}

std::int32_t cs::sdk::c_entity::crosshair_id() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_iCrosshairId);
}

bool cs::sdk::c_entity::is_dormant() const
{
	return memory::read<bool>(address + sdk::offsets::m_bDormant);
}

bool cs::sdk::c_entity::is_scoped() const
{
	return memory::read<bool>(address + sdk::netvars::m_bIsScoped);
}

bool cs::sdk::c_entity::is_spotted() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_bSpottedByMask);
}

bool cs::sdk::c_entity::is_visible_ray() const
{
	return vars::bsp::parsed_map ? vars::bsp::bp->is_visible(rn::vector3(local.eye_pos().x,
		local.eye_pos().y, local.eye_pos().z), rn::vector3(this->eye_pos().x,
			this->eye_pos().y, this->eye_pos().z)) : false;
}

bool cs::sdk::c_entity::has_immunity() const
{
	return memory::read<bool>(address + sdk::netvars::m_bGunGameImmunity);
}

std::int32_t cs::sdk::c_entity::move_type() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_MoveType);
}

std::int32_t cs::sdk::c_entity::life_state() const
{
	return memory::read<std::int32_t>(address + sdk::netvars::m_lifeState);
}

vector cs::sdk::c_entity::get_velocity() const
{
	return memory::read<vector>(address + sdk::netvars::m_vecVelocity);
}

std::int32_t cs::sdk::c_entity::get_class_id() const
{
	const auto IClientNetworkable = memory::read<std::uintptr_t>(address + 0x8);		  // IClientNetworkable vtable
	const auto GetClientClass = memory::read<std::uintptr_t>(IClientNetworkable + 2 * 0x4); // 3rd function in the vtable (GetClientClass)
	const auto GetClientClass_ptr = memory::read<std::uintptr_t>(GetClientClass + 0x1);	  // pointer to the ClientClass struct out of the mov eax

	return memory::read<std::int32_t>(GetClientClass_ptr + 0x14); // ClassID
}

vector cs::sdk::c_entity::bone_matrix(int bone) const
{
	const auto matrix = memory::read<std::uintptr_t>(address + sdk::netvars::m_dwBoneMatrix);

	vector vec_matrix = {
		memory::read<float>(matrix + 0x30 * bone + 0xC),
		memory::read<float>(matrix + 0x30 * bone + 0x1C),
		memory::read<float>(matrix + 0x30 * bone + 0x2C) };

	return vec_matrix;
}

vector cs::sdk::c_entity::eye_pos() const
{
	return memory::read<vector>(address + sdk::netvars::m_vecOrigin) + memory::read<vector>(address + sdk::netvars::m_vecViewOffset);
}

vector cs::sdk::c_entity::view_offset() const {
	return memory::read<vector>(address + sdk::netvars::m_vecViewOffset);
}

vector cs::sdk::c_entity::origin() const {
	return memory::read<vector>(address + sdk::netvars::m_vecOrigin);
}

vector cs::sdk::c_entity::vecmins() const
{
	return memory::read<vector>((address + sdk::netvars::m_Collision) + sdk::netvars::m_vecMins);
}

vector cs::sdk::c_entity::vecmaxs() const
{
	return memory::read<vector>((address + sdk::netvars::m_Collision) + sdk::netvars::m_vecMaxs);
}

vector cs::sdk::c_entity::aim_punch() const
{
	return memory::read<vector>(address + sdk::netvars::m_aimPunchAngle);
}

bool cs::sdk::c_entity::is_alive() const
{
	return (this->life_state() == sdk::structs::e_life_state::LIFE_ALIVE);
}

bool cs::sdk::c_entity::is_enemy() const
{
	auto mp_teammates_are_enemies = convar->find(_("mp_teammates_are_enemies"));
	if (!mp_teammates_are_enemies.get_pointer())
		return false;

	auto teammates_are_enemies = mp_teammates_are_enemies.get_int();
	teammates_are_enemies ^= mp_teammates_are_enemies.get_pointer();

	return teammates_are_enemies ? true : this->get_team() != local.get_team();
}

std::uintptr_t cs::sdk::c_entity::get_view_model() const
{
	return memory::read<std::uintptr_t>(local.get_entity() + sdk::netvars::m_hViewModel);
}

bool cs::sdk::c_entity::is_localplayer() const
{
	return (address == local.get_entity());
}

int cs::sdk::c_entity::get_observer_target() const
{
	return memory::read<int>(address + sdk::netvars::m_hObserverTarget) & 0xfff;
}

cs::sdk::c_weapon cs::sdk::c_entity::get_weapon() const
{
	const auto current_weapon = memory::read<std::uintptr_t>(this->address + sdk::netvars::m_hActiveWeapon);
	if (!current_weapon)
		return c_weapon(NULL);

	const auto current_weapon_entity = memory::read<std::uintptr_t>(memory::client_dll + sdk::offsets::dwEntityList + ((current_weapon & 0xFFF) - 1) * 0x10);
	if (!current_weapon_entity)
		return c_weapon(NULL);

	return c_weapon(current_weapon_entity);
}

cs::sdk::structs::playerinfo_t cs::sdk::c_entity::get_player_info() const {
	const auto player_info_items = memory::read<std::uintptr_t>(memory::read<std::uintptr_t>(cs::engine->get_user_info_table() + 0x40) + 0xC);
	return memory::read<sdk::structs::playerinfo_t>(memory::read<std::uintptr_t>(player_info_items + 0x28 + (this->index * 0x34)));
}

cs::sdk::structs::bounding_box_t cs::sdk::c_entity::get_bounding_box() const
{
	vector2d flb, brt, blb, frt, frb, brb, blt, flt;

	float left, top, right, bottom;

	vector origin = this->origin()/* - vector(0.0f, 0.0f, 8.0f)*/;
	vector min = vecmins() + origin;
	vector max = vecmaxs() + origin;

	vector points[] = {
			vector(min.x, min.y, min.z),
			vector(min.x, max.y, min.z),
			vector(max.x, max.y, min.z),
			vector(max.x, min.y, min.z),
			vector(max.x, max.y, max.z),
			vector(min.x, max.y, max.z),
			vector(min.x, min.y, max.z),
			vector(max.x, min.y, max.z) };

	world_to_screen(points[3], flb);
	world_to_screen(points[5], brt);
	world_to_screen(points[0], blb);
	world_to_screen(points[4], frt);
	world_to_screen(points[2], brb);
	world_to_screen(points[1], brb);
	world_to_screen(points[6], blt);
	world_to_screen(points[7], flt);

	vector2d arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	if (left || top || right || bottom) {
		for (auto x = 1; x < 4; ++x) {
			if (left > arr[x].x)
				left = arr[x].x;
			if (bottom < arr[x].y)
				bottom = arr[x].y;
			if (right < arr[x].x)
				right = arr[x].x;
			if (top > arr[x].y)
				top = arr[x].y;
		}
		return structs::bounding_box_t(left, top, right - left, bottom - top);
	}
	return structs::bounding_box_t();
}

uintptr_t cs::sdk::c_entity::get_studiohdr() const
{
	return memory::read<uintptr_t>(address + sdk::offsets::m_pStudioHdr);
}

cs::sdk::structs::studiohdr_t cs::sdk::c_entity::get_studiohdr2() const
{
	return memory::read<structs::studiohdr_t>(get_studiohdr());
}

std::array<matrix3x4_t, 128> cs::sdk::c_entity::get_bonematrix() const
{
	return memory::read< std::array < matrix3x4_t, 128>>(address + sdk::netvars::m_dwBoneMatrix);
}
