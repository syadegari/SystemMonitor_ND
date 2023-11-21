#include <fstream>
#include <functional>
#include <regex>

#include "../include/data_holder.h"
#include "../include/utils.h"
#include "gtest/gtest.h"

/*
MemTotal:        2023208 kB
MemFree:           74436 kB
MemAvailable:     742440 kB
Buffers:           28456 kB
Cached:           730372 kB
SwapCached:        32024 kB
Active:           632956 kB
Inactive:        1084956 kB
*/

class UtilTest : public ::testing::Test {
 protected:
  // SetUp and TearDown similar to python unittest
  void SetUp() override {
    // Print the current working directory
#ifndef NDEBUG
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
      std::cout << "Current working dir: " << cwd << std::endl;
    } else {
      std::cout << "getcwd() error" << std::endl;
    }
#endif

    std::ofstream file("test_file");
    file << "MemTotal:        2023208 kB\n";
    file << "MemFree:           74436 kB\n";
    file << "MemAvailable:     742440 kB\n";
    file << "Buffers:           28456 kB\n";
    file << "Cached:           730372 kB\n";
    file << "SwapCached:        32024 kB\n";
    file << "Active:           632956 kB\n";
    file << "Inactive:        1084956 kB\n";
    file.close();

    std::ifstream testFile("test_file");
    ASSERT_TRUE(testFile.good()) << "test_file does not exist";
  }

  void TearDown() override { std::remove("test_file"); }
};

TEST_F(UtilTest, ReturnsEmptyForNoFile) {
  auto result = Utils::read_file("no_file");
  EXPECT_TRUE(result.empty());
}

TEST_F(UtilTest, ReadsSampleFile) {
  auto result = Utils::read_file("test_file");
  ASSERT_FALSE(result.empty());
  EXPECT_EQ(result[0], "MemTotal:        2023208 kB");
  EXPECT_EQ(result[5], "SwapCached:        32024 kB");
  // EXPECT_EQ(result[7], "Inactive:        1084956 kB");
}

TEST_F(UtilTest, RegexFound) {
  auto lines = Utils::read_file("test_file");
  ASSERT_FALSE(lines.empty());
  auto regex = std::regex("MemFree:");
  auto result = Utils::get_first_occurance(lines, regex);
  EXPECT_EQ(result, "MemFree:           74436 kB");
}

TEST_F(UtilTest, RegexNotFound) {
  auto lines = Utils::read_file("test_file");
  ASSERT_FALSE(lines.empty());
  auto regex = std::regex("cached:");
  auto result = Utils::get_first_occurance(lines, regex);
  EXPECT_EQ(result, "");
}

TEST_F(UtilTest, ChainTransformations) {
  using vstring = std::vector<std::string>;
  using DH_vstring = DataHolder<vstring>;
  using DH_string = DataHolder<std::string>;

  auto grep_line = std::bind(Utils::get_first_occurance, std::placeholders::_1,
                             std::regex("^MemFree"));
  auto grep_word = std::bind(Utils::get_match, std::placeholders::_1,
                             std::regex(".+?(\\d+)"), 1);

  DataHolder<std::string> dh_path("test_file");

  // lifting grep_line and grep_word
  std::function<DH_vstring(const DH_string&)> lifted_readlines =
      lift_wrapper<std::string>(Utils::read_file);

  std::function<DH_string(const DH_vstring&)> lifted_grep_line =
      lift_wrapper<vstring>(grep_line);
  std::function<DH_string(const DH_string&)> lifted_grep_word =
      lift_wrapper<std::string>(grep_word);

  // chaining
  auto result =
      dh_path >> lifted_readlines >> lifted_grep_line >> lifted_grep_word;
  EXPECT_EQ(result.getData(), "74436");
}
