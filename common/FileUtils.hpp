#ifndef _BLACKHOLE_FILEUTILS_HPP_
#define _BLACKHOLE_FILEUTILS_HPP_

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

/**
 * Finds all files with the specified extension in the given directory and its subdirectories.
 * @param directoryPath The path to the directory to search.
 * @param ext The file extension to search for, defaults to ".list".
 * @return A vector of file paths for all matching files found.
 */
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