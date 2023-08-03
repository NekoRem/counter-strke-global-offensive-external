#pragma once

namespace math
{
    inline bool angle_clamp(vector& angles)
    {
        if (isfinite(angles.x) &&
            isfinite(angles.y) &&
            isfinite(angles.z))
        {
            angles.x = std::clamp(angles.x, -89.0f, 89.0f);
            angles.y = std::clamp(angles.y, -180.0f, 180.0f);
            angles.z = 0.0f;
            return true;
        }

        return false;
    }

    inline bool angle_normalize(vector& angles)
    {
        if (isfinite(angles.x) &&
            isfinite(angles.y) &&
            isfinite(angles.z))
        {
            angles.x = remainder(angles.x, 360.0f);
            angles.y = remainder(angles.y, 360.0f);
            return true;
        }

        return false;
    }

    inline bool angle_check(const vector& angles)
    {
        if (angles.x > 89.0f
            || angles.x < -89.0f
            || angles.y > 180.0f
            || angles.y < -180.0f
            || angles.z != 0.0f)
        {
            return false;
        }

        return true;
    }

    inline bool angle_sanitize(vector& angles)
    {
        if (!angle_normalize(angles)
            || !angle_clamp(angles))
        {
            return false;
        }

        if (!angle_check(angles))
        {
            return false;
        }

        return true;
    }
}