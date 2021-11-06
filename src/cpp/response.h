// ISA project - client implementing a protocol
// Peter Urgoš (xurgos00)
// 11/2021

#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <string>

class Response
{
public:
    /** true means success, false means error */
    bool success;

    Response(std::string message);

private:
    void parse(std::string message);
};

#endif  // __RESPONSE_H__
