#include "ParamParser.hpp"
#include <string>

void ParamParser::parse(int argc, char *argv[]) {
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg[0] == '-') {
      std::string option, value;
      size_t pos = arg.find('=');
      if (pos != std::string::npos) {
        option = arg.substr(0, pos);
        value = arg.substr(pos + 1);
      } else {
        option = arg;
        if (i + 1 < argc && argv[i + 1][0] != '-') {
          value = argv[++i];
        }
      }
      options_[option] = value;
    } else {
      positional_.push_back(arg);
    }
  }
}

bool ParamParser::hasOption(const std::string &option) const {
  return options_.find(option) != options_.end();
}

std::string ParamParser::getOptionValue(const std::string &option,
                                        const std::string &defaultValue) const {
  auto it = options_.find(option);
  if (it != options_.end())
    return it->second;
  return defaultValue;
}

std::vector<std::string> ParamParser::getPositionalArgs() const {
  return positional_;
}
