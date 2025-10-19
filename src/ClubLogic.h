#pragma once

#include "Models.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class ParseException : public std::runtime_error {
  public:
    explicit ParseException(const std::string &line) : std::runtime_error(line) {}
};

class ClubLogic {
  public:
    explicit ClubLogic(std::istream &in, std::ostream &out);
    ClubLogic(int numTables, const Time &openTime, const Time &closeTime, int costPerHour, std::ostream &out);

    void process();

    void processEvent(const Event &event);
    void closeClub();
    void printStats() const;

    const std::map<std::string, Client> &getClients() const { return clients; }
    const std::vector<Table> &getTables() const { return tables; }
    const std::queue<std::string> &getWaitingQueue() const { return waitingQueue; }

  private:
    std::string readLine(const std::string &errorOnEmpty);
    int parseInteger(const std::string &line);

    void handleClientArrival(const Event &event);
    void handleClientSit(const Event &event);
    void handleClientWait(const Event &event);
    void handleClientLeave(const Event &event);

    void generateOutgoingEvent(const Event &event);
    void seatClient(const Time &time, const std::string &clientName, int tableNum);
    void freeTable(int tableNum, const Time &endTime);
    bool checkAndReportError(bool errorCondition, const Event &event, const std::string &errorMessage);
    void seatNextFromQueue(int tableNum, const Time &time);

    int numTables;
    int busyTablesCount = 0;
    Time openTime;
    Time closeTime;
    int costPerHour;
    std::istream &in;
    std::ostream &out;
    static std::stringstream dummyStream;

    std::vector<Table> tables;
    std::map<std::string, Client> clients;
    std::queue<std::string> waitingQueue;
};
