#pragma once

#include <iostream>

class Time {
  public:
    int hours;
    int minutes;

    Time();
    Time(int h, int m);
    Time(const std::string &timeStr);

    int toMinutes() const;
    void fromMinutes(int totalMinutes);
    Time operator-(const Time &other) const;
    Time operator+(const Time &other) const;
    bool operator<(const Time &other) const;
    bool operator>(const Time &other) const;
    bool operator<=(const Time &other) const;
    bool operator>=(const Time &other) const;

    friend std::istream &operator>>(std::istream &is, Time &t);
    friend std::ostream &operator<<(std::ostream &os, const Time &t);

  private:
    explicit Time(int totalMinutes);
};