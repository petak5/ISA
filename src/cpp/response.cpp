// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <iostream>
#include <algorithm>
#include "response.h"
#include "tools.h"

Response::Response(Command command, std::string msg)
{
    parse(command, msg);

    message = unescapeString(message);
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
        std::cerr << "ERROR: Unexpected response from server.";
        exit(1);
    }

    if (!success || command == Command::Register || command == Command::Logout || command == Command::Send)
    {
        message = msg.substr(1, msg.length() - 2) + '\n';

        if (command == Command::Logout)
            deleteToken();
    }
    else if (command == Command::Login)
    {
        auto pos = msg.rfind('"', msg.length() - 2);
        auto token = msg.substr(pos + 1, msg.length() - pos - 2);

        message = msg.substr(1, pos - 3) + '\n';

        saveToken(token);
    }
    else if (command == Command::List)
    {
        message += '\n';

        // Remove outer parentheses
        msg = msg.substr(1, msg.length() - 2);
        size_t pos;

        while ((pos = getNextClosingParenthesePosition(msg)) != (size_t)-1)
        {
            // A single message entry
            auto item = msg.substr(1, pos - 1);

            auto temp_pos = item.find_first_of(' ');
            auto number = item.substr(0, temp_pos);

            item = item.substr(temp_pos + 1);

            temp_pos = getNextQuotePosition(item);
            auto sender = item.substr(1, temp_pos - 1);

            item = item.substr(temp_pos + 2);

            auto subject = item.substr(1, item.length() - 2);

            message += number + ":\n  From: " + sender + "\n  Subject: " + subject + '\n';

            msg = msg.substr(std::min(pos + 2, msg.length()));
        }
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
        std::cerr << "ERROR: Unexpected response from server #2.";
        exit(1);
    }
}

size_t Response::getNextQuotePosition(std::string msg)
{
    // Skips first character
    for (size_t i = 1; i < msg.length(); i++)
    {
        if (msg[i] == '"' && msg[i - 1] != '\\')
            return i;
    }

    return -1;
}

size_t Response::getNextClosingParenthesePosition(std::string msg)
{
    // Skips first character
    for (size_t i = 1; i < msg.length(); i++)
    {
        if (msg[i] == ')' && msg[i - 1] != '\\')
            return i;
    }

    return -1;
}