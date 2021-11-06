// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#ifndef __MAIN_H__
#define __MAIN_H__

#include <string>
#include <vector>
#include "params.h"
#include "response.h"

int main(int argc, char *argv[]);
void sendMessage(int socket, std::string message);
std::string receiveMessage(int socket);
std::string createMessage(Params params);
void printResponse(Response response);

#endif  // __MAIN_H__
