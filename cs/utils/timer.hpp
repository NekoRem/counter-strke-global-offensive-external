#pragma once

namespace timer
{
    inline void sleep(float ms)
    {
        static std::once_flag flag;
        std::call_once(flag, []()
            {
                ULONG current;
                cs::memory::zwsettimerresolution((ULONG)(0.5f * 10000.f), true, &current); });

        if (ms < 0.5f)
            ms = 0.5f;

        LARGE_INTEGER time = {};
        time.QuadPart = -1 * (LONGLONG)(ms * 10000.f);

        cs::memory::ntdelayexecution(false, &time);
    }
}