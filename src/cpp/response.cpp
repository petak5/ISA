// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <iostream>
#include "response.h"
#include "tools.h"

Response::Response(Command command, std::string message)
{
    parse(command, message);
}

void Response::parse(Command command, std::string msg)
{
    if (msg.starts_with("(ok"))
    {
        success = true;
        msg = msg.substr(4, msg.length() - 5);
    }
    else if (msg.starts_with("(err"))
    {
        success = false;
        msg = msg.substr(5, msg.length() - 6);
    }
    else
    {
        perror("ERROR: Unexpected response from server.");
        exit(1);
    }

    if (!success || command == Command::Register || command == Command::Logout || command == Command::Send)
    {
        message = msg.substr(1, msg.length() - 2) + '\n';
    }
    else if (command == Command::Login)
    {
        auto pos = msg.rfind('"', msg.length() - 2);
        auto token = msg.substr(pos + 1, msg.length() - pos - 2);
        std::cout << "Token: " + token + '\n';

        message = msg.substr(1, pos - 3) + '\n';

        saveToken(token);
    }
    else if (command == Command::List)
    {
    }
    else if (command == Command::Fetch)
    {
        // Remove parentheses
        msg = msg.substr(1, msg.length() - 2);

        auto pos = getNextQuotePosition(msg);
        auto sender = msg.substr(1, pos - 1);
        msg = msg.substr(pos + 2);

        pos = getNextQuotePosition(msg);
        auto subject = msg.substr(1, pos - 1);
        msg = msg.substr(pos + 2);

        auto body = msg.substr(1, msg.length() - 2);

        message = "\n\nFrom: " + sender + "\nSubject: " + subject + "\n\n" + body;
    }
    else
    {
        perror("ERROR: Unexpected response from server #2.");
        exit(1);
    }
}

size_t Response::getNextQuotePosition(std::string msg)
{
    for (size_t i = 1; i < msg.length(); i++)
    {
        if (msg[i] == '"' && msg[i - 1] != '\\')
            return i;
    }

    return -1;
}