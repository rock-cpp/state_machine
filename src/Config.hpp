#ifndef TASKMANAGEMET_CONFIG_HPP__
#define TASKMANAGEMET_CONFIG_HPP__

#include <string>
#include <map>

namespace state_machine
{

class Config
{
private:
    Config();
    
    std::map<std::string, std::string> config;
    
public:
    
    /**
     * Must be called only one time, to initially load the config
     * */
    static Config &getConfig(const std::string &configFile);
    
    /**
     * This function can be used after initialisation to get the 
     * config object. It must only be called after the initalization.
     * */
    static Config &getConfig();
    
    const std::string &getValue(const std::string& key) const;
    
};

}
#endif //TASKMANAGEMET_CONFIG_HPP__