#include "../include/format.h"

#include "gtest/gtest.h"

TEST(ElapsedTimeTest, ZeroSec) {
  long input = 0;
  std::string expected = "00:00:00";
  EXPECT_EQ(Format::ElapsedTime(input), expected);
}

TEST(ElapsedTimeTest, LessThan10Hours) {
  long input = 7335;
  std::string expected = "02:02:15";
  EXPECT_EQ(Format::ElapsedTime(input), expected);
}

TEST(ElapsedTimeTest, MoreThan10Hours) {
  long input = 83551;
  std::string expected = "23:12:31";
  EXPECT_EQ(Format::ElapsedTime(input), expected);
}

TEST(ElapsedTimeTest, UpperLimit) {
  long input = 359999;
  std::string expected = "99:59:59";
  EXPECT_EQ(Format::ElapsedTime(input), expected);
}

TEST(ElapsedTimeTest, Overflow) {
  long input = 360001;
  std::string expected = "99:59:59";
  EXPECT_EQ(Format::ElapsedTime(input), expected);
}
