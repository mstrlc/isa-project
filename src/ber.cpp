#include "ber.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iomanip>
#include <string>

BER processTLV(unsigned char *data) {
    BER ber;
    ber.tag = data[0];
    ber.length = data[1];
    ber.value = data + 2;

    return ber;
}

void processData(unsigned char *data) {
    int len = data[1];
    std::cout << "Length: " << len << std::endl;

    BER outermost = processTLV(data);
    // print BER
    std::cout << std::endl;
    std::cout << "Outermost BER: " << std::endl;
    std::cout << "Tag: 0x" << std::setw(2) << std::setfill('0') << std::hex << int(outermost.tag) << std::endl;
    std::cout << "Length: 0x" << std::setw(2) << std::setfill('0') << std::hex << int(outermost.length) << std::endl;
    std::cout << "Value: " << std::endl;

    for (int i = 0; i < int(outermost.length); i++) {
        std::cout << "0x" << std::setw(2) << std::setfill('0') << std::hex << int(outermost.value[i]) << " ";
    }

    std::cout << std::endl;
}
