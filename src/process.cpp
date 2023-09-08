#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const { 
    return getpid();
}

// TODO: Return this process's CPU utilization
long Process::CpuUtilization() const{
    long activeJiffies = LinuxParser::ActiveJiffies(Pid());
    long uptime = LinuxParser::UpTime();
    long starttime = LinuxParser::StartTime(Pid());

    long total_time = activeJiffies / sysconf(_SC_CLK_TCK);
    long seconds = uptime - (starttime / sysconf(_SC_CLK_TCK));
    //long jiffies = LinuxParser::Jiffies();
    //float cpuUtilization = static_cast<float>(activeJiffies) / jiffies;
    return (total_time / seconds);
}

// TODO: Return the command that generated this process
string Process::Command() const {
    return LinuxParser::Command(Pid());
}

// TODO: Return this process's memory utilization
string Process::Ram() const {
    return LinuxParser::Ram(Pid());
}

// TODO: Return the user (name) that generated this process
string Process::User() const {
    return LinuxParser::User(Pid());
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const {
    return std::stol(LinuxParser::UpTime(Pid()));
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return CpuUtilization() < a.CpuUtilization();
}