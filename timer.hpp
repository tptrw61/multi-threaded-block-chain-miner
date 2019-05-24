#include <chrono>

using TimePoint = std::chrono::steady_clock::time_point;
using TimeResolution = int;

class Timer {
    TimePoint m_start, m_end, m_last;
    bool m_ended;
public:
    static const TimeResolution MICRO = 0;
    static const TimeResolution MILLI = 1;
    static const TimeResolution SECOND = 2;
    static const TimeResolution MINUTE = 3;
    static const TimeResolution HOUR = 4;
    static const TimeResolution DAY = 5;
    
    static const TimeResolution DEFAULT_RESOLUTION = MILLI;
    static const TimeResolution MIN_RESOLUTION = MICRO;
    static const TimeResolution MAX_RESOLUTION = DAY;

    const bool ended = &m_ended;
    
    Timer();
    Timer(TimePoint start);
    
    void start();
    size_t lap(); //returns in ms
    size_t end(); //returns in ms
    
    //microsecond versions of above
    size_t lap_us();
    size_t end_us();
    
    //getters
    TimePoint startTime();
    TimePoint lapTime();
    TimePoint endTime();
    
    //turns object into size_t of a unit of time
    size_t micros();
    size_t millis();
    size_t secs();
    size_t mins();
    size_t hours();
    size_t days();
    
    //static version of above
    static size_t micros(TimePoint start, TimePoint end);
    static size_t millis(TimePoint start, TimePoint end);
    static size_t secs(TimePoint start, TimePoint end);
    static size_t mins(TimePoint start, TimePoint end);
    static size_t hours(TimePoint start, TimePoint end);
    static size_t days(TimePoint start, TimePoint end);
    
    //resolution is the smallest measure of time wanted in the string (except in definition 3 where it is the unit of dur)
    //extendTo is the biggest time unit that will be allowed be filled with leading zeros
    //example: Timer::toString(7, SECOND, MINUTE) returns "00:07"
    //example: Timer::toString(3600 + 120, SECOND, MINUTE) returns "01:02:00"
    std::string toString(TimeResolution resolution = DEFAULT_RESOLUTION, TimeResolution extendTo = SECOND);
    static std::string toString(TimePoint start, TimePoint end, TimeResolution resolution = DEFAULT_RESOLUTION, TimeResolution extendTo = SECOND);
    static std::string toString(size_t dur, TimeResolution resolution, TimeResolution extendTo = SECOND);
};

Timer::Timer() {
    m_ended = false;
    m_start = std::chrono::steady_clock::now();
}
Timer::Timer(TimePoint startTime) {
    m_ended = false;
    m_start = startTime;
}

void Timer::start() {
    m_ended = false;
    m_start = std::chrono::steady_clock::now();
}
size_t Timer::lap() {
    m_last = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_last - m_start).count();
}
size_t Timer::end() {
    m_end = std::chrono::steady_clock::now();
    m_ended = true;
    return std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count();
}

size_t Timer::lap_us() {
    m_last = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(m_last - m_start).count();
}
size_t Timer::end_us() {
    m_end = std::chrono::steady_clock::now();
    m_ended = true;
    return std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count();
}

TimePoint Timer::startTime() {
    return m_start;
}
TimePoint Timer::lapTime() {
    return m_last;
}
TimePoint Timer::endTime() {
    return m_end;
}

size_t Timer::micros() {
    return micros(m_ended ? m_end : m_last, m_start);
}
size_t Timer::millis() {
    return millis(m_ended ? m_end : m_last, m_start);
}
size_t Timer::secs() {
    return secs(m_ended ? m_end : m_last, m_start);
}
size_t Timer::mins() {
    return mins(m_ended ? m_end : m_last, m_start);
}
size_t Timer::hours() {
    return hours(m_ended ? m_end : m_last, m_start);
}
size_t Timer::days() {
    return days(m_ended ? m_end : m_last, m_start);
}


size_t Timer::micros(TimePoint start, TimePoint end) {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}
size_t Timer::millis(TimePoint start, TimePoint end) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}
size_t Timer::secs(TimePoint start, TimePoint end) {
    return std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
}
size_t Timer::mins(TimePoint start, TimePoint end) {
    return std::chrono::duration_cast<std::chrono::minutes>(end - start).count();
}
size_t Timer::hours(TimePoint start, TimePoint end) {
    return std::chrono::duration_cast<std::chrono::hours>(end - start).count();
}
size_t Timer::days(TimePoint start, TimePoint end) {
    return (Timer::hours(start, end) / 24.0) + .5;
}

std::string Timer::toString(TimeResolution resolution, TimeResolution extendTo) {
    return toString(m_start, m_ended ? m_end : m_last, resolution, extendTo);
}
std::string Timer::toString(TimePoint start, TimePoint end, TimeResolution resolution, TimeResolution extendTo) {
    size_t dur = 0;
    if (resolution == Timer::MICRO)
        dur = micros(start, end);
    if (resolution == Timer::MILLI)
        dur = millis(start, end);
    if (resolution == Timer::SECOND)
        dur = secs(start, end);
    if (resolution == Timer::MINUTE)
        dur = mins(start, end);
    if (resolution == Timer::HOUR)
        dur = hours(start, end);
    if (resolution == Timer::DAY)
        dur = days(start, end);
    return toString(dur, resolution, extendTo);
}
std::string Timer::toString(size_t dur, TimeResolution resolution, TimeResolution extendTo) {
    if (resolution > Timer::MAX_RESOLUTION || resolution < Timer::MIN_RESOLUTION)
        return std::string();
    std::string str = "";
    int divs[] = {1000, 1000, 60, 60, 24}; //div and mod by to get to next resolution
    int lens[] = {3, 3, 2, 2, 2}; //length of each part
    for (int i = resolution; i <= Timer::MAX_RESOLUTION; ++i) {
        if (i == Timer::MAX_RESOLUTION) {
            str = std::to_string(dur) + str;
            break;
        } else {
            auto val = dur % divs[i];
            dur /= divs[i];
            std::string sv = std::to_string(val);
            while (sv.length() < (unsigned)lens[i])
                sv = "0" + sv;
            str = sv + str;
        }
        if (dur == 0 && i >= extendTo)
            break;
        if (i + 1 > Timer::SECOND) //+1 because above if statement moved to next time unit
            str = ":" + str;
        if (i + 1 == Timer::SECOND) //^^^
            str = "." + str;
    }
    return str;
}

