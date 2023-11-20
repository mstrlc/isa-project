
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
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iomanip>
#include <regex>
#include <string>

#include "filter.h"
#include "ldap.cpp"
#include "ldap.h"

int comm_socket;
int welcome_socket;

int status = 0;

/**
 * @brief Handle the SIGINT signal
 *
 * @param sig signal
 */
void sighandler(int sig) {
    int pid = wait3(NULL, WNOHANG, NULL);
    close(comm_socket);
    close(welcome_socket);
    exit(0);
}

int my_assert(bool condition, std::string message) {
    if (!condition) {
        std::cerr << "Error: " << message << std::endl;
        close(welcome_socket);
        return 1;
    }
    return 0;
}

/**
 * @brief Receive bytes from the client socket
 *
 * @param comm_socket Socket to receive bytes from
 * @return ber_bytes Bytes received from the client
 */
ber_bytes receive_bytes(int comm_socket) {
    int buffer_size = 4096;
    ber_bytes buffer(buffer_size, 0);

    ber_bytes received_bytes;

    int total_bytes_read = 0;
    int bytes_read;

    do {
        bytes_read = recv(comm_socket, buffer.data(), buffer_size, 0);

        if (bytes_read > 0) {
            total_bytes_read += bytes_read;
            received_bytes.insert(received_bytes.end(), buffer.begin(), buffer.begin() + bytes_read);
        } else if (bytes_read == 0) {
            return received_bytes;
            break;
        } else {
            std::cerr << "Error: Couldn't receive data from client." << std::endl;
            return ber_bytes();
            break;
        }
    } while (bytes_read == buffer_size);

    return received_bytes;
}

/**
 * @brief Send bytes to the client socket
 *
 * @param comm_socket  Socket to send bytes to
 * @param bytes     Bytes to send
 * @return int    0 if successful, 1 otherwise
 */
int send_bytes(int comm_socket, ber_bytes bytes) {
    while (send(comm_socket, bytes.data(), bytes.size(), 0) > 0) {
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

int ldap_server(int comm_socket, std::vector<std::vector<std::string>> data) {
    ber_bytes bytes;

    // Receive bindRequest
    bytes = receive_bytes(comm_socket);

    BindRequest bindrequest = BindRequest();
    try {
        bindrequest = BindRequest(bytes);

    } catch (const std::runtime_error &e) {
        int result = my_assert(false, e.what());
        if (result == 1) {
            return 1;
        }
    }

    // Validate bindRequest
    my_assert(get_protocolop(bytes) == BIND_REQUEST, "Invalid protocolOp.");
    my_assert(bindrequest.get_version() == 3, "Invalid version.");
    my_assert(bindrequest.get_authentication() == 0x80, "Invalid authentication.");

    // Send bindResponse
    BindResponse bindresponse = BindResponse(RESULT_SUCCESS);
    bindresponse.set_message_id(bindrequest.get_message_id());
    bindresponse.set_result_code(RESULT_SUCCESS);
    bindresponse.build();
    send_bytes(comm_socket, bindresponse.get_bytes());

    // LDAP loop
    while (true) {
        // Receive searchRequest
        bytes = receive_bytes(comm_socket);

        SearchRequest searchrequest = SearchRequest();
        try {
            searchrequest = SearchRequest(bytes);

        } catch (const std::runtime_error &e) {
            int result = my_assert(false, e.what());
            if (result == 1) {
                return 1;
            }
        }

        // Validate searchRequest
        my_assert(get_protocolop(bytes) == SEARCH_REQUEST, "Invalid protocolOp.");
        my_assert(searchrequest.get_scope() == SCOPE_WHOLE_SUBTREE, "Invalid scope.");
        my_assert(searchrequest.get_deref_aliases() == 0, "Invalid derefAliases.");

        struct filter filter = searchrequest.get_filter();
        int size_limit = searchrequest.get_size_limit();
        if (size_limit == 0) {
            size_limit = 1000;  // Default size limit
        }
        // Send searchResEntry
        int index = 0;
        int result_code = RESULT_SUCCESS;
        for (std::vector<std::string> item : data) {
            if (index == size_limit) {
                result_code = RESULT_SIZE_LIMIT_EXCEEDED;
                break;
            }

            std::string cn = item[0];
            std::string uid = item[1];
            std::string mail = item[2];

            bool result = match_filter(filter, uid, cn, mail);
            if (!result) {
                continue;
            }

            SearchResEntry searchresentry = SearchResEntry(uid, cn, mail);
            searchresentry.set_message_id(searchrequest.get_message_id());
            searchresentry.build();
            send_bytes(comm_socket, searchresentry.get_bytes());

            index++;
        }

        // Send searchResDone
        SearchResDone searchresdone = SearchResDone(result_code);
        searchresdone.set_message_id(searchrequest.get_message_id());
        searchresdone.build();
        send_bytes(comm_socket, searchresdone.get_bytes());

        // Receive unbindRequest
        bytes = receive_bytes(comm_socket);
        std::cout << "end of loop" << std::endl;

        if (get_protocolop(bytes) == UNBIND_REQUEST) {
            std::cout << "unb" << std::endl;

            break;
        } else if (get_protocolop(bytes) == SEARCH_REQUEST) {
            std::cout << "searchreq" << std::endl;

            continue;
        } else {
            my_assert(false, "Invalid protocolOp.");
        }
    }

    UnbindRequest unbindrequest = UnbindRequest(bytes);

    // Validate unbindRequest
    my_assert(get_protocolop(bytes) == UNBIND_REQUEST, "Invalid protocolOp.");

    return 0;
}

int server(int port, std::vector<std::vector<std::string>> data) {
    signal(SIGINT, &sighandler);

    // Setup
    int rc;
    struct sockaddr_in6 sa;
    struct sockaddr_in6 sa_client;
    char str[INET6_ADDRSTRLEN];
    int port_number = port;

    // Forked child process
    pid_t child_pid;

    // Create welcome socket
    socklen_t sa_client_len = sizeof(sa_client);
    if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }

    // Allow IPv4 and IPv6 to bind to the same port
    int disable = 0;
    if (setsockopt(welcome_socket, IPPROTO_IPV6, IPV6_V6ONLY, &disable, sizeof(disable)) < 0) {
        perror("ERROR: setsockopt");
        exit(EXIT_FAILURE);
    }

    // Bind welcome socket
    memset(&sa, 0, sizeof(sa));
    sa.sin6_family = AF_INET6;
    sa.sin6_addr = in6addr_any;
    sa.sin6_port = htons(port_number);

    if ((rc = ::bind(welcome_socket, (struct sockaddr *)&sa, sizeof(sa))) < 0) {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }

    // Listen for new connections, limit is 10
    if ((listen(welcome_socket, 10)) < 0) {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }

    // Accept new connections in loop
    while (1) {
        comm_socket = accept(welcome_socket, (struct sockaddr *)&sa_client, &sa_client_len);
        // Successful connection
        if (comm_socket > 0) {
            if (inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))) {
                // Fork child process to allow parent process to accept new connections
                if ((child_pid = fork()) == 0) {
                    close(welcome_socket);  // Child process
                    // Accept messages from client

                    // LDAP server running
                    ldap_server(comm_socket, data);
                } else {
                    close(comm_socket);  // Parent process
                }
            }
        }
    }

    // Wait for all children to finish
    while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0)
        ;

    // Close the sockets
    close(comm_socket);
    close(welcome_socket);

    return 0;
}
