#pragma once
#include <string>
#include <filesystem>

namespace JsonUtils {
    void createDefaultJson(const std::string& jsonPath);


    std::string getJsonItem(const std::string& jsonPath, const std::string& spot, const std::string& name);


    bool replaceJsonValue(const std::string& jsonPath, const std::string& value, 
                         const std::string& spot, const std::string& key);
}