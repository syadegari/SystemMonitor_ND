#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "data_holder.h"
#include "utils.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

using vstring = std::vector<std::string>;
using DH_vstring = DataHolder<vstring>;
using DH_string = DataHolder<std::string>;

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::function<DH_string(const DH_vstring&)> lifted_line_memtotal =
      lift_wrapper<vstring>(std::bind(Utils::get_first_occurance,
                                      std::placeholders::_1,
                                      std::regex("^MemTotal")));

  DataHolder<std::string> dh_path(kProcDirectory + kMeminfoFilename);

  auto result_total_memory = dh_path >> Utils::lifted_readlines >>
                             lifted_line_memtotal >> Utils::lifted_get_num;

  // Now get free memory
  // We only need to bind `get_first_occurance` to "MemFree" (and lift it) and
  // the rest of lifted functions can be reused
  std::function<DH_string(const DH_vstring&)> lifted_line_memfree =
      lift_wrapper<vstring>(std::bind(Utils::get_first_occurance,
                                      std::placeholders::_1,
                                      std::regex("^MemFree")));

  auto result_free_memory = dh_path >> Utils::lifted_readlines >>
                            lifted_line_memfree >> Utils::lifted_get_num;

  return (std::stof(result_total_memory.getData()) -
          std::stof(result_free_memory.getData())) /
         std::stof(result_total_memory.getData());
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  // `/proc/uptime` has only one line
  // The pipeline should be:
  // dh_path >> read_file >> get_first_line >> get_first_number;
  DataHolder<std::string> dh_path(kProcDirectory + kUptimeFilename);

  auto result = dh_path >> Utils::lifted_readlines >>
                Utils::lifted_get_first_line >> Utils::lifted_split_by_space;

  return std::stol(result.getData()[0]);
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  DataHolder<std::string> dh_path(kProcDirectory + std::to_string(pid) +
                                  kStatFilename);
  auto dh_line_split_space = dh_path >> Utils::lifted_readlines >>
                             Utils::lifted_get_first_line >>
                             Utils::lifted_split_by_space;
  auto line_split_space = dh_line_split_space.getData();

  // 14 -> utime
  // 15 -> stime
  return (std::stol(line_split_space[13]) + std::stol(line_split_space[14])) /
         sysconf(_SC_CLK_TCK);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto jiffies = CpuUtilization();
  return std::stol(jiffies[CPUStates::kUser_]) +
         std::stol(jiffies[CPUStates::kSystem_]) +
         std::stol(jiffies[CPUStates::kSteal_]) +
         std::stol(jiffies[CPUStates::kNice_]) +
         std::stol(jiffies[CPUStates::kIRQ_]) +
         std::stol(jiffies[CPUStates::kSoftIRQ_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto jiffies = CpuUtilization();

  return std::stol(jiffies[CPUStates::kIOwait_]) +
         std::stol(jiffies[CPUStates::kIdle_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  DataHolder<std::string> dh_path(kProcDirectory + kStatFilename);

  auto dh_first_line_separated = dh_path >> Utils::lifted_readlines >>
                                 Utils::lifted_get_first_line >>
                                 Utils::lifted_split_by_space;

  auto first_line_separated = dh_first_line_separated.getData();
  vstring result = {};
  for (std::size_t i = 1; i < first_line_separated.size(); i++) {
    result.push_back(first_line_separated[i]);
  }

  return result;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::function<DH_string(const DH_vstring&)> lifted_line_processes =
      lift_wrapper<vstring>(std::bind(Utils::get_first_occurance,
                                      std::placeholders::_1,
                                      std::regex("^processes")));

  DataHolder<std::string> dh_path(kProcDirectory + kStatFilename);

  auto result = dh_path >> Utils::lifted_readlines >> lifted_line_processes >>
                Utils::lifted_get_num;

  return std::stoi(result.getData());
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::function<DH_string(const DH_vstring&)> lifted_line_procs_running =
      lift_wrapper<vstring>(std::bind(Utils::get_first_occurance,
                                      std::placeholders::_1,
                                      std::regex("^procs_running")));

  DataHolder<std::string> dh_path(kProcDirectory + kStatFilename);

  auto result = dh_path >> Utils::lifted_readlines >>
                lifted_line_procs_running >> Utils::lifted_get_num;

  return std::stoi(result.getData());
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  DataHolder<std::string> dh_path(kProcDirectory + std::to_string(pid) +
                                  kCmdlineFilename);

  auto result =
      dh_path >> Utils::lifted_readlines >> Utils::lifted_get_first_line;

  return result.getData();
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::function<DH_string(const DH_vstring&)> lifted_line_vmsize =
      lift_wrapper<vstring>(std::bind(Utils::get_first_occurance,
                                      std::placeholders::_1,
                                      std::regex("^VmSize:")));
  DataHolder<std::string> dh_path(kProcDirectory + std::to_string(pid) +
                                  kStatusFilename);

  auto result = dh_path >> Utils::lifted_readlines >> lifted_line_vmsize >>
                Utils::lifted_get_num;

  try {
    return std::to_string(std::stol(result.getData()) / 1024);
  } catch (...) {
    return "0";
  }
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::function<DH_string(const DH_vstring&)> lifted_line_uid =
      lift_wrapper<vstring>(std::bind(Utils::get_first_occurance,
                                      std::placeholders::_1,
                                      std::regex("^Uid:")));

  DataHolder<std::string> dh_path(kProcDirectory + std::to_string(pid) +
                                  kStatusFilename);

  auto result = dh_path >> Utils::lifted_readlines >> lifted_line_uid >>
                Utils::lifted_get_num;

  return result.getData();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string uid = LinuxParser::Uid(pid);

  std::function<DH_string(const DH_vstring&)> lifted_line_uid =
      lift_wrapper<vstring>(std::bind(Utils::get_first_occurance,
                                      std::placeholders::_1,
                                      std::regex("^.+:x:" + uid)));

  std::function<DH_string(const DH_string&)> lifted_get_user =
      lift_wrapper<std::string>(std::bind(
          Utils::get_match, std::placeholders::_1, std::regex("^(.+?):"), 1));

  DataHolder<std::string> dh_path(kPasswordPath);

  auto result =
      dh_path >> Utils::lifted_readlines >> lifted_line_uid >> lifted_get_user;
  return result.getData();
}

// DONE: Read and return the uptime of a process
// The 22nd input in /proc/pid/stat file is the start time of the process
// relative to the system boot time.
long LinuxParser::UpTime(int pid) {
  DataHolder<std::string> dh_path(kProcDirectory + std::to_string(pid) +
                                  kStatFilename);

  auto result = dh_path >> Utils::lifted_readlines >>
                Utils::lifted_get_first_line >> Utils::lifted_split_by_space;

  try {
    return std::stol(result.getData()[21]) / sysconf(_SC_CLK_TCK);
  } catch (...) {
    return 0;
  }
}