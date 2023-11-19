/*
 * main.cpp
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#include <iostream>
#include <string>

#include "csv.h"
#include "server.h"
#define LDAP_DEFAULT_PORT 389

void print_help() {
    std::cout << "Usage: ./isa-ldapserver {-p <port>} -f <soubor>" << std::endl;
    std::cout << "-p <port>: Set specific port for server listening to client requests. Default port is 389." << std::endl;
    std::cout << "-f <file>: Path to a text file in CSV format containing the database." << std::endl;
}

int main(int argc, char** argv) {
    int port = LDAP_DEFAULT_PORT;
    std::string filename = "";

    if (argc < 2) {
        print_help();
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-p") {
            port = std::stoi(argv[i + 1]);
        }
        if (std::string(argv[i]) == "-f") {
            filename = argv[i + 1];
        }
    }
    if (port < 0 || port > 65535) {
        std::cout << "Port must be in range 0-65535" << std::endl;
        return 1;
    }
    if (filename == "") {
        std::cout << "File path must be specified" << std::endl;
        return 1;
    }

    std::cout << "Port: " << port << std::endl;
    std::cout << "File path: " << filename << std::endl;

    std::vector<std::vector<std::string> > data = read_csv(filename);

    server(port, data);

    return 0;
}