
#include "pch.h"
#include "RetentionControl.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>
#include <ctime>
#define TM_YEAR_STARTTIME 1900 // Must be added to year as it counts from 900
#define TM_MDAY_ADDITION 1 // Januar = 0 like an array



void processAllRules() {
	std::vector<Rule> rules = getRules();
    int amount = rules.size();
    double progress = amount / 10;
    for (int i = 0; i < rules.size(); i++) {
        UpdateSyncProgress(i * progress, L"Processing rules", L"Progress");
		executeRule(rules[i]);
	}
    UpdateSyncProgress(100, L"Completed Sync", L"Progress");
}

std::tm getTimeNow()
{
    std::time_t now = std::time(nullptr);
    return *std::localtime(&now);
}

int executeRule(Rule rule){

    std::string fullpath = std::string(std::getenv("APPDATA")) + "\\" + "Retention Control"; //path to appdata folder
    std::string cachePath = fullpath + "\\" + std::to_string(rule.FolderID) +".conf"; // path to cached files for a specific rule // for each folder there is a seperate cachefile in the appdata folder
    //if no cachefile, it will generate new
   
    bool folderCacheExist = fileExists(cachePath); 
    if (!folderCacheExist){
       
        std::filesystem::path p = rule.path; // converting string to path object.
        createFile(fullpath, rule.FolderID);
    }
    
    std::vector<file> folderFiles = getFiles(rule.path); 
    
    std::vector<file> cachedFiles = getCache(cachePath); // load cached files
   
    divide_result catagprozedFiles = splitNewAndExistingFiles(folderFiles, cachedFiles); //seperate files for later processing
  
    registerFiles(catagprozedFiles.new_files, cachePath); //register newly discovered files that doesnt exist inc ache.
    
    std::vector<file> expiredFiles = checkRuleCompliance(rule, catagprozedFiles.know_files); //retrieves list of files that were determined expired.
   
    removeFiles(expiredFiles, cachePath,rule.path);
    
    return 0;
}
void notifyUser(std::vector<file> files, std::string rulepath){
    int amount = files.size();
    std::string notificationString = amount + " expired files has been deleted.";
}
void removeFileFromCache(file delFile, std::string cachePath){
    //get cached files
    std::vector<file> cachedFiles = getCache(cachePath);

    //open file and remove its content
    std::ofstream openFile(
        std::filesystem::path(cachePath),
        std::ios::trunc
    );

    //write the loaded content back into the file besides the one we wish to remove
    for (int i = 0; i < cachedFiles.size(); i++)
    {
        if (cachedFiles[i].FileId != delFile.FileId)
        {
            openFile << std::quoted(cachedFiles[i].path) << " "
                     << cachedFiles[i].discovery_date << " "
                     << cachedFiles[i].FileId << '\n';
        }
    }
}

