#include "server.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iomanip>
#include <string>

#include "ber.h"
#include "ldap.h"

int serverSocket;

int setup(int port) {
    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Couldn't create socket.\n";
        return 1;
    }

    // Bind the socket to an IP address and port
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Error: Couldn't bind the socket.\n";
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Error: Couldn't listen on the socket.\n";
        close(serverSocket);
        return 1;
    }
    std::cout << "Server listening on port " << port << "...\n";
}

int server(int port, std::vector<std::vector<std::string>> data) {
    // Setup the server
    setup(port);

    // Accept incoming connections and print received data
    sockaddr_in clientAddr{};
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == -1) {
        std::cerr << "Error: Couldn't accept the client connection.\n";
        close(serverSocket);
        return 1;
    }

    // TODO allocate dynamically
    unsigned char buffer[4096];

    // Send bindResponse
    // Receive searchRequest
    // Send searchResEntry
    // Send searchResDone
    // Receive next searchRequest or unbindRequest

    ssize_t bytesRead;

    // Receive bindRequest
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << "Received " << bytesRead << " bytes from client.\n";
        break;
    }

    // TODO validate bindRequest

    int messageId = buffer[4];
    std::cout << "Received bindRequest" << std::endl
              << "Message ID " << messageId << std::endl;

    // Send bindResponse

    std::vector<unsigned char> bindResponse = build_ldapmessage(messageId, BIND_RESPONSE);

    std::cout << "Sending bindResponse" << std::endl;

    while (send(clientSocket, bindResponse.data(), bindResponse.size(), 0) > 0) {
        std::cout << "Sent " << bindResponse.size() << " bytes to client.\n";
        break;
    }

    // Receive searchRequest
    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << "Received " << bytesRead << " bytes from client.\n";
        break;
    }

    // TODO validate searchRequest

    // Close the sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
