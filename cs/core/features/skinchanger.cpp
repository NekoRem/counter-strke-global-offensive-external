#include "../../includes.hpp"

void cs::features::skinchanger::run()
{
	do {
		if (!vars::skinchanger::enabled) {
			timer::sleep(1);
			continue;
		}

		if (!engine->in_game())
			continue;

		if (!local.get_entity())
			continue;

		if (!local.is_alive())
			continue;

		if (!local.get_view_model())
			continue;

		if (vars::skinchanger::models.empty())
			continue;

		int target_knife = sdk::structs::e_item_definitions::WEAPON_KNIFE_KARAMBIT;
		int32_t model_index = find_model_index_by_name(_("models/weapons/v_knife_karam.mdl"));

		for (const auto& [weapon_id, weapon_info] : sdk::structs::m_item_list)
		{
			if (weapon_info.name == vars::skinchanger::models[vars::skinchanger::selected_model_index])
			{
				target_knife = weapon_id;
				model_index = find_model_index_by_name(weapon_info.model);
			}
		}

		for (size_t i = 0; i < 8; i++)
		{
			const auto current_weapon = memory::read<std::uintptr_t>((local.get_entity() + sdk::netvars::m_hMyWeapons) + i * 0x4);
			if (!current_weapon)
				continue;

			const auto current_weapon_entity = client->get_entity_from_handle(current_weapon);
			if (!current_weapon_entity)
				continue;

			const auto weapon_index = memory::read<std::int16_t>(current_weapon_entity + sdk::netvars::m_iItemDefinitionIndex);
			if (weapon_index == sdk::structs::WEAPON_KNIFE || weapon_index == sdk::structs::WEAPON_KNIFE_T || weapon_index == target_knife)
			{
				memory::write<std::int16_t>(current_weapon_entity + sdk::netvars::m_iItemDefinitionIndex, target_knife);
				memory::write<std::int32_t>(current_weapon_entity + sdk::netvars::m_nModelIndex, model_index);
				memory::write<std::int32_t>(current_weapon_entity + sdk::netvars::m_iViewModelIndex, model_index);
				memory::write<std::int32_t>(current_weapon_entity + sdk::netvars::m_iEntityQuality, 3);

				// skins
				if (vars::skinchanger::selected_paintkit && !vars::skinchanger::paint_kit.empty())
				{
					memory::write<std::int32_t>(current_weapon_entity + sdk::netvars::m_iItemIDHigh, -1);
					memory::write<std::int32_t>(current_weapon_entity + sdk::netvars::m_nFallbackPaintKit, std::stoi(vars::skinchanger::paint_kit));
					memory::write<float>(current_weapon_entity + sdk::netvars::m_flFallbackWear, 0.0001f);
				}
			}
		}

		const auto active_weapon = memory::read<std::uintptr_t>(local.get_entity() + sdk::netvars::m_hActiveWeapon);
		if (!active_weapon)
			continue;

		const auto active_weapon_entity = client->get_entity_from_handle(active_weapon);
		if (!active_weapon_entity)
			continue;

		const auto weapon_index = memory::read<std::int16_t>(active_weapon_entity + sdk::netvars::m_iItemDefinitionIndex);
		if (weapon_index != target_knife)
			continue;

		const auto view_model_entity = client->get_entity_from_handle(local.get_view_model());
		if (!view_model_entity)
			continue;

		memory::write<std::int32_t>(view_model_entity + sdk::netvars::m_nModelIndex, model_index);
	} while (vars::is_running);
}

void cs::features::skinchanger::populate_models()
{
	const auto client_state = engine->get_client_state();
	const auto network_string_table = memory::read<std::uintptr_t>(client_state + 0x52a4); // m_dwModelPrecache
	const auto network_string_dictionary = memory::read<std::uintptr_t>(network_string_table + 0x40);
	const auto network_string_dictionary_item_first = memory::read<std::uintptr_t>(network_string_dictionary + 0xC);

	for (int i = 0; i < 1024; i++)
	{
		const auto mpc_name_ptr = memory::read<std::uintptr_t>(network_string_dictionary_item_first + 0xC + (i * 0x34));
		if (!mpc_name_ptr)
			continue;

		//auto mpc_read_name = std::make_unique<char[]>(255);
		char mpc_read_name[255] = { 0 };
		if (memory::read(mpc_name_ptr, mpc_read_name, sizeof(255)))
			model_index_list[mpc_read_name] = i;
	}
}

int32_t cs::features::skinchanger::find_model_index_by_name(std::string_view model_name)
{
	const auto it = model_index_list.find(model_name.data());
	if (it == model_index_list.end())
		return {};
	return it->second;
}