// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <iostream>
#include <fstream>
#include "tools.h"

void saveToken(std::string t)
{
    std::ofstream ofs("login-token");

    ofs << '"' << t << '"';

    ofs.close();
}

std::string loadToken()
{
    std::ifstream ifs("login-token");

    std::string token;
    ifs >> token;

    return token.substr(1, token.length() - 2);
}