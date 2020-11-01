#include "../header/minor.h"

int get_status(){
    //! function that gets return status
    char* res = getenv("MERRNO");
    if (res == nullptr){
        perror("Error: Unable to get env variable");
        return 1;
    } else {
        if (res[0] == '0'){
            return 0;
        } else {
            return 1;
        }
    }
}

bool if_dot(const char* d_name){
    //! function that returnes true if path is "." or ".."
    if ((d_name[0] == '.') && (d_name[1] == '\000')){
        return true;
    }
    if ((d_name[0] == '.') && (d_name[1] == '.') && (d_name[2] == '\000')){
        return true;
    }
    return false;
}