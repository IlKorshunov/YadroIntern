#include "Time.h"
#include <iomanip>
#include <stdexcept>
#include <string>

Time::Time() : hours(0), minutes(0) {}
Time::Time(int h, int m) : hours(h), minutes(m) {}

Time::Time(const std::string &timeStr) {
    if (sscanf(timeStr.c_str(), "%d:%d", &hours, &minutes) != 2) { throw std::invalid_argument("Invalid time format"); }
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) { throw std::invalid_argument("Invalid time value"); }
}

Time::Time(int totalMinutes) { fromMinutes(totalMinutes); }

int Time::toMinutes() const { return hours * 60 + minutes; }

void Time::fromMinutes(int totalMinutes) {
    hours = totalMinutes / 60;
    minutes = totalMinutes % 60;
}

Time Time::operator-(const Time &other) const { return Time(toMinutes() - other.toMinutes()); }
Time Time::operator+(const Time &other) const { return Time(toMinutes() + other.toMinutes()); }
bool Time::operator<(const Time &other) const { return toMinutes() < other.toMinutes(); }
bool Time::operator>(const Time &other) const { return toMinutes() > other.toMinutes(); }
bool Time::operator<=(const Time &other) const { return toMinutes() <= other.toMinutes(); }
bool Time::operator>=(const Time &other) const { return toMinutes() >= other.toMinutes(); }

std::istream &operator>>(std::istream &is, Time &t) {
    std::string timeStr;
    is >> timeStr;
    t = Time(timeStr);
    return is;
}

std::ostream &operator<<(std::ostream &os, const Time &t) {
    os << std::setfill('0') << std::setw(2) << t.hours << ":" << std::setw(2) << t.minutes;
    return os;
}
