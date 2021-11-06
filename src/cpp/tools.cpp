// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <iostream>
#include <fstream>
#include <filesystem>
#include "tools.h"

#define TOKEN_FILE "login-token"

void saveToken(std::string t)
{
    std::ofstream ofs(TOKEN_FILE);

    ofs << '"' << t << '"';

    ofs.close();
}

std::string loadToken()
{
    if (!std::filesystem::exists(TOKEN_FILE))
        return "";

    std::ifstream ifs(TOKEN_FILE);

    std::string token;
    ifs >> token;

    if (token.length() < 2)
        return "";
    else
        return token.substr(1, token.length() - 2);
}

void deleteToken()
{
    if (std::filesystem::exists(TOKEN_FILE))
        std::remove(TOKEN_FILE);
}
