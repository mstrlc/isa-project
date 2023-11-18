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

ber_bytes create_length(size_t input) {
    ber_bytes output;

    if (input < 128) {
        // short form
        output.push_back(input);
    } else {
        // long form
        int bytes_needed = 0;
        size_t temp = input;

        while (temp > 0) {
            temp >>= 8;
            bytes_needed++;
        }

        // length of the length bytes
        output.push_back(static_cast<unsigned char>(0x80 | bytes_needed));

        // actual length bytes
        for (int i = bytes_needed - 1; i >= 0; i--) {
            output.push_back(static_cast<unsigned char>((input >> (8 * i)) & 0xFF));
        }
    }
    return output;
}

ber_bytes create_integer(int input) {
    ber_bytes output;
    output.push_back(BER_INTEGER);
    ber_bytes length = create_length(1);
    for (unsigned char c : length) {
        output.push_back(c);
    }
    output.push_back(input);
    return output;
}

ber_bytes create_octet_string(std::string input) {
    ber_bytes output;
    output.push_back(BER_OCTET_STRING);
    ber_bytes length = create_length(input.size());
    for (unsigned char c : length) {
        output.push_back(c);
    }
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
    ber_bytes length = create_length(total_size);
    for (unsigned char c : length) {
        output.push_back(c);
    }
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
    ber_bytes length = create_length(total_size);
    for (unsigned char c : length) {
        output.push_back(c);
    }
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
    ber_bytes length = create_length(total_size);
    for (unsigned char c : length) {
        output.push_back(c);
    }
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
    ber_bytes length = create_length(1);
    for (unsigned char c : length) {
        output.push_back(c);
    }
    output.push_back(input);
    return output;
}