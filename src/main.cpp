#include "Logger.hpp"
#include "PackageManager.hpp"
#include "ParamParser.hpp"

#include <iostream>

int main(int argc, char *argv[]) {
  std::shared_ptr<Logger> logger = std::make_shared<Logger>();
  ParamParser parser;
  parser.parse(argc, argv);

  if (parser.hasOption("--logfile")) {
    std::string filename = parser.getOptionValue("--logfile");
    std::shared_ptr<Logger> logger = std::make_shared<Logger>(filename);
  }

  auto positional = parser.getPositionalArgs();
  if (!positional.empty()) {
    if (std::find(positional.begin(), positional.end(), "help") !=
        positional.end()) {
      logger->info("Usage: " + std::string(argv[0]) + " <command> <package>");
      return 0;
    }
    for (const auto &arg : positional) {
      logger->info("  " + arg);
    }
  }

  return 0;
}
