#include "../src/Time.h"
#include "gtest/gtest.h"
#include <stdexcept>

TEST(TimeTest, DefaultConstructor) {
    Time t;
    ASSERT_EQ(t.hours, 0);
    ASSERT_EQ(t.minutes, 0);
}

TEST(TimeTest, StringConstructorValid) {
    Time t("12:34");
    ASSERT_EQ(t.hours, 12);
    ASSERT_EQ(t.minutes, 34);
}

TEST(TimeTest, StringConstructorInvalid) {
    ASSERT_THROW(Time("invalid"), std::invalid_argument);
    ASSERT_THROW(Time("12:"), std::invalid_argument);
    ASSERT_THROW(Time(":34"), std::invalid_argument);
    ASSERT_THROW(Time("25:00"), std::invalid_argument);
    ASSERT_THROW(Time("12:60"), std::invalid_argument);
}

TEST(TimeTest, ComparisonOperators) {
    Time t1("10:00");
    Time t2("11:00");
    Time t3("10:00");

    ASSERT_TRUE(t1 < t2);
    ASSERT_FALSE(t2 < t1);
    ASSERT_TRUE(t1 <= t2);
    ASSERT_TRUE(t1 <= t3);
    ASSERT_TRUE(t2 > t1);
    ASSERT_FALSE(t1 > t2);
    ASSERT_TRUE(t2 >= t1);
    ASSERT_TRUE(t3 >= t1);
}

TEST(TimeTest, ArithmeticOperators) {
    Time t1("10:30");
    Time t2("01:15");

    Time sum = t1 + t2;
    ASSERT_EQ(sum.hours, 11);
    ASSERT_EQ(sum.minutes, 45);

    Time diff = t1 - t2;
    ASSERT_EQ(diff.hours, 9);
    ASSERT_EQ(diff.minutes, 15);
}

TEST(TimeTest, ToMinutes) {
    Time t("02:30");
    ASSERT_EQ(t.toMinutes(), 150);
}

TEST(TimeTest, FromMinutes) {
    Time t;
    t.fromMinutes(150);
    ASSERT_EQ(t.hours, 2);
    ASSERT_EQ(t.minutes, 30);
}
