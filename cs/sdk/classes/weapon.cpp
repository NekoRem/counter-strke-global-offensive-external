#include "../../includes.hpp"

// Thanks to: https://www.unknowncheats.me/forum/counterstrike-global-offensive/408596-ccsweapondata-itemdefinitionindex-internal-external.html
cs::sdk::structs::weapon_data_t cs::sdk::c_weapon::get_weapon_data() const {
	auto weapon_table = memory::read<sdk::structs::weapon_info_table_t>(memory::client_dll + sdk::offsets::dwWeaponTable);

	for (std::int16_t i = 0; i <= weapon_table.last_index; i++)
	{
		auto current_weapon = memory::read<sdk::structs::weapon_info_table_object_t>((std::uintptr_t)weapon_table.weapon_table + sizeof(sdk::structs::weapon_info_table_object_t) * i); // weapon_data_t* pWeaponInfo; 0x000C WeaponInfo*
		if (current_weapon.item_definition_index == this->index)
			return memory::read<sdk::structs::weapon_data_t>((std::uintptr_t)current_weapon.weapon_info);
	}
	return {};
}

std::int32_t cs::sdk::c_weapon::get_table_index() const {
	return memory::read<std::int32_t>(this->address + sdk::offsets::dwWeaponTableIndex);
}

std::string cs::sdk::c_weapon::get_weapon_name_from_index() const {
	for (const auto& [weapon_id, weapon_info] : sdk::structs::m_item_list)
	{
		if (weapon_id == this->index)
			return std::string(weapon_info.name);
	}
	return {};
}