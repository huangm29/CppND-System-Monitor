#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  auto hour = seconds / 3600;
  auto min_sec = seconds % 3600;
  auto min = min_sec / 60;
  auto sec = min_sec % 60;
  auto HH = hour > 9 ? std::to_string(hour) : "0" + std::to_string(hour);
  auto MM = min > 9 ? std::to_string(min) : "0" + std::to_string(min);
  auto SS = sec > 9 ? std::to_string(sec) : "0" + std::to_string(sec);
  return HH + ":" + MM + ":" + SS;
}