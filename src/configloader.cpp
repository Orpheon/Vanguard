#include <fstream>
#include <string>

#include "configloader.h"
#include "global.h"
#include "json.hpp"

nlohmann::json ConfigLoader::open(const std::string &path)
{
    std::ifstream configfile(path);
    if (configfile.good())
    {
        config << configfile;
        configfile.close();
        return config;
    }
    else
    {
        Global::logging().panic(__FILE__, __LINE__, "Config %s requested but not found.", path);
    }
}

void ConfigLoader::save(std::string &path)
{
    std::ofstream configfile(path);
    configfile << config.dump(4) << std::endl;
    configfile.close();
}
