#include "../../includes.hpp"

std::uintptr_t cs::c_engine::get_client_state() const
{
	return memory::read<uintptr_t>(memory::engine_dll + sdk::offsets::dwClientState);
}

cs::sdk::structs::globalvars_t cs::c_engine::get_globalvars() const
{
	return memory::read<sdk::structs::globalvars_t>(memory::engine_dll + sdk::offsets::dwGlobalVars);
}

std::int32_t cs::c_engine::in_game() const
{
	return memory::read<int32_t>(this->get_client_state() + sdk::offsets::dwClientState_State) == SIGNONSTATE_FULL;
}

std::int32_t cs::c_engine::get_max_player_count() const
{
	return memory::read<int32_t>(this->get_client_state() + sdk::offsets::dwClientState_MaxPlayer);
}

vector cs::c_engine::get_view_angles() const
{
	return memory::read<vector>(this->get_client_state() + sdk::offsets::dwClientState_ViewAngles);
}

const char* cs::c_engine::get_map_name()
{
	std::array<char, 0x80> map_name = memory::read<std::array<char, 0x80>>(this->get_client_state() + sdk::offsets::dwClientState_MapDirectory);
	return map_name.data();
}

const char* cs::c_engine::get_game_directory()
{
	static std::array<char, 0x120> game_directory = memory::read<std::array<char, 0x120>>(memory::engine_dll + sdk::offsets::dwGameDir);
	return game_directory.data();
}

bool cs::c_engine::set_view_angles(vector view_angles) const
{
	return memory::write<vector>(this->get_client_state() + sdk::offsets::dwClientState_ViewAngles, view_angles);
}

std::int32_t cs::c_engine::force_full_update() const
{
	return memory::write<int32_t>(this->get_client_state() + sdk::offsets::clientstate_delta_ticks, -1);
}

std::uintptr_t cs::c_engine::get_user_info_table() const
{
	return memory::read<uintptr_t>(this->get_client_state() + sdk::offsets::dwClientState_PlayerInfo);
}
