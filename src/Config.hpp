#ifndef _BLACKHOLE_CONFIG_HPP_
#define _BLACKHOLE_CONFIG_HPP_

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

struct Repository {
    std::string url;
    std::string configFile;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Repository, url, configFile)
};

struct Config {
    std::vector<Repository> repositories;
    std::string cacheDir;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, repositories, cacheDir)
};

#endif