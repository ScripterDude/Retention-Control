#include "pch.h"
#include "RetentionControl.h"

#include <iostream>
#include <string>
#include <cstdlib>

#define TOOL_NAME "Retention Control"


int start(void){
    std::cout << "Starting up "<<TOOL_NAME<< std::endl;
    //method to get appdata without active username
    std::string fullpath = std::string(std::getenv("APPDATA")) + "\\" + TOOL_NAME; //std::string(std::getenv("APPDATA")) + "\\" +"Retention Control"

    if (setupExists(fullpath)){ 
        std::cout << "Found existing setup" << std::endl;
        bool faultFree = validateDatastore(fullpath); 
        if (faultFree != true){ //if fault detected during validation, this will double check, if the fix actually fixxed it
            bool continiousFaultFree = validateDatastore(fullpath);//double check if fix worked, if not restore entire datastore
            if(continiousFaultFree){
                std::cout << "Datastore still corrupted after continous attempts to fix. Purging data and rebuilding setup." << std::endl;
                setup(fullpath, true); //purge setup and remake
            }
            
        }
    }else{
        std::cout << "No optimal previous setup found - Creating a new" << std::endl;
        int setupStatus = setup(fullpath, 1);
        std::cout << setupStatus << std::endl;
    }

    //setup validated terminal ready
    std::cout << "Retention Control is ready. Write help for commands.\n";
    while (1) {
        std::string userinput = "null";

        
        std::cin >> userinput;

        if (userinput == "help") {
            std::cout << "List: Lists all existing rules"<<std::endl
                      << "Add: Add a rule for file expiration in a designated folder"<<std::endl
                      << "Remove: Remove an existing rule"<<std::endl
            ;
        }
        else if (userinput == "list") {
            listRules();
        }
        else if (userinput == "add") {
            //addRule(fullpath);
        }
        else if (userinput == "remove") {
            removeRule("test");
        }
        else {
            std::cout << "Unknown command\n";
        }
    }
    
    }

