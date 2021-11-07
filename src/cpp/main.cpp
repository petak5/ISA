// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

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

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    auto params = Params(argc, argv);

    /*
    std::cout << "Address: " << params.address << '\n';
    std::cout << "Port: " << params.port << '\n';
    std::cout << "Command: " << params.command_str << '\n';
    if (params.args.size()) std::cout << "Args: \n";
    for (auto a : params.args)
        std::cout << "    " << a << '\n';
    */

    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(params.address.c_str(), params.port.c_str(), &hints, &servinfo)) != 0) {
        std::cerr << "getaddrinfo: " + std::string(gai_strerror(rv)) + '\n';
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        std::cerr << "client: failed to connect\n";
        return 1;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    // Send message
    sendMessage(sockfd, createMessage(params));

    // Receive response
    auto msg = receiveMessage(sockfd);

    //std::cout << "client: received '" + msg + "'\n";

    auto response = Response(params.command, msg);

    // Print response
    printResponse(response);

    close(sockfd);

    return 0;
}

void sendMessage(int socket, std::string message)
{
    if (send(socket, message.c_str(), message.length(), 0) == -1) {
        perror("ERROR: failed to send message.");
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
                std::cerr << "ERROR: Invalid arguments count for command register.";
                exit(1);
            }

            result = result + "(register \"" + params.args[0]  + "\" \"" + base64_encode(params.args[1]) + "\")";
            break;

        case Command::Login:
            if (params.args.size() != 2)
            {
                std::cerr << "ERROR: Invalid arguments count for command login.";
                exit(1);
            }

            result = result + "(login \"" + params.args[0]  + "\" \"" + base64_encode(params.args[1]) + "\")";
            break;

        case Command::Logout:
            if (params.args.size() != 0)
            {
                std::cerr << "ERROR: Invalid arguments count for command logout.";
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
                std::cerr << "ERROR: Invalid arguments count for command list.";
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
                std::cerr << "ERROR: Invalid arguments count for command fetch.";
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
                std::cerr << "ERROR: Invalid arguments count for command send.";
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
