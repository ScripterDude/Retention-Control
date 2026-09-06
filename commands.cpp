#include "fileexpiry.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
int addRule(std::string fullpath){

    std::vector<Rule> settinglist;
    int configuring = 1;

    while(configuring == 1){
        Rule tempSetting;
        std::cout << "What path does the folder have that you wish to include File expiration in\nPath: ";
        std::cin >> tempSetting.path;

        if(std::filesystem::is_directory(tempSetting.path) == 1){
            std::cout << "--Path is valid." << std::endl;
        }else{
            std::cout << "Invalid Path" << std::endl;
            return 1;
        }
        std::cout << "How many days, before files should expire?\nDays: ";
        std::cin >> tempSetting.expiration;

        settinglist.push_back(tempSetting);
        configuring = 0;
    }
    std::ofstream file(
    std::filesystem::path(fullpath) / "set.conf",
    std::ios::app
    );
    for (Rule row : settinglist){
        file << row.path << " " << row.expiration <<" " << "all" << std::endl;
    }
    std::cout << "The requested rule has been appended.\n";

    file.close();
    return 0;
}
void removeRule(std::string fullpath, int ruleNumber) {
    std::vector<Rule> rules;
    Rule tempRule;

    std::ifstream file(
        std::filesystem::path(fullpath) / "set.conf"
    );

    if (!file.is_open()) {
        std::cout << "Could not open configuration file.\n";
        return;
    }

    while (file >> tempRule.path >> tempRule.expiration >> tempRule.filetype) {
        rules.push_back(tempRule);
    }

    file.close();

    if (rules.empty()) {
        std::cout << "There are currently no existing rules.\n";
        return;
    }

    // ruleNumber wasn't supplied -> user initiated removal
    if (ruleNumber == -1) {
        listRules(fullpath);

        std::cout << "\nWhich rule would you like to remove?\nRule: ";
        std::cin >> ruleNumber;
    }

    if (ruleNumber < 1 || ruleNumber > static_cast<int>(rules.size())) {
        std::cout << "Invalid rule number.\n";
        return;
    }

    rules.erase(rules.begin() + (ruleNumber - 1));

    std::ofstream output(
        std::filesystem::path(fullpath) / "set.conf",
        std::ios::trunc
    );

    if (!output.is_open()) {
        std::cout << "Could not write to configuration file.\n";
        return;
    }

    for (const Rule& rule : rules) {
        output << rule.path << " "
               << rule.expiration << " "
               << rule.filetype << '\n';
    }

    output.close();

    std::cout << "Rule " << ruleNumber << " removed."
              << " (Keep in mind rule numbers have updated)\n";
}

int listRules(std::string fullpath) {
    std::ifstream file(std::filesystem::path(fullpath) / "set.conf");

    std::string path;
    int expiration;
    std::string filetype;
    // if file empty
    if (std::filesystem::is_empty(
        std::filesystem::path(fullpath) / "set.conf")) {
        std::cout << "There is currently no existing rules" << std::endl;
        return 0;
    }

    int i = 0;
    while (file >> path >> expiration >> filetype) {
        i=i+1;
        std::cout << "\033[1m[RULE " << i << "]\033[0m\n"
                << "Folder: " << "\033[3m" << path << "\033[0m\n"
                << "File type: " << "\033[3m" << filetype << "\033[0m\n"
                << "File expiration: " << expiration
                << '\n';
        }
    return 0;
}