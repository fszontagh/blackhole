#ifndef _BLACKHOLE_CURLWRAPPER_HPP_
#define _BLACKHOLE_CURLWRAPPER_HPP_

#include <curl/curl.h>
#include <iostream>
#include <memory>
#include <string>

#include "Logger.hpp"

class CurlWrapper {
public:
  CurlWrapper(std::shared_ptr<Logger> logger) : logger(logger) {
    curl_global_init(CURL_GLOBAL_ALL);
  }
  ~CurlWrapper() { curl_global_cleanup(); }

  std::string httpGet(const std::string &url) {
    CURL *curl = curl_easy_init();
    if (!curl) {
      std::cerr << "Curl inicializáció sikertelen!" << std::endl;
      return "";
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); // SSL ellenőrzés

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "HTTPS GET hiba: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
    return response;
  }


  std::string httpPost(const std::string &url, const std::string &postData) {
    CURL *curl = curl_easy_init();
    if (!curl) {
      std::cerr << "Curl inicializáció sikertelen!" << std::endl;
      return "";
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L); // SSL ellenőrzés

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "HTTPS POST hiba: " << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);
    return response;
  }


  bool ftpDownload(const std::string &ftpUrl, const std::string &outputFile) {
    CURL *curl = curl_easy_init();
    if (!curl) {
      std::cerr << "Curl inicializáció sikertelen!" << std::endl;
      return false;
    }

    FILE *file = fopen(outputFile.c_str(), "wb");
    if (!file) {
      std::cerr << "Nem sikerült megnyitni a fájlt: " << outputFile
                << std::endl;
      return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ftpUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    CURLcode res = curl_easy_perform(curl);
    fclose(file);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
      std::cerr << "FTP letöltési hiba: " << curl_easy_strerror(res)
                << std::endl;
      return false;
    }

    return true;
  }

private:
  std::shared_ptr<Logger> logger;
  static size_t writeCallback(void *contents, size_t size, size_t nmemb,
                              std::string *userp) {
    size_t totalSize = size * nmemb;
    userp->append((char *)contents, totalSize);
    return totalSize;
  }

  static size_t writeToFile(void *ptr, size_t size, size_t nmemb,
                            FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
  }
};

#endif // _BLACKHOLE_CURLWRAPPER_HPP_