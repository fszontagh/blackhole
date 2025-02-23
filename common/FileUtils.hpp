#ifndef _BLACKHOLE_FILEUTILS_HPP_
#define _BLACKHOLE_FILEUTILS_HPP_

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class FileUtils {
public:
    static std::vector<std::string> findListFiles(const std::string& directoryPath, std::string ext = ".list") {
        std::vector<std::string> listFiles;

        for (const auto& entry : fs::recursive_directory_iterator(directoryPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ext) {
                listFiles.push_back(entry.path().string());
            }
        }

        return listFiles;
    }
};

#endif  // _BLACKHOLE_FILEUTILS_HPP_