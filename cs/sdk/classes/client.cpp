#include "../../includes.hpp"

std::tuple<std::uintptr_t, std::int32_t> cs::c_client::get_local_player() const
{
    const auto local_player_index = memory::read<std::int32_t>(engine->get_client_state() + sdk::offsets::dwClientState_GetLocalPlayer);
    const auto local_player_address = memory::read<std::uintptr_t>(memory::client_dll + sdk::offsets::dwEntityList + 0x10 * local_player_index);

    return { local_player_address, local_player_index };
}

std::uintptr_t cs::c_client::get_entity_from_index(int index) const
{
    return cs::memory::read<uintptr_t>(cs::memory::client_dll + sdk::offsets::dwEntityList + (index * 0x10));
}

std::uintptr_t cs::c_client::get_entity_from_handle(uintptr_t handle) const
{
    const auto entity = memory::read<std::uintptr_t>(memory::client_dll + sdk::offsets::dwEntityList + ((handle & 0xfff) - 1) * 0x10);
    if (!entity)
        return {};
    return entity;
}

bool cs::c_client::in_menu() const
{
    CURSORINFO ci{ sizeof(CURSORINFO) };
    if (!LI_FN(GetCursorInfo).safe_cached()(&ci))
        return false;

    const auto handle = ci.hCursor;
    if ((handle > (HCURSOR)50000) && (handle < (HCURSOR)100000))
        return true;

    return false;
}

std::int32_t cs::c_client::get_game_type() const
{
    const auto game_rules_proxy = memory::read<std::uintptr_t>(memory::client_dll + sdk::offsets::dwGameRulesProxy);
    const auto game_type = memory::read<std::int32_t>(game_rules_proxy + sdk::netvars::m_SurvivalGameRuleDecisionTypes);

    return game_type;
}

std::uintptr_t cs::c_client::get_glow_object_manager() const
{
    return memory::read<std::uintptr_t>(memory::client_dll + sdk::offsets::dwGlowObjectManager);
}

std::int32_t cs::c_client::get_force_jump() const
{
    return memory::read<std::int32_t>(memory::client_dll + sdk::offsets::dwForceJump);
}

std::int32_t cs::c_client::get_force_attack() const
{
    return memory::read<std::int32_t>(memory::client_dll + sdk::offsets::dwForceAttack);
}

std::int32_t cs::c_client::force_jump(const std::int32_t state) const
{
    return memory::write<std::int32_t>(memory::client_dll + sdk::offsets::dwForceJump, state);
}

std::int32_t cs::c_client::force_attack(const std::int32_t state) const
{
    return memory::write<std::int32_t>(memory::client_dll + sdk::offsets::dwForceAttack, state);
}
