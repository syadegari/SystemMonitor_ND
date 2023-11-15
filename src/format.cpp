#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

const long HOUR = 3600;
const long MINUTE = 60;

string Format::ElapsedTime(long seconds) {
  if (seconds > 99 * HOUR + 59 * MINUTE + 59) {
    return "99:59:59";
  }

  long hours = seconds / HOUR;
  seconds %= HOUR;
  long minutes = seconds / MINUTE;
  seconds %= MINUTE;

  std::ostringstream stream;

  stream << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2)
         << minutes << ":" << std::setw(2) << seconds;

  return stream.str();
}