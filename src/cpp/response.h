// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <string>
#include "params.h"

class Response
{
public:
    /** true means success, false means error */
    bool success = false;
    std::string message = "";

    Response(Command command, std::string message);

private:
    void parse(Command command, std::string msg);
    size_t getNextQuotePosition(std::string msg);
};

#endif  // __RESPONSE_H__
