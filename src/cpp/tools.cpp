// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
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

std::string escapeString(std::string s)
{
    std::string result = "";

    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == '"')
            result += "\\\"";
        else if (s[i] == '\\')
            result += "\\\\";
        else
            result += s[i];
    }

    return result;
}

std::string unescapeString(std::string s)
{
    std::string result = "";

    if (s.length() > 0)
        result += s[0];

    for (size_t i = 1; i < s.length(); i++)
    {
        if (s[i] == '"' && s[i - 1] == '\\')
            result = result.substr(0, result.length() - 1) + '"';
        else if (s[i] == 'n' && s[i - 1] == '\\')
            result = result.substr(0, result.length() - 1) + '\n';
        else
            result += s[i];
    }

    result = std::regex_replace(result, std::regex("\\\\\\\\"), "\\");

    return result;
}
