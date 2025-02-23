#ifndef _BLACKHOLE_CONFIG_HPP_
#define _BLACKHOLE_CONFIG_HPP_

#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "FileUtils.hpp"
#include "Logger.hpp"

struct Repository {
    std::string url;
    std::string configFile;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Repository, url)
};

class Config {
public:
    Config(std::shared_ptr<Logger> logger, const std::string& configPath = std::string(CONFIG_DIRECTORY), const std::string& dotDir = std::string(CONFIG_DOT_DIRECTORY))
        : configPath(configPath), dotDir(dotDir), logger(logger) {
        this->loadConfig();
    }
    std::vector<Repository> getRepositories() { return repositories; }
    std::string getCacheDir() { return cacheDir; }
    void setCacheDir(const std::string& dir) { cacheDir = dir; }
    void addRepository(Repository repo) { repositories.push_back(repo); }
    std::string getLogFilePath() { return configPath + "/blackhole.log"; }

private:
    std::vector<Repository> repositories;
    std::string cacheDir;
    std::string configPath;
    std::string dotDir;
    std::string logFilePath;
    std::shared_ptr<Logger> logger;

    void loadConfig() {
        std::string configFile = configPath + "/config.json";

        std::ifstream file(configFile);
        if (!file.is_open()) {
            this->logger->error("Config file not found: " + configFile);
            return;
        }

        try {
            nlohmann::json data;
            file >> data;

            if (data.contains("repositories")) {
                repositories = data["repositories"].get<std::vector<Repository>>();
                for (auto& repo : repositories) {
                    repo.configFile = configFile;
                }
            }
            if (data.contains("cacheDir")) {
                cacheDir = data["cacheDir"].get<std::string>();
            }
            if (data.contains("logFilePath")) {
                logFilePath = data["logFilePath"].get<std::string>();
            }
        } catch (const nlohmann::json::parse_error& e) {
            this->logger->error("Config file is not valid JSON: " + configFile + ": byte " + std::to_string(e.byte) + "\n" + std::string(e.what()));
        } catch (const nlohmann::json::exception& e) {
            this->logger->error("Config file error in: " + configFile + "\n" + std::string(e.what()));
        }

        // load additional repos
        std::string fullDotDirPath = configPath + "/" + dotDir;
        this->logger->debug("Loading additional repositories from: " + fullDotDirPath);
        const auto list = FileUtils::findListFiles(fullDotDirPath, ".json");
        for (const auto& repoFile : list) {

            try {
                std::ifstream file(repoFile);
                if (file.is_open() == false) {
                  this->logger->warn("Config file not found: " + repoFile);
                }
                nlohmann::json data;
                file >> data;
                if (!data.contains("url")) {
                    this->logger->error("Config file is not valid JSON: " + repoFile + ": missing url");
                    continue;
                }
                repositories.push_back({data["url"].get<std::string>(), repoFile});
            } catch (const nlohmann::json::parse_error& e) {
                this->logger->warn("Failed to parse JSON from file: " + repoFile + ": " + std::string(e.what()));
            } catch (const std::exception& e) {
                this->logger->warn("Error processing file: " + repoFile + ": " + std::string(e.what()));
            }
        }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, repositories, cacheDir, logFilePath)
};

#endif