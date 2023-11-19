#include "server.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iomanip>
#include <string>

#include "ldap.cpp"
#include "ldap.h"

int serverSocket;

int setup(int port) {
    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error: Couldn't create socket.\n";
        return 1;
    }
    int enable = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

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
    return 0;
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

    // Receive bindRequest
    BindRequest bindrequest = BindRequest();
    bindrequest.receive(clientSocket);
    bindrequest.construct();

    // TODO validate bindRequest

    // Send bindResponse
    BindResponse bindresponse = BindResponse(RESULT_SUCCESS);
    bindresponse.set_message_id(bindrequest.get_message_id());
    bindresponse.set_result_code(RESULT_SUCCESS);
    bindresponse.build();
    bindresponse.send(clientSocket);

    // Receive searchRequest
    SearchRequest searchrequest = SearchRequest();
    searchrequest.receive(clientSocket);
    searchrequest.construct();

    // TODO validate searchRequest

    // Send searchResEntry
    int index = 0;
    for (std::vector<std::string> item : data) {
        if (index > 10) {
            break;
        }

        std::string cn = item[0];
        std::string uid = item[1];
        std::string mail = item[2];

        SearchResEntry searchresentry = SearchResEntry(uid, cn, mail);
        searchresentry.set_message_id(searchrequest.get_message_id());
        searchresentry.build();
        searchresentry.send(clientSocket);

        index++;
    }

    // Send searchResDone
    SearchResDone searchresdone = SearchResDone(RESULT_SUCCESS);
    searchresdone.set_message_id(searchrequest.get_message_id());
    searchresdone.build();
    searchresdone.send(clientSocket);

    // Receive unbindRequest
    UnbindRequest unbindrequest = UnbindRequest();
    unbindrequest.receive(clientSocket);
    unbindrequest.construct();

    // TODO validate unbindRequest

    // Close the sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
