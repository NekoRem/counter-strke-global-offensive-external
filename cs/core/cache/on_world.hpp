#pragma once

namespace cs
{
	inline void on_world_cache()
	{
		do {
			if (!cs::local.get_entity())
			{
				const auto [addr, index] = cs::client->get_local_player();
				cs::local = cs::sdk::c_entity(addr, index);
			}

			if (!cs::engine->in_game())
			{
				cs::vars::bsp::current_map = ""; // trigger bsp parser when loading new world
				cs::vars::bsp::bp->unload_map();
				cs::vars::bsp::parsed_map = false;
				cs::local = cs::sdk::c_entity();
				cs::convar_list.clear();
				continue;
			}

			// parse bsp when in game
			std::string map = cs::engine->get_map_name();
			if (map != cs::vars::bsp::current_map)
			{
				cs::vars::bsp::current_map = map;
				cs::vars::bsp::parsed_map = cs::vars::bsp::bp->load_map(cs::engine->get_game_directory(), cs::engine->get_map_name());
				if (!cs::vars::bsp::parsed_map)
					std::cerr << _("failed to loadmap into bsp parser.\n");
			}

			if (vars::skinchanger::models.empty())
			{
				for (const auto& [weapon_id, weapon_info] : sdk::structs::m_item_list) {
					if (weapon_id > sdk::structs::WEAPON_KNIFE_BAYONET && weapon_id < sdk::structs::WEAPON_KNIFE_SKELETON)
						vars::skinchanger::models.emplace_back(weapon_info.name);
				}
			}

			if (features::skinchanger::model_index_list.empty())
				features::skinchanger::populate_models();

			//if (convar_list.empty())
				//convar->populate_convars();

			if (LI_FN(GetAsyncKeyState).safe_cached()(VK_END) & 0x8000)
				vars::is_running = false; // killswitch

			timer::sleep(1000);
		} while (cs::vars::is_running);
	}
}