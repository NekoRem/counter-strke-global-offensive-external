#pragma once
namespace cs
{
	namespace sdk
	{
		class c_entity
		{
		private:
			int32_t index = 0;
			uintptr_t address = 0;
			int32_t aimbot_bone = 0;
		public:
			c_entity() = default;
			c_entity(const uintptr_t addr, const int32_t idx = 0)
			{
				this->address = addr;
				this->index = idx;
			}
			bool operator==(const c_entity& entity) const
			{
				return this->address == entity.address;
			}
		public:
			constexpr auto& get_entity() const {
				return this->address;
			}

			constexpr auto& get_entity_index() const {
				return this->index;
			}

			std::int32_t get_health() const;
			std::int32_t get_team() const;
			std::int32_t get_flags() const;
			std::int32_t glow_index() const;
			std::int32_t crosshair_id() const;
			bool is_dormant() const;
			bool is_scoped() const;
			bool is_spotted() const;
			bool is_visible_ray() const;
			bool has_immunity() const;
			std::int32_t move_type() const;
			std::int32_t life_state() const;
			vector get_velocity() const;
			std::int32_t get_class_id() const;
			vector bone_matrix(int bone) const;
			vector eye_pos() const;
			vector view_offset() const;
			vector origin() const;
			vector vecmins() const;
			vector vecmaxs() const;
			bool is_alive() const;
			bool is_enemy() const;
			std::uintptr_t get_view_model() const;
			vector aim_punch() const;
			bool is_localplayer() const;
			int get_observer_target() const;

			c_weapon get_weapon() const;
			structs::playerinfo_t get_player_info() const;
			structs::bounding_box_t get_bounding_box() const;
			uintptr_t get_studiohdr() const;
			structs::studiohdr_t get_studiohdr2() const;
			std::array < matrix3x4_t, 128> get_bonematrix() const;
		};
	}

	inline auto local = sdk::c_entity();
}