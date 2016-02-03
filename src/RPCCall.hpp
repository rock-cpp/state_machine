#pragma once

#include <rtt/types/TypekitRepository.hpp>
#include <rtt/types/TypeInfoRepository.hpp>
#include <rtt/types/TypeInfo.hpp>
#include <rtt/typelib/TypelibMarshallerBase.hpp>
#include <rtt/Logger.hpp>

#include <rtt/types/Types.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/transports/corba/TaskContextServer.hpp>
#include <rtt/transports/corba/CorbaDispatcher.hpp>

// void setHeight(double height) {
//     RPCCall* setHeightFunction = new RPCCall();
//     setHeightFunction->addArgument("double", (void*) &height);
//     //todo write rpc call on port
// 
// std::function<void (double)> setHeightCallback;
// 
// void setHeightRecieve(RPCCall* call) {
//     std::string arg1Name("double");
//     double height;
//     call->getArgument(0, (void*) &height);
//     setHeightCallback(height);
// }
/*

void recieve() {
    RPCCall newCall;
    // todo read from rpc port
    
    if(newCall.getMethodName() == "setHeight") {
        setHeightRecieve(newCall);
    }
}*/

class RPCCall {
    std::vector<std::string> argTypeNames;
    std::vector<std::vector<uint8_t>> args;
    std::string methodName;
public:
    RPCCall(std::string methodName) : methodName(methodName){};
    bool addArgument(std::string argName, void *data);
    bool getArgument(int i, void *data);
    std::string getMethodName() { return methodName; };
};

//         double foo = 4;
// 
//         RPCCall call;
// 
//         call.addArgument(arg1Name, (void *) &foo);
// 
// 
// 
//         //receiver
// 
//         double height;
//         call.getArgument(0, (void *) &height);
