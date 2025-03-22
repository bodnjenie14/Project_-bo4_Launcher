#pragma once
#include <string>
#include <filesystem>

namespace JsonUtils {
    // Create default JSON file with initial settings
    void createDefaultJson(const std::string& jsonPath);

    // Get a value from the JSON file
    // spot: The top-level object name (e.g., "demonware", "identity")
    // name: The key within that object (e.g., "ipv4", "name")
    std::string getJsonItem(const std::string& jsonPath, const std::string& spot, const std::string& name);

    // Replace a value in the JSON file
    // value: The new value to set
    // spot: The top-level object name (e.g., "demonware", "identity")
    // key: The key within that object (e.g., "ipv4", "name")
    bool replaceJsonValue(const std::string& jsonPath, const std::string& value, 
                         const std::string& spot, const std::string& key);
}