#pragma once

namespace cs
{
	class c_client
	{
	public: // read
		std::tuple<std::uintptr_t, std::int32_t> get_local_player() const;
		std::uintptr_t get_entity_from_index(int index) const;
		std::uintptr_t get_entity_from_handle(uintptr_t handle) const;
		bool in_menu() const;
		std::int32_t get_game_type() const;
		std::uintptr_t get_glow_object_manager() const;
		std::int32_t get_force_jump() const;
		std::int32_t get_force_attack() const;

	public: // write
		std::int32_t force_jump(const std::int32_t state) const;
		std::int32_t force_attack(const std::int32_t state) const;
	};

	inline auto client = std::make_unique<c_client>();
}