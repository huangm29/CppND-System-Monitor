#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  float mem_util = 0.0;
  string key;
  string value;
  string unit;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    // The first line is the total memory
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key >> value >> unit;
    auto total_mem = std::stof(value);
    // The second line is the free memory used
    std::getline(stream, line);
    linestream.clear();
    linestream.str(line);
    linestream >> key >> value >> unit;
    auto free_mem = std::stof(value);
    // Calculate memory utilization
    mem_util = (1.0 - free_mem / total_mem);
  }
  return mem_util;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime;
  string uptime_str;
  string uptime_str_idle;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime_str >> uptime_str_idle;
  }
  uptime = stol(uptime_str);
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  auto cpu_util = CpuUtilization();
  // Avoid reading twice
  return stol(cpu_util[CPUStates::kUser_]) + stol(cpu_util[CPUStates::kNice_]) +
         stol(cpu_util[CPUStates::kSystem_]) +
         stol(cpu_util[CPUStates::kIdle_]) +
         stol(cpu_util[CPUStates::kIOwait_]) +
         stol(cpu_util[CPUStates::kIRQ_]) +
         stol(cpu_util[CPUStates::kSoftIRQ_]) +
         stol(cpu_util[CPUStates::kSteal_]);
}

// Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long act_jiffies = 0;
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) {
      linestream >> value;
    }
    // The 14-17th strings are utime, stime, cutime, cstime
    for (int i = 0; i < 4; i++) {
      linestream >> value;
      act_jiffies += stol(value);
    }
  }
  return act_jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto cpu_util = CpuUtilization();
  return stol(cpu_util[CPUStates::kUser_]) + stol(cpu_util[CPUStates::kNice_]) +
         stol(cpu_util[CPUStates::kSystem_]) +
         stol(cpu_util[CPUStates::kIRQ_]) +
         stol(cpu_util[CPUStates::kSoftIRQ_]) +
         stol(cpu_util[CPUStates::kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto cpu_util = CpuUtilization();
  return stol(cpu_util[CPUStates::kIdle_]) +
         stol(cpu_util[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string key;
  string value;
  vector<string> cpu_util;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "cpu") {
        while (linestream >> value) {
          cpu_util.emplace_back(value);
        }
        return cpu_util;
      } else
        continue;
    }
  }
  return cpu_util;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterProcesses) {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterRunningProcesses) {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}

// Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string value;
  string key;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == filterProcMem) {
        if (value.length() > 3) {
          value.erase(value.end() - 3, value.end());
          return value;
        } else
          return 0;
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string value;
  string key;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == filterUID) {
        return value;
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid;
  string line;
  string key;
  uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string name, x, id;
      std::getline(linestream, name, ':');
      std::getline(linestream, x, ':');
      std::getline(linestream, id, ':');
      if (uid == id) {
        return name;
      } else
        continue;
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long uptime = 0;
  string line;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + to_string(pid) + "/" +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++) {
      linestream >> value;
    }
    // The 22th string is uptime
    linestream >> value;
    uptime = std::stol(value) / sysconf(_SC_CLK_TCK);
  }
  return uptime;
}
