#pragma once

namespace cs
{
	class c_engine
	{
	public: // read
		std::uintptr_t get_client_state() const;
		sdk::structs::globalvars_t get_globalvars() const;
		std::int32_t in_game() const;
		std::int32_t get_max_player_count() const;
		vector get_view_angles() const;
		const char* get_map_name();
		const char* get_game_directory();
	public: // write
		bool set_view_angles(vector view_angles) const;
		std::int32_t force_full_update() const;
		std::uintptr_t get_user_info_table() const;
	};

	inline auto engine = std::make_unique<c_engine>();
}