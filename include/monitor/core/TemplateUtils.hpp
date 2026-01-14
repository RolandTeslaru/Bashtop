#pragma once

namespace monitor::core {

    // Clamps values
    template <typename T>
    T clamp_value(T v, T lo, T hi) {
        if (v < lo)
            return lo;
        if (v > hi)
            return hi;
        return v;
    }

}
