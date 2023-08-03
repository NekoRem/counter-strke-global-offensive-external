#pragma once

namespace cs
{
	inline std::vector<sdk::c_entity> entities = {};
	inline std::mutex entities_mutex = {};

	namespace vars
	{
		inline bool is_running = true;

		namespace bsp
		{
			inline bool parsed_map = false;
			inline std::string current_map;
			inline rn::bsp_parser* bp = new rn::bsp_parser();
		}

		namespace visuals
		{
			inline bool enabled = true;
			
			inline bool box = true;
			inline bool name = true;
			inline bool info = true;
		}

		namespace misc
		{
			inline bool enabled = false;

			inline bool bunnyhop = false;
			inline bool edgejump = false;
		}

		namespace aimbot
		{
			inline bool enabled = true;

			inline float smoothing = 18.0f;
			inline float fov = 6.0f;
			inline float rcs_x = 40.0f;
			inline float rcs_y = 60.0f;
			inline float sens = 1.40f;
		}

		namespace skinchanger
		{
			inline bool enabled = true;

			inline bool selected_paintkit = true;
			inline int selected_model_index = 0;

			inline std::string paint_kit = {};
			inline std::vector<const char*> models = {};
		}
	}

	namespace window
	{
		inline std::unique_ptr<aero::overlay> overlay = {};
		inline aero::surface_ptr surface = {};
		inline aero::api_status status = {};
		inline aero::font_ptr font = {};
	}
}