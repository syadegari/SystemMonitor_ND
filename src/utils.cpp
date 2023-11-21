#include "utils.h"

#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace Utils {

std::vector<std::string> read_file(const std::string& path) {
  std::ifstream filestream(path);
  std::string line;
  std::vector<std::string> file_vector;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      file_vector.push_back(line);
    }
  }
  return file_vector;
}

std::string get_first_occurance(const std::vector<std::string>& lines,
                                const std::regex& regex) {
  std::smatch match;
  for (const auto& line : lines) {
    if (std::regex_search(line, match, regex)) {
      return line;
    }
  }
  return "";
}

std::string get_match(const std::string& string, const std::regex& regex,
                      std::size_t match_number) {
  std::smatch match;
  if (std::regex_search(string, match, regex) && match.size() > match_number) {
    return match[match_number];
  }
  return "";
}

vstring split_by_space(const std::string& str) {
  std::istringstream iss(str);
  vstring result;
  std::string token;

  while (iss >> token) {
    result.push_back(token);
  }

  return result;
}

std::string get_nth_line(const vstring& lines, std::size_t line_num) {
  try {
    return lines.at(line_num);
  } catch (...) {
    return "";
  }
}

std::function<DH_vstring(const DH_string&)> lifted_readlines =
    lift_wrapper<std::string>(read_file);

std::function<DH_string(const DH_string&)> lifted_get_num =
    lift_wrapper<std::string>(
        std::bind(get_match, ph::_1, std::regex(".+?(\\d+)"), 1));

std::function<DH_string(const DH_vstring&)> lifted_get_first_line =
    lift_wrapper<vstring>(std::bind(Utils::get_nth_line, ph::_1, 0));

std::function<DH_string(const DH_string&)> lifted_get_first_num =
    lift_wrapper<std::string>(
        std::bind(Utils::get_match, ph::_1, std::regex("(\\d+)\\s+(\\d+)"), 1));

std::function<DH_vstring(const DH_string&)> lifted_split_by_space =
    lift_wrapper<std::string>(split_by_space);

}  // namespace Utils
