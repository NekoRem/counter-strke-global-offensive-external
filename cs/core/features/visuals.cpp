#include "../../includes.hpp"

void cs::features::visuals::run()
{
	if (entities.empty())
		return;

	if (!engine->in_game())
		return;

	int32_t local_team = local.get_team();

	std::lock_guard guard(entities_mutex);
	for (const auto& entity : entities)
	{
		int health = entity.get_health();
		bool alive = entity.is_alive();
		vector origin = entity.origin();
		int32_t team = entity.get_team();

		if (!alive || team == local_team)
			continue;

		sdk::structs::playerinfo_t info = entity.get_player_info();
		sdk::structs::bounding_box_t bbox = entity.get_bounding_box();
		if (!bbox.x || !bbox.y)
			continue;

		aero::color color = aero::color(255, 100, 100, 255);
		if (entity.is_visible_ray())
			color = aero::color(255, 255, 255, 255);

		if (aim_target.get_entity() && entity == aim_target)
			color = aero::color(100, 100, 255);

		visuals::box(entity, bbox, color);
		visuals::info(entity, bbox, info);
		visuals::aimbone(entity, color);
	}
}

void cs::features::visuals::box(sdk::c_entity entity, sdk::structs::bounding_box_t bbox, aero::color color)
{
	window::surface->border_box(bbox.x, bbox.y, bbox.w, bbox.h, 1.0f, color, 1.0f, 1.0f);
}

void cs::features::visuals::info(sdk::c_entity entity, sdk::structs::bounding_box_t bbox, sdk::structs::playerinfo_t info)
{
	float x, y = 0; // center the text (reusable)
	window::surface->get_text_size<char>(info.szName, window::font, &x, &y);
	//window::surface->rect(bbox.x, bbox.y - y, bbox.w, y, aero::color(255, 100, 100, 255), 1.0f, aero::color(0, 0, 0, 255));
	window::surface->text_ansii(bbox.x + (bbox.w * 0.5f) - x * 0.5f, bbox.y - y - 1, window::font, aero::color(255, 255, 255, 255), info.szName, true);

	sdk::c_weapon weapon = entity.get_weapon();
	if (!weapon.get_weapon_address())
		return;

	std::string weapon_name = weapon.get_weapon_name_from_index();
	window::surface->get_text_size<char>(weapon_name, window::font, &x, &y);
	window::surface->text_ansii(bbox.x + (bbox.w * 0.5f) - x * 0.5f, (bbox.y + bbox.h), window::font, aero::color(255, 255, 255, 255), weapon_name, true);
}

void cs::features::visuals::aimbone(sdk::c_entity entity, aero::color color)
{
	if (aim_bone.IsZero())
		return;

	vector2d bone_scrn{};
	if (!sdk::world_to_screen(aim_bone, bone_scrn))
		return;

	window::surface->rect(bone_scrn.x - 2, bone_scrn.y - 2, 4, 4, color, 1.0f);
}

void cs::features::visuals::spectator_list(sdk::c_entity entity)
{
	uintptr_t spectator = client->get_entity_from_handle(entity.get_observer_target());

	static int spectator_count = 0;
	if (spectator == local.get_entity())
	{
		spectator_count++;

		if (spectator_count > 0)
		{
			sdk::structs::playerinfo_t info = entity.get_player_info();
			window::surface->text_ansii(20, window::overlay->_height * 0.5f + (20 * spectator_count), window::font, aero::color(0xffffff), info.szName, true);
		}
	}
}