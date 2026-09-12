#pragma once
#include <vector>
#include <string>


//Datatypes
struct Rule
{
    std::string path;
    int expiration;
    std::string filetypes;
    unsigned long long FolderID;
};
struct file{
    unsigned long long FileId;
    std::string path;
    std::string discovery_date;
};    
struct divide_result {
    std::vector<file> new_files;
    std::vector<file> know_files;
    };
struct calenderDate {
    int day;
    int month;
    int year;
};

//Prototypes
int setup(std::string fullpath, bool corrupted);
bool setupExists(std::string fullpath);
bool validateDatastore(std::string fullpath);

int addRule(std::string targetPath, int days, std::string filetypes);
void removeRule(std::string targetPath);
int listRules(void);
bool validateRulePath(std::string rule_path);
std::vector<Rule> getRules();
int executeRule(Rule rule);
void processAllRules();

std::vector<file> getFiles(std::string folderPath);
std::vector<std::string> analyzeFiles(std::vector<std::string> folderFiles, std::string fileTypes);
divide_result splitNewAndExistingFiles(std::vector<file> folderFiles, std::vector<file> cachedFiles);
std::vector<file> checkRuleCompliance(Rule rule, std::vector<file> files); //check previously discovered files if they violate a rule.

void registerFiles(std::vector<file> files, std::string ruleCacheFile);
std::vector<file> getCache(std::string cachePath);
void createFile(std::string path, unsigned long long name);
bool fileExists(std::string fullpath);
unsigned long long getFileID(const std::string& fullpath);
unsigned long long getFileID(const std::string& fullpath);

bool removeFiles(std::vector<std::string> fileList);
void removeFiles(std::vector<file> files, std::string cachepath, std::string rulepath);
void logRemovedFiles(file deletedFile);
std::vector<file> getRemovedFiles();

calenderDate calculateExpiration(calenderDate currentDate, int expiration);
std::tm getTimeNow();