#ifndef _BLACKHOLE_LOGGER_HPP_
#define _BLACKHOLE_LOGGER_HPP_

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

/**
 * @enum LogLevel
 * @brief Enumeration of log levels.
 */
enum class LogLevel { INFO,
                      WARNING,
                      ERROR,
                      DEBUG };

/**
 * @class Logger
 * @brief Provides a simple logging utility for writing log messages to a file or stdout.
 *
 * The Logger class allows you to log messages at different log levels (INFO, WARNING, ERROR, DEBUG) to either a file or stdout. It supports optional features like colored output, timestamp, and forcing stdout.
 *
 * @param filename Optional filename to log messages to. If not provided, logs will be written to stdout.
 */
class Logger {
public:
    Logger();
    explicit Logger(const std::string& filename);
    ~Logger();

    /**
     * @brief Sets the output stream for logging.
     * @param os The output stream to use for logging.
     */
    void setOutput(std::ostream& os);

    /**
     * @brief Logs a message with the specified log level and options.
     * @param level The log level (INFO, WARNING, ERROR, DEBUG).
     * @param message The message to log.
     * @param forceStdout If true, forces output to stdout regardless of file logging settings.
     * @param disableColors If true, disables colored output.
     * @param disableTimestamp If true, disables timestamp in log messages.
     */
    void log(LogLevel level, const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false);

    /**
     * @brief Logs an info level message.
     * @param message The message to log.
     * @param forceStdout If true, forces output to stdout.
     * @param disableColors If true, disables colored output.
     * @param disableTimestamp If true, disables timestamp.
     */
    void info(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false);

    /**
     * @brief Logs a warning level message.
     * @param message The message to log.
     * @param forceStdout If true, forces output to stdout.
     * @param disableColors If true, disables colored output.
     * @param disableTimestamp If true, disables timestamp.
     */
    void warn(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false);

    /**
     * @brief Logs an error level message.
     * @param message The message to log.
     * @param forceStdout If true, forces output to stdout.
     * @param disableColors If true, disables colored output.
     * @param disableTimestamp If true, disables timestamp.
     */
    void error(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false);

    /**
     * @brief Logs a debug level message.
     * @param message The message to log.
     * @param forceStdout If true, forces output to stdout.
     * @param disableColors If true, disables colored output.
     * @param disableTimestamp If true, disables timestamp.
     */
    void debug(const std::string& message, bool forceStdout = false, bool disableColors = false, bool disableTimestamp = false);

    /**
     * @brief Checks if file logging is enabled.
     * @return true if file logging is enabled, false otherwise.
     */
    bool isFileLoggingEnabled() const;

private:
    std::ostream* outputStream_;  ///< Pointer to the current output stream
    std::ofstream fileStream_;    ///< File stream for logging to file
    std::mutex mtx_;              ///< Mutex for thread-safe logging

    /**
     * @brief Gets the ANSI color code for the specified log level.
     * @param level The log level.
     * @return String containing the ANSI color code.
     */
    std::string getColorCode(LogLevel level);

    /**
     * @brief Gets the ANSI reset code.
     * @return String containing the ANSI reset code.
     */
    std::string getResetCode();

    /**
     * @brief Converts a LogLevel to its string representation.
     * @param level The log level to convert.
     * @return String representation of the log level.
     */
    std::string levelToString(LogLevel level);
};

#endif  // _BLACKHOLE_LOGGER_HPP_
