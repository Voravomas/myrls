#include "../header/printers.h"

void print_each(const struct mystat* mainInfo){
    //! prints file with description
    std::string tail;
    if (mainInfo->type[0] == '@'){
        tail += '@' + mainInfo->name + mainInfo->type.substr(1, std::string::npos);
    } else{
        tail += mainInfo->type + mainInfo->name;
    }
    std::cout << boost::format("%4i %4i %10i %4i %4i") % mainInfo->permission %
                 mainInfo->owner %
                 std::to_string(mainInfo->size) %
                 mainInfo->time_mod %
                 tail << std::endl;
}

int recursive_print(const char* dirname, std::vector<mystat>* resVec, int deep) {
    //! main function that calls recursively itself
    DIR *dp;
    struct dirent *ep;
    char abs_filename[FILENAME_MAX];
    dp = opendir(dirname);
    if (dp != nullptr){
        while((ep = readdir(dp))) {
            struct stat stFileInfo{};
            struct mystat newStat{};

            //if d_name is "." or ".."
            if (if_dot(ep->d_name)){
                continue;
            }

            //filling abs_filename var

            std::string temp_abs_fn = std::string(dirname) + "/" + std::string(ep->d_name);
            //interchange "//" with "/" if it occurs
            if (temp_abs_fn.find("//") != std::string::npos){
                temp_abs_fn.replace(temp_abs_fn.find("//"), 2, "/");
            }

            std::strcpy(abs_filename, temp_abs_fn.c_str());

            //getting primary info about file
            if (lstat(abs_filename, &stFileInfo) < 0){
                perror((std::string("Error: Unable to get info about ") + abs_filename).c_str());
                setenv("MERRNO", "1", 1);
                continue;
            }

            //filling mystat structure and saving entry
            getInfo(&stFileInfo, &newStat, &deep, abs_filename);
            resVec->push_back(newStat);


            if(S_ISDIR(stFileInfo.st_mode)){
                recursive_print(abs_filename, resVec, deep + 1);
            }
        }
        (void) closedir(dp);
    }
    else {
        std::string tempStr = std::string("Error: Couldn't open directory ") + dirname;
        perror(tempStr.c_str());
        setenv("MERRNO", "1", 1);
    }
    return get_status();
}

void print_dir(const std::vector<mystat>* resVec){
    //! function that prints all entries with directory names before
    std::string curDir;
    for (const auto& el: *resVec){
        if (el.dirname != curDir){
            curDir = el.dirname;
            std::cout << "\n" << el.dirname << ":" << std::endl;
        }
        print_each(&el);
    }
}