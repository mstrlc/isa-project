
/*
 * server.cpp
 * 2023-11-20
 *
 * ISA project 2023
 * LDAP server
 *
 * Matyas Strelec xstrel03
 */

#include "server.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iomanip>
#include <string>

#include "ldap.cpp"
#include "ldap.h"

int serverSocket;

int my_assert(bool condition, std::string message) {
    if (!condition) {
        std::cerr << "Error: " << message << std::endl;
        close(serverSocket);
        return 1;
    }
    return 0;
}

ber_bytes receive_bytes(int client_socket) {
    unsigned char buffer[4096];
    bzero(buffer, sizeof(buffer));
    int bytes_read;
    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << "Received " << bytes_read << " bytes from client.\n";
        break;
    }
    std::cout << bytes_read << std::endl;
    return ber_bytes(buffer, buffer + bytes_read);
}

int send_bytes(int client_socket, ber_bytes bytes) {
    while (send(client_socket, bytes.data(), bytes.size(), 0) > 0) {
        std::cout << "Sent " << bytes.size() << " bytes to client.\n";
        break;
    }
    return 0;
}

unsigned char get_protocolop(ber_bytes bytes) {
    BERreader reader = BERreader(bytes);
    reader.read_tag();         // LDAP message tag
    reader.read_integer();     // Message ID
    return reader.read_tag();  // ProtocolOp tag
}

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
    ber_bytes bytes;

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
    bytes = receive_bytes(clientSocket);
    BindRequest bindrequest = BindRequest(bytes);

    // Validate bindRequest
    my_assert(bindrequest.get_version() == 3, "Invalid version.");
    my_assert(bindrequest.get_authentication() == 0x80, "Invalid authentication.");

    // Send bindResponse
    BindResponse bindresponse = BindResponse(RESULT_SUCCESS);
    bindresponse.set_message_id(bindrequest.get_message_id());
    bindresponse.set_result_code(RESULT_SUCCESS);
    bindresponse.build();
    send_bytes(clientSocket, bindresponse.get_bytes());

    // LDAP loop
    while (true) {
        // Receive searchRequest
        bytes = receive_bytes(clientSocket);
        SearchRequest searchrequest = SearchRequest(bytes);

        // Validate searchRequest
        my_assert(searchrequest.get_scope() == SCOPE_WHOLE_SUBTREE, "Invalid scope.");
        my_assert(searchrequest.get_deref_aliases() == 0, "Invalid derefAliases.");

        int size_limit = searchrequest.get_size_limit();
        if (size_limit == 0) {
            size_limit = 100;  // Default size limit
        }
        // Send searchResEntry
        int index = 0;
        for (std::vector<std::string> item : data) {
            if (index == size_limit) {
                break;
            }

            std::string cn = item[0];
            std::string uid = item[1];
            std::string mail = item[2];

            SearchResEntry searchresentry = SearchResEntry(uid, cn, mail);
            searchresentry.set_message_id(searchrequest.get_message_id());
            searchresentry.build();
            send_bytes(clientSocket, searchresentry.get_bytes());

            index++;
        }

        // Send searchResDone
        SearchResDone searchresdone = SearchResDone(RESULT_SUCCESS);
        searchresdone.set_message_id(searchrequest.get_message_id());
        searchresdone.build();
        send_bytes(clientSocket, searchresdone.get_bytes());

        // Receive unbindRequest
        bytes = receive_bytes(clientSocket);

        if (get_protocolop(bytes) == UNBIND_REQUEST) {
            break;
        } else if (get_protocolop(bytes) == SEARCH_REQUEST) {
            continue;
        } else {
            my_assert(false, "Invalid protocolOp.");
        }
    }

    UnbindRequest unbindrequest = UnbindRequest(bytes);

    // Validate unbindRequest
    my_assert(get_protocolop(bytes) == UNBIND_REQUEST, "Invalid protocolOp.");

    // Close the sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}
