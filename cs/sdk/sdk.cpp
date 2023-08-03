#include "../includes.hpp"

bool cs::sdk::world_to_screen(vector pos, vector2d& screen)
{
    float matrix[16];
    static auto pattern = memory::pattern_scan(_("client.dll"), _("0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9"), 3, 176, true, false);
    memory::ntreadvirtualmemory(memory::handle, (PFLOAT)pattern, &matrix, sizeof(matrix), 0);

    vector4d clip_coords = {};
    clip_coords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    clip_coords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    clip_coords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    clip_coords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clip_coords.w < 0.1f) {
        return false;
    }

    vector normal = {};
    normal.x = clip_coords.x / clip_coords.w;
    normal.y = clip_coords.y / clip_coords.w;
    normal.z = clip_coords.z / clip_coords.w;

    screen.x = ((cs::window::overlay->_width * 0.5f) * normal.x) + (normal.x + (cs::window::overlay->_width * 0.5f));
    screen.y = -((cs::window::overlay->_height * 0.5f) * normal.y) + (normal.y + (cs::window::overlay->_height * 0.5f));
    return true;
}
