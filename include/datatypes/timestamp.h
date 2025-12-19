#pragma once

// The types used for the duration of the timers. It is set to float for now, but might be changed to a double later
typedef float sre_timeStamp;

#ifdef __cplusplus

    namespace sre
    {
        using timeStamp = sre_timeStamp;
    }

    constexpr sre::timeStamp operator ""_ts(long double n) { return static_cast<sre::timeStamp>(n); }

#endif