#include "includes.hpp"

int main()
{
	if (!cs::memory::setup_winapi())
		return 0;

	do {
		cs::memory::target = LI_FN(FindWindowA).safe_cached()(_("Valve001"), 0);
		std::cout << _("waiting for game window..\n");
		timer::sleep(1000);
	} while (!cs::memory::target);

	if (!cs::memory::setup())
		return 0;

	std::thread([] { cs::on_world_cache(); }).detach();
	std::thread([] { cs::on_entity_cache(); }).detach();
	std::thread([] { cs::features::misc::bunnyhop(); }).detach();
	std::thread([] { cs::features::aimbot::run(); }).detach(); 
	//std::thread([] { cs::features::skinchanger::run(); }).detach();

	cs::window::overlay = std::make_unique<aero::overlay>();
	cs::window::status = cs::window::overlay->attach(cs::memory::target);

	if (cs::window::status != aero::api_status::success)
	{
		std::cerr << _("failed to attach to target window.\n");
		return 0;
	}

	cs::window::surface = cs::window::overlay->get_surface();
	cs::window::font = cs::window::surface->add_font("text", "Tahoma", 11.f, 400);
	while (cs::window::overlay->message_loop() && cs::vars::is_running) 
	{
		cs::window::overlay->scale(); // fix window
		if (cs::window::surface->begin_scene()) 
		{
			cs::features::visuals::run();
			cs::window::surface->end_scene();
		}
		timer::sleep(2);
	}
	cs::window::overlay->destroy();
	return 1;
}