#include "ClubLogic.h"
#include <algorithm>
#include <cmath>
#include <format>
#include <iostream>
#include <sstream>
#include <vector>

std::stringstream ClubLogic::dummyStream;

std::ostream &operator<<(std::ostream &os, const Event &event) {
    os << event.time << " " << event.id << " " << event.body;
    return os;
}

ClubLogic::ClubLogic(std::istream &in, std::ostream &out) : in(in), out(out) {
    numTables = parseInteger(readLine("Error: Input file is empty or first line is invalid."));

    std::string timeLine = readLine("Error: Missing open/close time definition.");
    std::stringstream timeStream(timeLine);
    timeStream >> openTime >> closeTime;
    if (timeStream.fail()) throw ParseException(std::format("error in timed: {}", timeLine));

    costPerHour = parseInteger(readLine("Error: Missing cost per hour definition."));
    tables.resize(numTables + 1);
}

ClubLogic::ClubLogic(int numTables, const Time &openTime, const Time &closeTime, int costPerHour, std::ostream &out)
    : numTables(numTables), openTime(openTime), closeTime(closeTime), costPerHour(costPerHour), in(dummyStream),
      out(out) {
    tables.resize(numTables + 1);
}

std::string ClubLogic::readLine(const std::string &errorOnEmpty) {
    std::string line;
    if (!std::getline(in, line) || line.empty()) throw ParseException(std::format("error in readLine: {}", errorOnEmpty));
    return line;
}

int ClubLogic::parseInteger(const std::string &line) {
    try {
        return std::stoi(line);
    } catch (const std::invalid_argument &) { throw ParseException(std::format("error in parseInteger: {}", line)); }
}

void ClubLogic::process() {
    out << openTime << std::endl;
    std::string currentLine;
    Time lastEventTime = Time(0, 0);

    while (std::getline(in, currentLine)) {
        if (currentLine.empty()) continue;

        try {
            std::stringstream ss(currentLine);
            Event event;
            std::string timeStr, idStr, clientName, tableStr;

            if (!(ss >> timeStr >> idStr >> clientName)) throw ParseException(std::format("error in process: {}", currentLine));

            event.time = Time(timeStr);
            if (event.time < lastEventTime) throw ParseException(std::format("error in events time: {}", currentLine));
            lastEventTime = event.time;

            event.id = std::stoi(idStr);
            event.clientName = clientName;
            event.body = clientName;

            if (event.id == static_cast<int>(IncomingEvent::ClientSat)) {
                if (!(ss >> tableStr)) throw ParseException(std::format("error in id of the table: {}", currentLine));
                event.tableNumber = std::stoi(tableStr);
                event.body = std::format("{} {}", clientName, tableStr);
            } else {
                std::string extra;
                if (ss >> extra) throw ParseException(std::format("Invalid format for event: {}", currentLine));
            }

            out << currentLine << std::endl;
            processEvent(event);
        } catch (const std::exception &) { out << currentLine << std::endl; }
    }

    closeClub();
    out << closeTime << std::endl;
    printStats();
}

void ClubLogic::processEvent(const Event &event) {
    switch (static_cast<IncomingEvent>(event.id)) {
    case IncomingEvent::ClientArrived: handleClientArrival(event); break;
    case IncomingEvent::ClientSat: handleClientSit(event); break;
    case IncomingEvent::ClientWaited: handleClientWait(event); break;
    case IncomingEvent::ClientLeft: handleClientLeave(event); break;
    }
}

void ClubLogic::generateOutgoingEvent(const Event &event) { out << event << std::endl; }

bool ClubLogic::checkAndReportError(bool errorCondition, const Event &event, const std::string &errorMessage) {
    if (errorCondition) {
        generateOutgoingEvent({event.time, static_cast<int>(OutgoingEvent::Error), event.clientName, 0, errorMessage});
        return true;
    }
    return false;
}

void ClubLogic::handleClientArrival(const Event &event) {
    auto it = clients.find(event.clientName);
    if (checkAndReportError(it != clients.end() && it->second.inClub, event, "YouShallNotPass")) { return; }
    if (checkAndReportError(event.time < openTime, event, "NotOpenYet")) { return; }
    clients[event.clientName].inClub = true;
}

void ClubLogic::handleClientSit(const Event &event) {
    auto it = clients.find(event.clientName);
    if (checkAndReportError(it == clients.end() || !it->second.inClub, event, "ClientUnknown")) { return; }
    if (checkAndReportError(tables[event.tableNumber].isBusy, event, "PlaceIsBusy")) { return; }

    int currentTable = it->second.tableNumber;
    if (currentTable != 0) {
        freeTable(currentTable, event.time);
        seatNextFromQueue(currentTable, event.time);
    }
    seatClient(event.time, event.clientName, event.tableNumber);
}

void ClubLogic::handleClientWait(const Event &event) {
    if (checkAndReportError(busyTablesCount < numTables, event, "ICanWaitNoLonger!")) { return; }

    if (waitingQueue.size() >= numTables) {
        generateOutgoingEvent({event.time, static_cast<int>(OutgoingEvent::ClientLeft), event.clientName, 0, event.clientName});
        clients.erase(event.clientName);
    } else {
        waitingQueue.push(event.clientName);
    }
}

void ClubLogic::handleClientLeave(const Event &event) {
    auto it = clients.find(event.clientName);
    if (checkAndReportError(it == clients.end() || !it->second.inClub, event, "ClientUnknown")) { return; }

    int tableNum = it->second.tableNumber;
    if (tableNum != 0) {
        freeTable(tableNum, event.time);
        seatNextFromQueue(tableNum, event.time);
    }
    clients.erase(it);
}

void ClubLogic::seatClient(const Time &time, const std::string &clientName, int tableNum) {
    clients[clientName].tableNumber = tableNum;
    tables[tableNum].isBusy = true;
    tables[tableNum].timeStart = time;
    busyTablesCount++;
}

void ClubLogic::seatNextFromQueue(int tableNum, const Time &time) {
    if (!waitingQueue.empty()) {
        std::string nextClient = waitingQueue.front();
        waitingQueue.pop();
        seatClient(time, nextClient, tableNum);
        std::string body = std::format("{} {}", nextClient, tableNum);
        generateOutgoingEvent({time, static_cast<int>(OutgoingEvent::ClientSat), nextClient, tableNum, body});
    }
}

void ClubLogic::freeTable(int tableNum, const Time &endTime) {
    if (tableNum == 0 || !tables[tableNum].isBusy) return;
    Time sessionTime = endTime - tables[tableNum].timeStart;
    tables[tableNum].totalTime = tables[tableNum].totalTime + sessionTime;
    tables[tableNum].revenue += (std::ceil(sessionTime.toMinutes() / 60.0)) * costPerHour;
    tables[tableNum].isBusy = false;
    busyTablesCount--;
}

void ClubLogic::closeClub() {
    std::vector<std::string> remainingClients;
    for (const auto &pair : clients) {
        if (pair.second.inClub) remainingClients.push_back(pair.first);
    }
    std::sort(remainingClients.begin(), remainingClients.end());
    for (const auto &name : remainingClients) {
        generateOutgoingEvent({closeTime, static_cast<int>(OutgoingEvent::ClientLeft), name, 0, name});
        int tableNum = clients[name].tableNumber;
        if (tableNum != 0) freeTable(tableNum, closeTime);
        clients.erase(name);
    }
}

void ClubLogic::printStats() const {
    for (int i = 1; i <= numTables; ++i)
        out << i << " " << tables[i].revenue << " " << tables[i].totalTime << std::endl;
}