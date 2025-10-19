#include "../src/ClubLogic.h"
#include "gtest/gtest.h"
#include <sstream>

class ClubLogicTest : public ::testing::Test {
  protected:
    ClubLogic *club;
    Time openTime{"09:00"};
    Time closeTime{"19:00"};
    std::stringstream test_out;

    void SetUp() override { club = new ClubLogic(2, openTime, closeTime, 10, test_out); }
    void TearDown() override { delete club; }
};

TEST_F(ClubLogicTest, Arrival_Success) {
    club->processEvent({Time("10:00"), 1, "client1"});
    ASSERT_TRUE(club->getClients().at("client1").inClub);
    ASSERT_EQ(test_out.str(), "");
}

TEST_F(ClubLogicTest, Arrival_Error_NotOpenYet) {
    club->processEvent({Time("08:00"), 1, "client1"});
    ASSERT_EQ(test_out.str(), "08:00 13 NotOpenYet\n");
}

TEST_F(ClubLogicTest, Arrival_Error_YouShallNotPass) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("11:00"), 1, "client1"});
    ASSERT_EQ(test_out.str(), "11:00 13 YouShallNotPass\n");
}

TEST_F(ClubLogicTest, Sit_Success) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("10:05"), 2, "client1", 1});
    ASSERT_EQ(club->getClients().at("client1").tableNumber, 1);
    ASSERT_TRUE(club->getTables().at(1).isBusy);
    ASSERT_EQ(test_out.str(), "");
}

TEST_F(ClubLogicTest, Sit_Error_ClientUnknown) {
    club->processEvent({Time("10:05"), 2, "non-existent-client", 1});
    ASSERT_EQ(test_out.str(), "10:05 13 ClientUnknown\n");
}

TEST_F(ClubLogicTest, Sit_Error_PlaceIsBusy) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("10:05"), 2, "client1", 1});
    club->processEvent({Time("11:00"), 1, "client2"});
    club->processEvent({Time("11:05"), 2, "client2", 1});
    ASSERT_EQ(test_out.str(), "11:05 13 PlaceIsBusy\n");
}

TEST_F(ClubLogicTest, Sit_ClientChangesTable) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("10:05"), 2, "client1", 1});
    club->processEvent({Time("11:05"), 2, "client1", 2});
    ASSERT_FALSE(club->getTables().at(1).isBusy);
    ASSERT_EQ(club->getTables().at(1).revenue, 10);
    ASSERT_TRUE(club->getTables().at(2).isBusy);
    ASSERT_EQ(club->getClients().at("client1").tableNumber, 2);
    ASSERT_EQ(test_out.str(), "");
}

TEST_F(ClubLogicTest, Wait_Success) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("10:00"), 2, "client1", 1});
    club->processEvent({Time("10:00"), 1, "client2"});
    club->processEvent({Time("10:00"), 2, "client2", 2});
    club->processEvent({Time("11:00"), 1, "client3"});
    club->processEvent({Time("11:05"), 3, "client3"});
    ASSERT_EQ(club->getWaitingQueue().size(), 1);
    ASSERT_EQ(club->getWaitingQueue().front(), "client3");
    ASSERT_EQ(test_out.str(), "");
}

TEST_F(ClubLogicTest, Wait_Error_ICanWaitNoLonger) {
    club->processEvent({Time("11:05"), 3, "client3"});
    ASSERT_EQ(test_out.str(), "11:05 13 ICanWaitNoLonger!\n");
}

TEST_F(ClubLogicTest, Wait_QueueIsFull_ClientLeaves) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("10:00"), 2, "client1", 1});
    club->processEvent({Time("10:00"), 1, "client2"});
    club->processEvent({Time("10:00"), 2, "client2", 2});
    club->processEvent({Time("11:00"), 1, "c3"});
    club->processEvent({Time("11:00"), 3, "c3"});
    club->processEvent({Time("11:00"), 1, "c4"});
    club->processEvent({Time("11:00"), 3, "c4"});
    club->processEvent({Time("12:00"), 1, "c5"});
    club->processEvent({Time("12:00"), 3, "c5"});
    ASSERT_EQ(test_out.str(), "12:00 11 c5\n");
}

TEST_F(ClubLogicTest, Leave_FreesTable) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("10:05"), 2, "client1", 1});
    club->processEvent({Time("11:00"), 4, "client1"});
    ASSERT_FALSE(club->getTables().at(1).isBusy);
    ASSERT_EQ(club->getClients().count("client1"), 0);
    ASSERT_EQ(club->getTables().at(1).revenue, 10);
    ASSERT_EQ(test_out.str(), "");
}

TEST_F(ClubLogicTest, Leave_QueuedClientSits) {
    club->processEvent({Time("10:00"), 1, "client1"});
    club->processEvent({Time("10:00"), 2, "client1", 1});
    club->processEvent({Time("10:00"), 1, "client2"});
    club->processEvent({Time("10:00"), 2, "client2", 2});
    club->processEvent({Time("11:00"), 1, "client3"});
    club->processEvent({Time("11:00"), 3, "client3"});
    club->processEvent({Time("12:00"), 4, "client1"});
    ASSERT_TRUE(club->getTables().at(1).isBusy);
    ASSERT_EQ(club->getClients().at("client3").tableNumber, 1);
    ASSERT_EQ(club->getWaitingQueue().size(), 0);
    ASSERT_EQ(test_out.str(), "12:00 12 client3 1\n");
}

TEST_F(ClubLogicTest, Close_RemainingClientsLeave) {
    club->processEvent({Time("18:00"), 1, "client-b"});
    club->processEvent({Time("18:00"), 2, "client-b", 1});
    club->processEvent({Time("18:30"), 1, "client-a"});
    club->processEvent({Time("18:30"), 2, "client-a", 2});
    club->closeClub();
    ASSERT_EQ(test_out.str(), "19:00 11 client-a\n19:00 11 client-b\n");
}
