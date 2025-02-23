#ifndef _BLACKHOLE_PARAMPARSER_HPP_
#define _BLACKHOLE_PARAMPARSER_HPP_

#include <string>
#include <vector>
#include <map>

class ParamParser {
public:
    ParamParser() = default;
    void parse(int argc, char* argv[]);
    bool hasOption(const std::string& option) const;
    std::string getOptionValue(const std::string& option, const std::string& defaultValue = "") const;
    std::vector<std::string> getPositionalArgs() const;

private:
    std::map<std::string, std::string> options_;
    std::vector<std::string> positional_;
};

#endif // _BLACKHOLE_PARAMPARSER_HPP_
