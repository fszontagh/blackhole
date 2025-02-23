#include "Logger.hpp"
#include <chrono>
#include <ctime>

Logger::Logger()
    : outputStream_(&std::cout) {}

Logger::Logger(const std::string& filename) {
    fileStream_.open(filename, std::ios::out | std::ios::app);
    if (fileStream_.is_open())
        outputStream_ = &fileStream_;
    else
        outputStream_ = &std::cout;
}

Logger::~Logger() {
    if (fileStream_.is_open())
        fileStream_.close();
}

void Logger::setOutput(std::ostream& os) {
    std::lock_guard<std::mutex> lock(mtx_);
    outputStream_ = &os;
}

void Logger::log(LogLevel level, const std::string& message, bool forceStdout /*= false*/, bool disableColors /*= false*/, bool disableTimestamp /*= false*/) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto now            = std::chrono::system_clock::now();
    std::time_t now_c   = std::chrono::system_clock::to_time_t(now);
    std::string timeStr = std::ctime(&now_c);
    timeStr.erase(timeStr.find_last_not_of("\n") + 1);

    std::stringstream ss;
    if (!disableTimestamp) {
        ss << "[" << timeStr << "] ";
    }
    if (outputStream_ == &std::cout || forceStdout) {
#ifdef ENABLE_COLOR_OUTPUT
        if (!disableColors)
            ss << getColorCode(level);
#endif
    }
    if (forceStdout) {
        ss << message;
    } else {
        ss << levelToString(level) << ": " << message;
    }

    if (outputStream_ == &std::cout || forceStdout) {
#ifdef ENABLE_COLOR_OUTPUT
        if (!disableColors)
            ss << getResetCode();
#endif
    }
    ss << "\n";
    if (forceStdout)
        std::cout << ss.str();
    else
        (*outputStream_) << ss.str();
}

std::string Logger::getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:
            return "\033[1;32m";  // green
        case LogLevel::WARNING:
            return "\033[1;33m";  // yellow
        case LogLevel::ERROR:
            return "\033[1;31m";  // red
        case LogLevel::DEBUG:
            return "\033[1;34m";  // blue
        default:
            return "";
    }
}

std::string Logger::getResetCode() {
    return "\033[0m";
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}
