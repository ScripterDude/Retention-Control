
#include "pch.h"
#include "RetentionControl.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

std::vector<Rule> ruleList;

int addRule(std::string targetPath, int days, std::string filetypes){
    std::string fullpath = std::string(std::getenv("APPDATA")) + "\\" + "Retention Control";
    
    Rule ruleToAdd;
    std::cin >> ruleToAdd.expiration;
    std::cin >> ruleToAdd.path;
    if (filetypes.empty())
    {
        filetypes = "all";
    }

    unsigned long long ruleid = getFileID(targetPath);
    std::ofstream file(
    std::filesystem::path(fullpath) / "set.conf",
    std::ios::app
    );
    file<< targetPath<< " "<< days<< " "<< filetypes<< " "<< ruleid<< '\n';
    file.close();

    std::cout << "The requested rule has been appended.\n";

    file.close();
    return 0;
}
void removeRule(std::string targetPath)
{
    std::string fullpath =
        std::string(std::getenv("APPDATA")) + "\\Retention Control";

    std::vector<Rule> rules;
    Rule tempRule;

    std::ifstream file(
        std::filesystem::path(fullpath) / "set.conf"
    );

    if (!file.is_open()) {
        std::cout << "Could not open configuration file.\n";
        return;
    }

    while (file >> tempRule.path
        >> tempRule.expiration
        >> tempRule.filetypes
        >> tempRule.FolderID)
    {
		//keep only rules that do not match the target path
        if (tempRule.path != targetPath) {
            rules.push_back(tempRule);
        }
    }

    file.close();

    std::ofstream output(
        std::filesystem::path(fullpath) / "set.conf",
        std::ios::trunc
    );

    if (!output.is_open()) {
        std::cout << "Could not write to configuration file.\n";
        return;
    }

    for (const Rule& rule : rules) {
        output << rule.path << ' '
            << rule.expiration << ' '
            << rule.filetypes << ' '
            << rule.FolderID << '\n';
    }
}

int listRules(void) {
    std::string fullpath = std::string(std::getenv("APPDATA")) + "\\" + "Retention Control";
    std::ifstream file(std::filesystem::path(fullpath) / "set.conf");

    std::string path;
    int expiration;
    std::string filetypes;
    // if file empty
    if (std::filesystem::is_empty(
        std::filesystem::path(fullpath) / "set.conf")) {
        std::cout << "There is currently no existing rules" << std::endl;
        return 0;
    }

    int i = 0;
    while (file >> path >> expiration >> filetypes) {
        i=i+1;
        std::cout << "\033[1m[RULE " << i << "]\033[0m\n"
                << "Folder: " << "\033[3m" << path << "\033[0m\n"
                << "File type: " << "\033[3m" << filetypes << "\033[0m\n"
                << "File expiration: " << expiration
                << '\n';
        }
    return 0;
}
std::vector<Rule> getRules()
{
    std::string fullpath =
        std::string(std::getenv("APPDATA")) + "\\Retention Control";

    std::vector<Rule> rules;

    auto configPath =
        std::filesystem::path(fullpath) / "set.conf";

    //No configuration file yet = no rules.
    if (!std::filesystem::exists(configPath))
    {
        return rules;
    }

    std::ifstream file(configPath);

    if (!file.is_open())
    {
        return rules;
    }

    std::string path;
    int expiration;
    std::string filetypes;
    unsigned long long id;

    while (file >> path
        >> expiration
        >> filetypes
        >> id)
    {
        Rule currentRule;

        currentRule.path = path;
        currentRule.expiration = expiration;
        currentRule.filetypes = filetypes;
        currentRule.FolderID = id;

        rules.push_back(currentRule);
    }

    return rules;
}


