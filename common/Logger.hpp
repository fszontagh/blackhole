#ifndef _BLACKHOLE_LOGGER_HPP_
#define _BLACKHOLE_LOGGER_HPP_

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum class LogLevel { INFO,
                      WARNING,
                      ERROR,
                      DEBUG };

class Logger {
public:
    Logger();
    explicit Logger(const std::string& filename);
    ~Logger();

    void setOutput(std::ostream& os);
    void log(LogLevel level, const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false);
    void info(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false) { log(LogLevel::INFO, message, forceStdout, disableColors, disableTimestamp); }
    void warn(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false) { log(LogLevel::WARNING, message, forceStdout, disableColors, disableTimestamp); }
    void error(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false) { log(LogLevel::ERROR, message, forceStdout, disableColors, disableTimestamp); }
    void debug(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false) { log(LogLevel::DEBUG, message, forceStdout, disableColors, disableTimestamp); }
    bool isFileLoggingEnabled() const { return fileStream_.is_open(); }


private:
    std::ostream* outputStream_;
    std::ofstream fileStream_;
    std::mutex mtx_;

    std::string getColorCode(LogLevel level);
    std::string getResetCode();
    std::string levelToString(LogLevel level);
};

#endif  // _BLACKHOLE_LOGGER_HPP_
