#include "csv.h"

#include <fstream>
#include <sstream>

std::vector<std::vector<std::string> > read_csv(std::string filename) {
    std::vector<std::vector<std::string> > data;
    std::ifstream file("../" + filename);

    if (!file.is_open()) {
        throw std::runtime_error("File " + filename + " not found");
    }

    std::string line;
    while (std::getline(file, line, '\n')) {
        std::vector<std::string> vector;
        std::string temp;
        std::stringstream ss(line);

        getline(ss, temp, ';');
        vector.push_back(temp);
        getline(ss, temp, ';');
        vector.push_back(temp);
        getline(ss, temp, '\r');
        vector.push_back(temp);

        data.push_back(vector);
    }

    return data;
}