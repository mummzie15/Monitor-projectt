#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "linux_parser.h"

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
  vector<int>pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::string line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    std::string mem_total_label;
    std::string mem_free_label;
    std::string line;
    long mem_total = 0;
    long mem_free = 0;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> mem_total_label >> mem_total) {
        if (mem_total_label == "MemTotal:") {
          break;
        }
      }
    }
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> mem_free_label >> mem_free) {
        if (mem_free_label == "MemFree:") {
          break;
        }
      }
    }
    float memory_utilization = static_cast<float>(mem_total - mem_free) / mem_total;
    return memory_utilization;
  }
  return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::string line;
  std::ifstream filestream("/proc/uptime");
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    long uptime;
    linestream >> uptime;
    return uptime;
  }
   return 0; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  std::string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string cpu_label;
    linestream >> cpu_label;
    long jiffies = 0;
    long value;
    while (linestream >> value) {
      jiffies += value;
    }
    return jiffies;
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
float LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  std::ifstream filestream("/proc/" + std::to_string(pid) + "/stat");
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string value;
    for (int i = 0; i < 13; ++i) {
      linestream >> value;
    }
    long utime = std::stol(value);
    linestream >> value;
    long stime = std::stol(value);
    linestream >> value;
    long cutime = std::stol(value);
    linestream>> value;
    long cstime = std::stol(value);
    filestream.close();
    return utime + stime + cutime + cstime;
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::string line;
  std::ifstream filestream("/proc/stat");
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string cpuLabel;
    linestream >> cpuLabel;
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    long activeJiffies = user + nice + system + irq + softirq + steal;
    return activeJiffies;
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::string line;
  std::ifstream filestream("/proc/stat");
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string cpuLabel;
    linestream >> cpuLabel;
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
    long idleJiffies = idle + iowait;
    return idleJiffies;
  }
  return 0;
}

// TODO: Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() {
  std::string line;
  std::ifstream filestream("/proc/stat");
  std::vector<std::string> CpuUtilization;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    CpuUtilization.push_back(line);
  }
  return CpuUtilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  std::string line;
  std::ifstream filestream("/proc/stat");
  while (std::getline(filestream, line)) {
    if (line.substr(0, 9) == "processes") {
      std::istringstream linestream(line);
      std::string key;
      int value;
      linestream >> key >> value;
      return value;
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line;
  std::ifstream filestream("/proc/stat");
  while (std::getline(filestream, line)) {
    if (line.substr(0, 14) == "procs_running") {
      std::istringstream linestream(line);
      std::string key;
      int value;
      linestream >> key >> value;
      return value;
    }
  } 
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Command(int pid) {
  std::string line;
  std::string path = "/proc/" + std::to_string(pid) + "cmdline";
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return "";
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Ram(int pid) {
  std::string line;
  std::string path = "/proc/" + std::to_string(pid) + "/status";
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.substr(0, 6) == "VmSize") {
        std::istringstream linestream(line);
        std::string key;
        long value;
        linestream >> key >> value;
        value /= 1024;
        return std::to_string(value);
      }
    }
  }
  return "";  
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Uid(int pid) {
  std::string line;
  std::string path = "/proc/" + std::to_string(pid) + "/status/";
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.substr(0, 4) == "Uid") {
        std::istringstream linestream(line);
        std::string key;
        std::string uid;
        linestream >> key >> uid;
        return uid;
      }
    }
  }
  return "";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::User(int pid) {
  std::string line;
  std::string path = "/proc/" + std::to_string(pid) + "/status";
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.substr(0, 5) == "Uid") {
        std::istringstream linestream(line);
        std::string key;
        std::string uid;
        linestream >> key >> uid;
        return uid;
      }
    }
  }
  return "";
}

std::string LinuxParser::Username(std::string uid) {
  std::string line;
  std::ifstream filestream("/etc/passwd");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::string username;
      std::string x;
      std::string current_uid;
      std::getline(linestream, username, ':');
      std::getline(linestream, x, ':');
      std::getline(linestream, current_uid, ':');
      if (current_uid == uid) {
        return username;
      }
    }
  }
  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::UpTime(int pid) {
  std::string line;
  std::ifstream filestream("proc/" + std::to_string(pid) + "/stat");
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::istream_iterator<std::string> beg(linestream), end;
    std::vector<std::string> statFields(beg, end);
    return statFields[21];
  }
  return "";
}
