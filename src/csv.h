#ifndef CSV_H
#define CSV_H

#include <iostream>
#include <string>
#include <vector>

// Name, mail, login are strings
// Rows are vectors of strings
// Data is vector of rows
std::vector<std::vector<std::string> > read_csv(std::string filename);

#endif  // CSV_H