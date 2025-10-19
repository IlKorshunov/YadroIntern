#pragma once

#include "Time.h"
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

enum class IncomingEvent { ClientArrived = 1, ClientSat = 2, ClientWaited = 3, ClientLeft = 4 };
enum class OutgoingEvent { ClientLeft = 11, ClientSat = 12, Error = 13 };

struct Event {
    Time time;
    int id;
    std::string clientName;
    int tableNumber = 0;
    std::string body;
    friend std::ostream &operator<<(std::ostream &os, const Event &event);
};

struct Table {
    Time timeStart;
    Time totalTime{0, 0};
    int revenue = 0;
    bool isBusy = false;
};

struct Client {
    int tableNumber = 0;
    bool inClub = false;
};
