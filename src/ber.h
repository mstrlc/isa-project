#ifndef BER_H
#define BER_H

#include <string>
#include <vector>

void print_hex(std::vector<unsigned char> data);

std::vector<unsigned char> create_integer(int input);

std::vector<unsigned char> create_octet_string(std::string input);

std::vector<unsigned char> create_sequence(std::vector<std::vector<unsigned char>> input);

std::vector<unsigned char> create_set(std::vector<std::vector<unsigned char>> input);

#endif  // BER_H