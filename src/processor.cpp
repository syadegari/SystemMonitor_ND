#include "processor.h"

#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
// taken from:
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() {
  auto idle = LinuxParser::IdleJiffies();
  auto non_idle = LinuxParser::ActiveJiffies();
  auto total = LinuxParser::Jiffies();

  auto total_d = total - prev_total;
  auto idle_d = idle - prev_idle;

  auto utilization =
      static_cast<float>(total_d - idle_d) / static_cast<float>(total_d);

  prev_idle = idle;
  prev_non_idle = non_idle;
  prev_total = prev_idle + prev_non_idle;

  return utilization;
}