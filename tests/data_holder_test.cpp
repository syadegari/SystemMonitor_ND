#include "../include/data_holder.h"
#include "../include/utils.h"
#include <functional>
#include <regex>
#include <string>
#include <vector>

#include "gtest/gtest.h"

// test `lift` on both simple functions of one variable and
// functions resulted from std::bind

int addOne(const int& v) { return v + 1; }
int add(const int& v1, const int& v2) { return v1 + v2; }

TEST(LiftTest, SimpleFunction) {
  auto lifted_addOne = lift<int, int>(addOne);

  auto result = lifted_addOne(DataHolder<int>(5));
  EXPECT_EQ(result.getData(), 6);
}

TEST(LiftTest, BoundFunction) {
  auto bind_add_7 = std::bind(add, std::placeholders::_1, 7);

  auto lifted_add_7 = lift<int, int>(bind_add_7);

  auto result = lifted_add_7(DataHolder<int>(5));
  EXPECT_EQ(result.getData(), 12);
}

// testing `>>` operator
// some functions that are needed to construct the test

std::vector<std::string> readfile(const std::string& path) {
  return {"line1", "line2", "line3"};
}

std::string get_first_occurance(const std::vector<std::string>& lines,
                                const std::regex& regex) {
  for (const auto& line : lines) {
    if (std::regex_match(line, regex)) {
      return line;
    }
  }
  return "";
}

std::string get_match(const std::string& line, const std::regex& regex,
                      int match_number) {
  std::smatch match;
  if (std::regex_search(line, match, regex) && match.size() > match_number) {
    return match[match_number];
  }
  return "";
}

// grep "line2"
auto grep_line =
    std::bind(get_first_occurance, std::placeholders::_1, std::regex(".+ne2"));
// grep 2
auto grep_num =
    std::bind(Utils::get_match, std::placeholders::_1, std::regex("line(\\d)"), 1);

TEST(OverloadTest, SimpleChain) {
  std::function<DataHolder<int>(const DataHolder<int>&)> addOne =
      [](const DataHolder<int>& v) { return DataHolder(v.getData() + 1); };
  std::function<DataHolder<int>(const DataHolder<int>&)> addTwo =
      [](const DataHolder<int>& v) { return DataHolder(v.getData() + 2); };

  auto val = DataHolder(1);
  auto result = val >> addOne >> addTwo;
  ASSERT_EQ(result.getData(), 4);
}

TEST(OverloadTest, ChainTransformations) {
  using vstring = std::vector<std::string>;
  using DH_vstring = DataHolder<vstring>;
  using DH_string = DataHolder<std::string>;

  // this doesn't work
  //   auto lifted_readlines = lift_wrapper<std::string>(readfile);

  std::function<DH_vstring(const DH_string&)> lifted_readlines =
      lift_wrapper<std::string>(readfile);
  std::function<DH_string(const DH_vstring&)> lifted_grep_line =
      lift_wrapper<vstring>(grep_line);
  std::function<DH_string(const DH_string&)> lifted_grep_num =
      lift_wrapper<std::string>(grep_num);

  DataHolder<std::string> dh_path("dummy");

  auto result =
      dh_path >> lifted_readlines >> lifted_grep_line >> lifted_grep_num;
  ASSERT_EQ(result.getData(), "2");
}
