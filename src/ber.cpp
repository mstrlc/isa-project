#include "ber.h"

#include <iomanip>
#include <iostream>

#include "ldap.h"

void print_hex(ber_bytes data) {
    for (size_t i = 0; i < data.size(); i++) {
        std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";
    }
    std::cout << std::endl;
}

// TODO long form length suppoort

ber_bytes create_integer(int input) {
    ber_bytes output;
    output.push_back(BER_INTEGER);
    output.push_back(0x01);
    output.push_back(input);
    return output;
}

ber_bytes create_octet_string(std::string input) {
    ber_bytes output;
    output.push_back(BER_OCTET_STRING);
    output.push_back(input.size());
    for (char i : input) {
        output.push_back(i);
    }
    return output;
}

ber_bytes create_sequence(std::vector<ber_bytes> input) {
    ber_bytes output;
    output.push_back(BER_SEQUENCE);
    int total_size = 0;
    for (ber_bytes i : input) {
        total_size += i.size();
    }
    output.push_back(total_size);
    for (ber_bytes i : input) {
        for (unsigned char c : i) {
            output.push_back(c);
        }
    }
    return output;
};

ber_bytes create_sequence(std::vector<ber_bytes> input, unsigned char tag) {
    ber_bytes output;
    output.push_back(tag);
    int total_size = 0;
    for (ber_bytes i : input) {
        total_size += i.size();
    }
    output.push_back(total_size);
    for (ber_bytes i : input) {
        for (unsigned char c : i) {
            output.push_back(c);
        }
    }
    return output;
};

ber_bytes create_set(std::vector<ber_bytes> input) {
    ber_bytes output;
    output.push_back(BER_SET);
    int total_size = 0;
    for (ber_bytes i : input) {
        total_size += i.size();
    }
    output.push_back(total_size);
    for (ber_bytes i : input) {
        for (unsigned char c : i) {
            output.push_back(c);
        }
    }
    return output;
};

ber_bytes create_enumerated(int input) {
    ber_bytes output;
    output.push_back(BER_ENUMERATED);
    output.push_back(0x01);
    output.push_back(input);
    return output;
}
