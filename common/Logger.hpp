#ifndef _BLACKHOLE_LOGGER_HPP_
#define _BLACKHOLE_LOGGER_HPP_

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

enum class LogLevel { INFO, WARNING, ERROR, DEBUG };

class Logger {
public:
  Logger();
  explicit Logger(const std::string &filename);
  ~Logger();

  void setOutput(std::ostream &os);
  void log(LogLevel level, const std::string &message);
  void info(const std::string &message) { log(LogLevel::INFO, message); }
  void warn(const std::string &message) { log(LogLevel::WARNING, message); }
  void error(const std::string &message) { log(LogLevel::ERROR, message); }
  void debug(const std::string &message) { log(LogLevel::DEBUG, message); }

private:
  std::ostream *outputStream_;
  std::ofstream fileStream_;
  std::mutex mtx_;

  std::string getColorCode(LogLevel level);
  std::string getResetCode();
  std::string levelToString(LogLevel level);
};

#endif // _BLACKHOLE_LOGGER_HPP_
