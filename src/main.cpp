#include "Config.hpp"
#include "Logger.hpp"
#include "PackageManager.hpp"
#include "ParamParser.hpp"

#include <iostream>

static std::string commands_help() {
    std::string help = "Commands: \n";
    help += "  help \t\t- show this help\n";
    help += "  install \t- install one or more package\n";
    help += "  search \t- search package by name\n";
    help += "  list \t\t- list installed packages\n";
    help += "  list-repo \t- list repositories\n";
    help += "  remove \t- remove one or more package\n";
    help += "  update \t- update one or more package\n";
    help += "  clean \t- clean caches\n";
    return help;
}

int main(int argc, char* argv[]) {
    std::shared_ptr<Logger> logger = std::make_shared<Logger>();
    ParamParser parser;
    parser.parse(argc, argv);

    if (parser.hasOption("--logfile")) {
        std::string filename = parser.getOptionValue("--logfile");
        if (!std::filesystem::exists(filename)) {
            std::ofstream file(filename);
            file.close();
        }
        std::shared_ptr<Logger> logger = std::make_shared<Logger>(filename);
    }

    Config config(logger);

    if (logger->isFileLoggingEnabled() == false) {
        if (config.getLogFilePath() != "") {
            std::shared_ptr<Logger> logger = std::make_shared<Logger>(config.getLogFilePath());
        }
    }

    auto positional = parser.getPositionalArgs();
    if (!positional.empty()) {
        if (std::find(positional.begin(), positional.end(), "help") !=
            positional.end()) {
            logger->info("Usage: " + std::string(argv[0]) + " <command> <package>\n" + commands_help(), true, true, true);
            return 0;
        }
        for (const auto& arg : positional) {
            if (arg == "install") {
                logger->info("Installing " + arg, true, true, true);
            } else if (arg == "search") {
                logger->info("Searching for " + arg, true, true, true);
            } else if (arg == "list") {
                logger->info("Listing installed packages", true, true, true);
            } else if (arg == "list-repo") {
                if (config.getRepositories().empty()) {
                    logger->info("No repositories found", true, true, true);
                    return 0;
                }
                logger->info("Listing repositories", true, true, true);
                for (const auto repo : config.getRepositories()) {
                    logger->info(repo.url + " - " + repo.configFile, true, true, true);
                }
            } else if (arg == "remove") {
                logger->info("Removing " + arg, true, true, true);
            } else if (arg == "update") {
                logger->info("Updating " + arg, true, true, true);
            } else if (arg == "clean") {
                logger->info("Cleaning caches", true, true, true);
            } else {
                logger->info("Unknown command: " + arg, true, true, true);
            }
        }
    }

    return 0;
}
