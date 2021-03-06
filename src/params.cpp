// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <iostream>
#include "params.h"
#include "tools.h"

enum class State
{
    Start,
    Address,
    Port
};

Params::Params(int argc, char *argv[])
{
    parse(argc, argv);

    command_str = command_to_string(command);

    for (size_t i = 0; i < this->args.size(); i++)
    {
        this->args[i] = escapeString(this->args[i]);
    }
}

void Params::parse(int argc, char *argv[])
{
    State state = State::Start;
    bool addressSet = false;
    bool portSet = false;
    bool commandSet = false;

    if (argc == 1)
    {
        std::cout << "client: expects <command> [<args>] ... on the command line, given 0 arguments\n";
        exit(1);
    }

	// Start from 1, 0-th parameter is program name
	for (int i = 1; i < argc; i++)
	{
		if (state == State::Start)
		{
    		if ( (argv[i] == std::string("-a")) || (argv[i] == std::string("--address")) )
    		{
                if (addressSet)
                {
                    std::cout << "Address already set\n";
                    exit(1);
                }

                state = State::Address;
	    	}
			else if ( (argv[i] == std::string("-p")) || (argv[i] == std::string("--port")) )
			{
                if (portSet)
                {
                    std::cout << "Port already set\n";
                    exit(1);
                }

				state = State::Port;
			}
            else
            {
                if (!commandSet)
                {
                    if (argv[i] == std::string("-h"))
                        help();
                    else if (argv[i] == std::string("register"))
                        command = Command::Register;
                    else if (argv[i] == std::string("login"))
                        command = Command::Login;
                    else if (argv[i] == std::string("logout"))
                        command = Command::Logout;
                    else if (argv[i] == std::string("list"))
                        command = Command::List;
                    else if (argv[i] == std::string("fetch"))
                        command = Command::Fetch;
                    else if (argv[i] == std::string("send"))
                        command = Command::Send;
                    else
                    {
                        std::cout << "unknown command\n";
                        exit(1);
                    }

                    commandSet = true;
                }
                else
                {
                    args.push_back(argv[i]);
                }
            }
		}
		else if (state == State::Address)
		{
            address = argv[i];

            state = State::Start;
            addressSet = true;
		}
		else if (state == State::Port)
		{
            port = argv[i];

            state = State::Start;
            portSet = true;
		}
	}

    if (state == State::Address)
    {
        std::cout << "No address provided\n";
        exit(1);
    }
    else if (state == State::Port)
    {
        std::cout << "No port provided\n";
        exit(1);
    }
}

/**
 * Converts command to it's string representation
 */
std::string Params::command_to_string(Command command)
{
    std::string result = "";

    switch(command)
    {
        case Command::Register:
            result = "Register";
            break;
        case Command::Login:
            result = "Login";
            break;
        case Command::Logout:
            result = "Logout";
            break;
        case Command::List:
            result = "List";
            break;
        case Command::Fetch:
            result = "Fetch";
            break;
        case Command::Send:
            result = "Send";
            break;
    }

    return result;
}

void Params::help()
{
    std::cout << std::string("") +
        "usage: client [ <option> ... ] <command> [<args>] ...\n\n" +
        "<option> is one of\n\n" +
        "  -a <addr>, --address <addr>\n" +
        "     Server hostname or address to connect to\n" +
        "  -p <port>, --port <port>\n" +
        "     Server port to connect to\n" +
        "  --help, -h\n" +
        "     Show this help\n" +
        "  --\n" +
        "     Do not treat any remaining argument as a switch (at this level)\n\n" +
        " Multiple single-letter switches can be combined after\n" +
        " one `-`. For example, `-h-` is the same as `-h --`.\n" +
        " Supported commands:\n" +
        "   register <username> <password>\n" +
        "   login <username> <password>\n" +
        "   list\n" +
        "   send <recipient> <subject> <body>\n" +
        "   fetch <id>\n" +
        "   logout\n";

    exit(0);
}
