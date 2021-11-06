// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#ifndef __PARAMS_H__
#define __PARAMS_H__

#include <string>
#include <vector>

enum class Command
{
    Register,
    Login,
    Logout,
    List,
    Fetch,
    Send
};

class Params
{
public:
    std::string address = "127.0.0.1";
    std::string port = "32323";
    Command command;
    // Command's string representation
    std::string command_str;
    // Command arguments
    std::vector<std::string> args;

    Params(int argc, char *argv[]);

private:
    void parse(int argc, char *argv[]);
    std::string command_to_string(Command command);
};

#endif  // __PARAMS_H__
