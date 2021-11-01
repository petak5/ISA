// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

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
    unsigned short port = 32323;
    Command command;
    std::string command_str;
    std::vector<std::string> args;

    Params(int argc, char *argv[]);

private:
    void parse(int argc, char *argv[]);
    std::string command_to_string(Command command);
};