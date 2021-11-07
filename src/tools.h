// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <string>

void saveToken(std::string t);
std::string loadToken();
void deleteToken();

std::string escapeString(std::string s);
std::string unescapeString(std::string s);

#endif  // __TOOLS_H__
