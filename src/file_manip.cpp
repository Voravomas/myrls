#include "../header/file_manip.h"

void permissions(const mode_t* perm, std::string* resStr){
    //! function that gets permission of a file
    char res[10];
    unsigned int constants[] = {S_IRUSR, S_IWUSR, S_IXUSR,
                                S_IRGRP, S_IWGRP, S_IXGRP,
                                S_IROTH, S_IWOTH, S_IXOTH};
    char rwx[] = {'r', 'w', 'x', 'r', 'w', 'x', 'r', 'w', 'x'};
    for (auto i = 0; i < 9; i++){
        res[i] = (*perm & (unsigned) constants[i]) ? rwx[i] : '-';
    }
    res[9] = '\0';
    *resStr = res;
}

void time_mod(const timespec* ts, std::string* res){
    //! function that gets last time modified in correct form
    char temp[100];
    strftime(temp, sizeof temp, "%F %T", gmtime(&ts->tv_sec));
    *res = std::string(temp);
}

void if_s_link(const char* name, std::string* type){
    //! function that adds additional info if a file is a symbol link
    char buf[100];
    ssize_t res = readlink(name, buf, sizeof buf);
    if (res == -1){
        perror("Error: Unable to read Symbolic link");
        setenv("MERRNO", "1", 1);
    } else {
        *type = "@ -> " + std::string(buf);
    }
}

void type_of_file(const mode_t* mode, std::string* type, const char* name){
    //! function that adds special symbol in order to know file type
    /*
    / -- директорія
    * -- виконавчий,
    @ - symlink, вказувати, на що посилається (приклад: fd -> /proc/self/fd/)
    | -- іменований канал,
    = -- сокет,
    ? - всі інші.
    */
    if (S_ISREG(*mode)){
        if (*mode & S_IXUSR){
            *type = "*";
        } else {
            *type = "";
        }
        return;
    }
    switch (*mode & S_IFMT) {
        case S_IFDIR:  *type = "/";            break;
        case S_IFIFO:  *type = "|";            break;
        case S_IFLNK:  if_s_link(name, type);  break;
        case S_IFREG:  *type = "";             break;
        case S_IXUSR:  *type = "*";            break;
        case S_IFSOCK: *type = "=";            break;
        default:       *type = "?";            break;
    }

}

std::string cur_dir_extractor(){
    //! function that extracts nearest directory name by using realpath
    char newpath[PATH_MAX];
    char* res = realpath(".", newpath);
    if (res == nullptr){
        perror("Error: Unable to get realpath with '.' path");
        setenv("MERRNO", "1", 1);
    }
    std::string tempStr = std::string(newpath);
    int idx = tempStr.rfind('/');
    std::string tempStr2 = "." + tempStr.substr(idx, std::string::npos);
    return tempStr2;
}