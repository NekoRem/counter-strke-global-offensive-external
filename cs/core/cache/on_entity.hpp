#pragma once

namespace cs
{
	inline void on_entity_cache()
	{
		do {
			if (!engine->in_game())
				continue;

			std::vector<sdk::c_entity> temp_list;
			for (int32_t idx = 1; idx < engine->get_max_player_count(); idx++)
			{
				uintptr_t ent = client->get_entity_from_index(idx);
				if (!ent)
					continue;

				sdk::c_entity entity(ent, idx);

				if (entity.is_dormant())
					continue;

				const auto list_iteration = std::find(temp_list.begin(), temp_list.end(), entity);
				if (list_iteration != temp_list.end())
				{
					temp_list.erase(list_iteration);
					continue;
				}
				else if (list_iteration != temp_list.end())
					continue;

				if (temp_list.size() > 500)
					temp_list.clear();

				temp_list.push_back(entity);
			}

			entities_mutex.lock();
			entities = temp_list;
			entities_mutex.unlock();

			timer::sleep(3000);
		} while (vars::is_running);
	}
}