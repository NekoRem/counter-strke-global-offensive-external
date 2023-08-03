#pragma once

namespace cs
{
	namespace features
	{
		inline sdk::c_entity aim_target = {};
		inline vector aim_bone = {};

		namespace visuals
		{
			void run();

			void box(sdk::c_entity entity, sdk::structs::bounding_box_t bbox, aero::color color);
			void info(sdk::c_entity entity, sdk::structs::bounding_box_t bbox, sdk::structs::playerinfo_t info);
			void aimbone(sdk::c_entity entity, aero::color color);
			void spectator_list(sdk::c_entity entity);
		}

		namespace misc
		{
			void bunnyhop();
			void edgejump();
		}

		namespace aimbot
		{
			void run();

			cs::sdk::c_entity find_target();
			vector calculate_angle(vector& source, vector& destination, vector& view_angles);
			int32_t find_nearest_bone(sdk::c_entity entity, vector local_eye_pos, vector viewangles);
			void mouse_aimbot(vector aim_angle);
		}

		namespace skinchanger
		{
			void run();

			void populate_models();
			int32_t find_model_index_by_name(std::string_view model_name);
			inline std::map<std::string, std::int32_t> model_index_list = {};
		}
	}
}