// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <iostream>
#include "main.h"
#include "params.h"

int main(int argc, char *argv[])
{
    auto params = Params(argc, argv);

    std::cout << "Address: " << params.address << std::endl;
    std::cout << "Port: " << params.port << std::endl;
    std::cout << "Command: " << params.command_str << std::endl;
    if (params.args.size()) std::cout << "Args: " << std::endl;
    for (auto a : params.args)
        std::cout << "    " << a << std::endl;
}
