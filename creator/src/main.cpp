#include <iostream>
#include "ParamParser.hpp"

int main(int argc, char* argv[]) {
    ParamParser parser;
    parser.parse(argc, argv);

    return 0;
}
