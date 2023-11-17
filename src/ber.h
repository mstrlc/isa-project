#ifndef BER_H
#define BER_H

#include <string>
#include <vector>

#define ber_bytes std::vector<unsigned char>

void print_hex(ber_bytes data);

ber_bytes create_integer(int input);

ber_bytes create_octet_string(std::string input);

ber_bytes create_sequence(std::vector<ber_bytes> input);
ber_bytes create_sequence(std::vector<ber_bytes> input, unsigned char tag);

ber_bytes create_set(std::vector<ber_bytes> input);

ber_bytes create_enumerated(int input);

#endif  // BER_H