

#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  auto act_time = LinuxParser::ActiveJiffies();
  auto total_time = LinuxParser::Jiffies();  // TODO: See src/processor.cpp
  auto act_time_prev = acttime();
  auto total_time_prev = totaltime();
  acttime(act_time);
  totaltime(total_time);
  return float(act_time - act_time_prev) / float(total_time - total_time_prev);
}

void Processor::acttime(long acttime) { acttime_ = acttime; }
long Processor::acttime() { return acttime_; }
void Processor::totaltime(long totaltime) { totaltime_ = totaltime; }
long Processor::totaltime() { return totaltime_; }