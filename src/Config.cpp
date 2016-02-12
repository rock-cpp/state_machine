#include "Config.hpp"
#include <yaml-cpp/yaml.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

namespace state_machine
{

static Config *instance = nullptr;

Config& Config::getConfig(const std::string& configFile)
{
    if(instance)
        throw std::runtime_error("Config: Error, allready initialized");
    
    if(!boost::filesystem::exists(configFile))
        throw std::runtime_error("Config: Error, config file " + configFile + " does not exist");
    
    instance = new Config();
    
    try
    {
            std::ifstream fin(configFile);
            YAML::Parser parser(fin);
            YAML::Node doc;
            
            std::cout << "Parsing config ..." << std::endl;
            parser.GetNextDocument(doc);
            for(unsigned int i = 0; i < doc.size(); i++)
            {
                for(auto it = doc[i].begin(); it != doc[i].end(); it++)
                {
                    std::string memberName;
                    it.first() >> memberName;

                    std::string value;
                    it.second() >> value;
                    
                    instance->config.insert(std::make_pair(memberName, value));
                }
            }
    
    }
    catch(YAML::ParserException& e)
    {
            std::cout << "YAML ParserException: Check config file!" << std::endl;
            std::cout << e.what() << std::endl;
    }

    return *instance;
}

Config& Config::getConfig()
{
    if(!instance)
        throw std::runtime_error("Config: Error, not initialized");

    return *instance;
}

Config::Config()
{

}

const std::string& Config::getValue(const std::string& key) const
{
    auto it = config.find(key);
    if(it == config.end())
        throw std::runtime_error("Config: Error no config value " + key + " known");
    
    return it->second;
}


// void operator >> (const YAML::Node& node, Config& conf)
// {
//     
//     
// 	node["numberOfRetries"] >> conf.numberOfRetries;
// 	node["executeFrequency"] >> conf.executeFrequency;
// 	node["defaultReadTimeout"] >> conf.defaultReadTimeout;
// 	node["generateMapEveryMeters"] >> conf.generateMapEveryMeters;
// 	node["generateMapGoalDistance"] >> conf.generateMapGoalDistance;
// node["proxyConfigType"] >> conf.proxyConfigType;
//     node["verifyGoalEnabled"] >> conf.verifyGoalEnabled;
// 	
// 	const YAML::Node& loggingEnabledTaskMap = node["loggingEnabledTaskMap"];
// 	
// 	if (loggingEnabledTaskMap.Type() == YAML::NodeType::Map)
// 	{
// 		for(YAML::Iterator it = loggingEnabledTaskMap.begin(); it != loggingEnabledTaskMap.end(); it++) 
// 		{
// 			std::string key;
// 			bool value;
// 			it.first() >> key;
// 			it.second() >> value;
// 			conf.loggingEnabledTaskMap[key] = value;
// 		}
// 	}
// }
// 
// void ConfigParser::applyConfig(std::string filePath, Config& conf)
// {
// 
// }

}