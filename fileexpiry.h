#pragma once

#include <string>

//Datatypes
struct Rule {
    std::string path;
    int expiration;
    std::string filetype;
};

//Prototypes
int setup(std::string fullpath, bool corrupted);
bool validateDatastore(std::string fullpath);
bool setupExists(std::string fullpath);
int addRule(std::string fullpath);
int listRules(std::string fullpath);
bool validateRulePath(std::string rule_path);
void removeRule(std::string fullpath, int ruleNumber = -1);