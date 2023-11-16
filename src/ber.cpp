#include "ber.h"

#include <iomanip>
#include <iostream>

void print_hex(std::vector<unsigned char> data) {
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
    }
    std::cout << std::endl;
}

// TODO long form length suppoort

std::vector<unsigned char> create_integer(int input) {
    std::vector<unsigned char> output;
    output.push_back(0x02);
    output.push_back(0x01);
    output.push_back(input);
    return output;
}

std::vector<unsigned char> create_octet_string(std::string input) {
    std::vector<unsigned char> output;
    output.push_back(0x04);
    output.push_back(input.size());
    for (char i : input) {
        output.push_back(i);
    }
    return output;
}

std::vector<unsigned char> create_sequence(std::vector<std::vector<unsigned char>> input) {
    std::vector<unsigned char> output;
    output.push_back(0x30);
    int total_size = 0;
    for (std::vector<unsigned char> i : input) {
        total_size += i.size();
    }
    output.push_back(total_size);
    for (std::vector<unsigned char> i : input) {
        for (unsigned char c : i) {
            output.push_back(c);
        }
    }
    return output;
};

std::vector<unsigned char> create_set(std::vector<std::vector<unsigned char>> input) {
    std::vector<unsigned char> output;
    output.push_back(0x31);
    int total_size = 0;
    for (std::vector<unsigned char> i : input) {
        total_size += i.size();
    }
    output.push_back(total_size);
    for (std::vector<unsigned char> i : input) {
        for (unsigned char c : i) {
            output.push_back(c);
        }
    }
    return output;
};
