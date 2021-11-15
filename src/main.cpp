// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

/* Used Resources:
 * TCP client skeleton: https://beej.us/guide/bgnet/html/#client-server-background
 * Base64 implementation: https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/
 */

#include <iostream>
#include <string>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <err.h>
#include <string.h>

#include "main.h"
#include "base64.h"
#include "tools.h"

#define BUFF_SIZE 1024

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sock;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    auto params = Params(argc, argv);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(params.address.c_str(), params.port.c_str(), &hints, &servinfo)) != 0)
    {
        std::cerr << "ERROR: getaddrinfo: " + std::string(gai_strerror(rv)) + '\n';
        return 1;
    }

    // Loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("ERROR: client: socket");
            continue;
        }

        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sock);
            perror("ERROR: client: connect");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        std::cerr << "ERROR: failed to connect\n";
        return 1;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));

    freeaddrinfo(servinfo);

    // Send message
    sendMessage(sock, createMessage(params));

    // Receive response
    auto msg = receiveMessage(sock);

    auto response = Response(params.command, msg);

    // Print response
    printResponse(response);

    close(sock);

    return 0;
}

void sendMessage(int socket, std::string message)
{
    if (send(socket, message.c_str(), message.length(), 0) == -1) {
        perror("ERROR: failed to send message");
        exit(1);
    }
}

std::string receiveMessage(int socket)
{
    int numbytes;
    char buffer[BUFF_SIZE];

    if ((numbytes = recv(socket, buffer, BUFF_SIZE - 1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buffer[numbytes] = '\0';

    return std::string(buffer);
}

std::string createMessage(Params params)
{
    std::string result = "";

    std::string token = loadToken();

    switch (params.command)
    {
        case Command::Register:
            if (params.args.size() != 2)
            {
                std::cerr << "register <username> <password>\n";
                exit(1);
            }

            result = result + "(register \"" + params.args[0]  + "\" \"" + base64_encode(params.args[1]) + "\")";
            break;

        case Command::Login:
            if (params.args.size() != 2)
            {
                std::cerr << "login <username> <password>\n";
                exit(1);
            }

            result = result + "(login \"" + params.args[0]  + "\" \"" + base64_encode(params.args[1]) + "\")";
            break;

        case Command::Logout:
            if (params.args.size() != 0)
            {
                std::cerr << "logout\n";
                exit(1);
            }

            if (token.empty())
            {
                std::cerr << "Not logged in\n";
                exit(1);
            }

            result = result + "(logout \"" + token + "\")";
            break;

        case Command::List:
            if (params.args.size() != 0)
            {
                std::cerr << "list\n";
                exit(1);
            }

            if (token.empty())
            {
                std::cerr << "Not logged in\n";
                exit(1);
            }

            result = result + "(list \"" + token + "\")";
            break;

        case Command::Fetch:
            if (params.args.size() != 1)
            {
                std::cerr << "fetch <id>\n";
                exit(1);
            }

            try
            {
                if (std::to_string(std::stoi(params.args[0])) != params.args[0])
                {
                    std::cerr << "ERROR: id " + params.args[0] + " is not a number\n";
                    exit(1);
                }
            }
            catch (std::invalid_argument const&)
            {
                std::cerr << "ERROR: id " + params.args[0] + " is not a number\n";
                exit(1);
            }

            if (token.empty())
            {
                std::cerr << "Not logged in\n";
                exit(1);
            }

            result = result + "(fetch \"" + token + "\" " + params.args[0] + ")";
            break;

        case Command::Send:
            if (params.args.size() != 3)
            {
                std::cerr << "send <recipient> <subject> <body>\n";
                exit(1);
            }

            if (token.empty())
            {
                std::cerr << "Not logged in\n";
                exit(1);
            }

            result = result + "(send \"" + token + "\" \"" + params.args[0] + "\" \"" + params.args[1] + "\" \"" + params.args[2] + "\")";
            break;
    }

    return result;
}

void printResponse(Response response)
{
    std::cout << (response.success ? "SUCCESS:" : "ERROR:");

    if (!response.message.starts_with('\n'))
        std::cout << " ";

    std::cout << response.message;
}
