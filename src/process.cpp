#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "data_holder.h"
#include "linux_parser.h"
#include "utils.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

// DONE: Return this process's ID
int Process::Pid() const { return pid_; }

// DONE: Return this process's CPU utilization
/*
From
https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599

First we determine the total time spent for the process:

`total_time = utime + stime`
We also have to decide whether we want to include the time from children
processes. If we do, then we add those values to total_time:

`total_time = total_time + cutime + cstime`
Next we get the total elapsed time in seconds since the process started:

`seconds = uptime - (starttime / Hertz)`
Finally we calculate the CPU usage percentage:

`cpu_usage = 100 * ((total_time / Hertz) / seconds)`
*/
float Process::CpuUtilization() const {
  DataHolder<std::string> dh_path(LinuxParser::kProcDirectory +
                                  std::to_string(Pid()) +
                                  LinuxParser::kStatFilename);

  auto dh_line_split_by_space = dh_path >> Utils::lifted_readlines >>
                                Utils::lifted_get_first_line >>
                                Utils::lifted_split_by_space;
  auto line_split_space = dh_line_split_by_space.getData();
  if (line_split_space[0] == "") {
    return 0;
  }

  // from `man proc`:
  // utime     14
  // stime     15
  // starttime 22

  auto utime = std::stol(line_split_space[13]);
  auto stime = std::stol(line_split_space[14]);
  auto starttime = std::stol(line_split_space[21]);

  auto total_time = utime + stime;

  auto seconds = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
  if (seconds == 0) {
    return 0;
  } else {
    // the result is multiplied by 100 when this is called in ncurses
    auto utilization = static_cast<float>(total_time) /
                       static_cast<float>(sysconf(_SC_CLK_TCK)) /
                       static_cast<float>(seconds);
    return utilization;
  }
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// DONE: Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(Pid()); }

// DONE: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// DONE: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  // return std::stol(Ram()) < std::stol(a.Ram());
  return CpuUtilization() < a.CpuUtilization();
}