bool deleteFile(file delFile, bool hardDel){

    try {
        if (hardDel) {
            return std::filesystem::remove(delFile.path);
        } 
        else {
            //to be implemnented at later stage - move to bin instead (option)
            //std::string path_to_bin = std::string(std::getenv("bin"));
            //std::filesystem::rename(delFile.path, destination);
            return true;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return false;
    }
}


void logRemovedFiles(file deletedFile) {
    std::string fullpath =
        std::string(std::getenv("APPDATA")) + "\\Retention Control";

    std::ofstream logFile(
        std::filesystem::path(fullpath) / "removalLog.txt",
        std::ios::app
    );

    logFile << std::quoted(deletedFile.path) << " "
        << getTimeNow().tm_mday << "/"
        << getTimeNow().tm_mon + TM_MDAY_ADDITION << "/"
        << getTimeNow().tm_year + TM_YEAR_STARTTIME << "\n";
}

std::vector<file> getRemovedFiles() {
    std::string fullpath =
        std::string(std::getenv("APPDATA")) + "\\Retention Control";

    std::vector<file> loggedFiles;

    std::ifstream logFile(
        std::filesystem::path(fullpath) / "removalLog.txt"
    );

    std::string path;
    std::string date;

    while (logFile >> std::quoted(path) >> date) {
        loggedFiles.push_back({ 0, path, date });
    }

    return loggedFiles;
}

void removeFiles(std::vector<file> files, std::string cachepath, std::string rulepath){


    for(int i=0 ; i < files.size() ; i++){
        bool deleted = deleteFile(files[i], true);
        if (deleted){
            printf("Removed file: %s", files[i].path);
            removeFileFromCache(files[i], cachepath); 
            logRemovedFiles(files[i]);
        }
       
    }
    
    //UI NOTIFICATION MOVED TO PAPIRKUV LATER FEATURE
};

std::vector<file> checkRuleCompliance(Rule rule, std::vector<file> files){
    std::vector<file> expiredFiles;
    std::tm timeNow = getTimeNow();

    calenderDate currentDate = {
        timeNow.tm_mday,
        timeNow.tm_mon + TM_MDAY_ADDITION,
        timeNow.tm_year + TM_YEAR_STARTTIME
    };

    calenderDate eLB= calculateExpiration(currentDate, rule.expiration); //EXPIRATION LOWER BOUND //finding the date to declare as the lower treshold

    for (int i = 0; i < files.size() ; i++){
        int fileDay, fileMonth, fileYear;
        
        std::string fileDate = files[i].discovery_date;
        //loading from the string into date variables using placeholders.
        std::sscanf(
            fileDate.c_str(),
            "%d/%d/%d",
            &fileDay,
            &fileMonth,
            &fileYear);
        //COMPARES FILE DETECTION DATAE WITH EXPIRATION LOWER BOUND
    if (fileYear < eLB.year) {
        expiredFiles.push_back(files[i]);
    }
    else if (fileYear == eLB.year) {
        if (fileMonth < eLB.month) {
            expiredFiles.push_back(files[i]);
        }
        else if (fileMonth == eLB.month) {
            if (fileDay <= eLB.day) {
                expiredFiles.push_back(files[i]);
            }
        }
    }
        
        

    }
    //loop over files if they are in violation.
    //removeFile
    return expiredFiles;
}


calenderDate calculateExpiration(calenderDate date, int expiration) {
    tm t = {};

    t.tm_mday = date.day - expiration; //subtracts experiring time to calculate lower bound of treshold.
    t.tm_mon = date.month - 1;
    t.tm_year = date.year - 1900;

    mktime(&t); //mktime normalizes the date fields above, so that we cant have example -5th day in a month, but then calculates the right date format simple.

    return {
        t.tm_mday,
        t.tm_mon + 1,
        t.tm_year + 1900
    };
}
divide_result splitNewAndExistingFiles(std::vector<file> folderFiles, std::vector<file> cachedFiles){;
    std::vector<file> newFiles = {};
    std::vector<file> existsInCache= {};

    for(int i = 0; i < folderFiles.size() ; i++){
        bool found = false; //added boolean to fix it from adding to new files anyway, as the break obviously only breaks the one loop and not both which i initially expected as an brainfart.
        std::cout <<"cachedfile size: " <<cachedFiles.size();
        for(int k = 0; k < cachedFiles.size(); k++){
            //if cachced file exists in cache then add to existincache and break loop
            std::cout <<cachedFiles[k].FileId<< " == "<< folderFiles[i].FileId << '\n';
            if(cachedFiles[k].FileId==folderFiles[i].FileId){
                std::cout << "FILE ALLREADY CACHED: "<< folderFiles[i].path << '\n';
                found = true;
                existsInCache.push_back(cachedFiles[k]);
                break;
            }
        }
        //if not found in cachefile add to newfiles
        if(!found){
            newFiles.push_back(folderFiles[i]);
            std::cout << "FILE new: "<< folderFiles[i].path << '\n';
        }
    }
    divide_result divivdedRes = {
        newFiles,
        existsInCache
    };
    return divivdedRes;
}
std::vector<file> getCache(std::string cachePath)
{
    //Buffer values
    unsigned long long BFileId;
    std::string Bpath;
    std::string Bdiscovery_date;
    std::vector<file> files;
    
    // open file
    std::ifstream openFile{std::filesystem::path(cachePath)};
    // if file empty
    if (std::filesystem::is_empty(std::filesystem::path(cachePath))){
        std::cout << "Cache file empty: "<< cachePath << std::endl;

    }else{
         int i = 0;
            while (openFile >> std::quoted(Bpath) >> Bdiscovery_date >> BFileId){
                file currentFile;
                currentFile.path = Bpath;
                currentFile.discovery_date = Bdiscovery_date;
                currentFile.FileId = BFileId;
                files.push_back(currentFile);
            }
    }
    


    return files;
}

bool fileExists(std::string fullpath){
    if(std::filesystem::is_regular_file(std::filesystem::path(fullpath))){
        return true;
    }else{
        return false;
    }
}
void createFile(std::string path, unsigned long long name){
    std::ofstream file(std::filesystem::path(path) / (std::to_string(name)+".conf"));
    std::cout << "--New file generated" << std::endl;
    file.close();
    return;
}

void registerFiles(std::vector<file> files, std::string ruleCacheFilePath){
    if (files.empty()) { //dont proceed if file list is empty // previously it used files.empty() however it didnt work.
    return;
    }


    std::ofstream openFile(
    std::filesystem::path(ruleCacheFilePath),
    std::ios::app
    );

    for(int i = 0;i<files.size(); i++){
        openFile << std::quoted(files[i].path) << " "
         << files[i].discovery_date << " "
         << files[i].FileId << '\n';
    }
    std::cout << "Files registred to cache: "<< ruleCacheFilePath<<'\n' ;
    //openFile.close(); no longer needed as i realised it automaticly closes.
    return;
}

std::vector<file> getFiles(std::string folderPath){
    std::tm timeOfSearch = getTimeNow();
    std::vector<file> fileList;
    std::cout <<folderPath<<":"<<'\n';
    for (auto const& entry : std::filesystem::directory_iterator{folderPath}){
        //std::cout << files.path() <<'\n'<< '\n';
        file newfile;
        newfile.path = entry.path().string();
        newfile.discovery_date = std::to_string(timeOfSearch.tm_mday) + "/" + std::to_string(timeOfSearch.tm_mon + TM_MDAY_ADDITION) + "/" + std::to_string(timeOfSearch.tm_year + TM_YEAR_STARTTIME);
        newfile.FileId = getFileID(entry.path().string());
        fileList.push_back(newfile);
        std::cout <<"    "<< newfile.path<<";"<<newfile.FileId<<";"<<newfile.discovery_date << '\n';
    }

    
    return fileList;
}




//The getfileid function was made with assistance from Copilot
unsigned long long getFileID(const std::string& fullpath)
{
    HANDLE h = CreateFileA(
        fullpath.c_str(),
        0,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    if (h == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();

        std::cerr << "FAILED: [" << fullpath << "]\n";
        std::cerr << "Error: " << error << '\n';

        return 0;
    }

    BY_HANDLE_FILE_INFORMATION info{};

    if (!GetFileInformationByHandle(h, &info))
    {
        DWORD error = GetLastError();

        std::cerr << "GetFileInformationByHandle FAILED\n";
        std::cerr << "Error: " << error << '\n';

        CloseHandle(h);
        return 0;
    }

    CloseHandle(h);

    return (static_cast<unsigned long long>(info.nFileIndexHigh) << 32)
         | info.nFileIndexLow;
}

