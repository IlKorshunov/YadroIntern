#include "ClubLogic.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    try {
        std::ifstream inputFile(argv[1]);
        if (!inputFile.is_open()) { throw std::runtime_error("Could not open file " + std::string(argv[1])); }
        ClubLogic club(inputFile, std::cout);
        club.process();
    } catch (const ParseException &e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
