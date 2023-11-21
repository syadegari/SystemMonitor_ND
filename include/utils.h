#ifndef UTILS_H
#define UTILS_H

#include <functional>
#include <regex>
#include <string>
#include <vector>

#include "data_holder.h"

namespace Utils {

using vstring = std::vector<std::string>;
using DH_vstring = DataHolder<vstring>;
using DH_string = DataHolder<std::string>;

namespace ph = std::placeholders;

vstring read_file(const std::string& path);
std::string get_first_occurance(const vstring& lines, const std::regex& regex);
std::string get_match(const std::string& string, const std::regex& regex,
                      std::size_t match_number);
std::string get_nth_line(const vstring& lines, std::size_t line_num);
vstring split_by_space(const std::string& str);

extern std::function<DH_vstring(const DH_string&)> lifted_readlines;
extern std::function<DH_string(const DH_string&)> lifted_get_num;
extern std::function<DH_string(const DH_vstring&)> lifted_get_first_line;
extern std::function<DH_string(const DH_string&)> lifted_get_first_num;
extern std::function<DH_vstring(const DH_string&)> lifted_split_by_space;
};  // namespace Utils
#endif
