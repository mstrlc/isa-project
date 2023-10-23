#ifndef BER_H
#define BER_H

#include <iostream>

struct BER {
    unsigned char tag;
    unsigned char length;
    unsigned char *value;
};

BER processTLV(unsigned char *data);
void processData(unsigned char *data);

#endif  // BER_H