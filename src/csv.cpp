/*
 * csv.cpp
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#include "csv.h"

#include <fstream>
#include <sstream>

/**
 * @brief Read a CSV file by filename, returning a vector of vectors of strings.
 *
 * Expected format is data;data;data\n or data;data;data\r\n
 *
 * @param filename The name of the CSV file to read (relative to the project root)
 * @return std::vector<std::vector<std::string> > A vector of vectors of strings read from the CSV file
 */
std::vector<std::vector<std::string> > read_csv(std::string filename) {
    std::vector<std::vector<std::string> > data;
    std::ifstream file(filename);

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
        getline(ss, temp, '\n');
        // Remove trailing characters if source CSV is CRLF
        if (temp[temp.length() - 1] == '\r') {
            temp.erase(temp.length() - 1);
        }
        vector.push_back(temp);

        data.push_back(vector);
    }

    return data;
}