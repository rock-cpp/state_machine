#pragma once

#include <rtt/types/TypekitRepository.hpp>
#include <rtt/types/TypeInfoRepository.hpp>
#include <rtt/types/TypeInfo.hpp>
#include <rtt/typelib/TypelibMarshallerBase.hpp>
#include <rtt/Logger.hpp>


namespace state_machine
{
    
class RPCCall 
{
private:
    std::vector<std::string> argTypeNames;
    std::vector<std::vector<uint8_t> > args;
    std::string methodName;
public:
    RPCCall(){};
    RPCCall(std::string methodName) : methodName(methodName){};
    bool addArgument(std::string argName, void *data);
    bool getArgument(int i, void *data);
    std::string getMethodName() { return methodName; };
};

}
