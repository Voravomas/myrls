#include "../header/primary.h"

void getInfo(const struct stat* primInfo, struct mystat* mainInfo, const int* deep, const char* name){
    //! function that fills "mystat" structure from stat structure
    //copying name because basename and dirname may change value
    char* name_c = strdup(name);
    std::string base_name = std::string(basename(name_c));
    std::string dir_name = std::string(dirname(name_c));
    // if path is "." replace it with appropriate dir name
    if (std::string(dir_name) == "."){
        dir_name = cur_dir_extractor();
    }

    mainInfo->deep = *deep;
    mainInfo->name = base_name;
    mainInfo->dirname = dir_name;
    mainInfo->size = primInfo->st_size;
    time_mod(&primInfo->st_mtim, &mainInfo->time_mod);
    mainInfo->owner = getpwuid(primInfo->st_uid)->pw_name;
    permissions(&primInfo->st_mode, &mainInfo->permission);
    type_of_file(&primInfo->st_mode, &mainInfo->type, name);
}