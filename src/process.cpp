#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  auto acttime = LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
  auto starttime = LinuxParser::UpTime(pid_);
  auto uptime = LinuxParser::UpTime();
  return float(acttime) / float(uptime - starttime);
}

// Return the command that generated this process
string Process::Command() {
  string full_command = LinuxParser::Command(pid_);
  if (full_command.length() > 40) return full_command.substr(0, 40) + "...";
  return full_command;
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
}

// Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  // This one show the one with highest CPU utilization on the top
  return CpuUtilization() > a.CpuUtilization();
}