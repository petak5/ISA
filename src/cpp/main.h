// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#include <string>
#include <vector>
#include "params.h"

int main(int argc, char *argv[]);
std::string createMessage(Params params);
void sendMessage(int socket, std::string message);