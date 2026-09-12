
#include "pch.h"
#include "RetentionControl.h"
#include <iostream>
#include <filesystem>
#include <fstream>

//checks if a previous setup exists (Settings folder and config file)
bool setupExists(std::string fullpath){
    return std::filesystem::is_directory(fullpath) &&
           std::filesystem::is_regular_file(
               std::filesystem::path(fullpath) / "set.conf"
           );
}

//checks if the path of an given rule exists.
bool validateRulePath(std::string rule_path){
    if(std::filesystem::is_directory(rule_path)){
       return true; 
    }
    else{
        return false;
    }
}

//checks config file for invalid data, and handles potential corrupted data.
bool validateDatastore(std::string fullpath){
    std::ifstream file(std::filesystem::path(fullpath) / "set.conf");
    
    std::string path;
    int expiration;
    std::string filetype;
    // if file empty
    if (std::filesystem::is_empty(
        std::filesystem::path(fullpath) / "set.conf")) {
        std::cout << "There is currently no existing rules" << std::endl;
        return true;
    }

    int i = 0;
    bool had_corrupion = false;
    while (file >> path >> expiration >> filetype) {
        i=i+1;
        if (path.empty() || expiration < 0 || filetype.empty()) {
            std::cout << "WARNING: Corrupted rule data detected. Removing [RULE "
                    << i << "]" << std::endl;
            removeRule("test");
        }
        if(!validateRulePath(path)){
            std::cout << "WARNING: Invalid rule path detected. Removing [RULE  " << i <<"]"<< std::endl;
            removeRule("test");
            had_corrupion = true;
        }
    }
    if(had_corrupion){
        return false;
    }else{
    return true;
    }  
}
    

int setup(std::string fullpath, bool corrupted){
    
    if (corrupted){
        std::filesystem::remove_all(fullpath);
        std::cout << "--Removed corrupted setup" << std::endl;
    }

    std::filesystem::create_directories(fullpath);

    std::cout << "--New settings folder generated" << std::endl;
    std::ofstream file(std::filesystem::path(fullpath) / "set.conf");
    std::cout << "--New configuration file generated" << std::endl;
    file.close();
    return 0;
}

