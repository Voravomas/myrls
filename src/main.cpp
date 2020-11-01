#include <iostream>
#include <boost/program_options.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <boost/format.hpp>
#include <libgen.h>

namespace po = boost::program_options;

struct mystat{
    //! simplified structure, contains only needed values
    int deep;
    std::string owner;
    std::string permission;
    int size;
    std::string time_mod;
    std::string name;
    std::string dirname;
    std::string type;
};

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

// true if left
// false if right
struct compare_stats {
    //! compare structure that counts 3 values: deep, dirname, name.
    inline bool
    operator() (const mystat& left, const mystat& right) const
    {
        if (left.deep != right.deep){
            if (left.deep < right.deep){
                return true;
            } else {
                return false;
            }
        }
        if (left.dirname != right.dirname){
            return left.dirname < right.dirname;
        }
        int res1 = left.name.compare(right.name);
        std::string tempA = left.name;
        std::for_each(tempA.begin(), tempA.end(), [](char & c) {
            c = ::tolower(c);
        });
        std::string tempB = right.name;
        std::for_each(tempB.begin(), tempB.end(), [](char & c) {
            c = ::tolower(c);
        });
        int res2 = tempA.compare(tempB);
        int totRes;
        if (res1 != res2){
            totRes = (-1) * res1;
        } else {
            totRes = res1;
        }
        if (totRes < 0){
            return true;
        } else {
            return false;
        }
    }
};

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

int main(int ac, char *av[]) {
    //setting primary values
    struct stat stFileInfo{};
    setenv("MERRNO", "0", 1);
    std::vector<std::string> str_files;
    std::vector<mystat> resVec;
    std::string path;
    int deep = 0;
    std::string desc_str = std::string("Description:\n\tThis program recursively prints content") +
                                        "\n\tof a file or a directory." +
                                        "\n\tOnly one path is allowed.\nAllowed options";

    //configuring positional arguments
    try{
        po::options_description desc(desc_str);
        desc.add_options()
                ("help,h", "Produce help message")
                ("input-file", po::value< std::vector<std::string> >(), "Enter file or directory")
                ;

        po::positional_options_description p;
        p.add("input-file", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
                options(desc).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("input-file")){
            str_files = vm["input-file"].as<std::vector<std::string> >();
        }
    }
    catch (std::exception& e) {
        std::string tempStr = std::string("Error: ") + e.what();
        perror(tempStr.c_str());
        return 1;
    }

    // error handling for user's argv
    if (str_files.empty()){
        path = "./";
    } else if (str_files.size() == 1){
        path = str_files[0];
    } else if (str_files.size() > 1){
        perror("Error: Only one argument is allowed!");
        return 1;
    }

    //determining if entry is a file or a folder
    if (lstat(path.c_str(), &stFileInfo) < 0){
        perror((std::string("Error: Unable to get info about ") + path).c_str());
        setenv("MERRNO", "1", 1);
        return 1;
    }
    if(!S_ISDIR(stFileInfo.st_mode)){
        //entry is a folder
        struct mystat newStat{};
        getInfo(&stFileInfo, &newStat, &deep, path.c_str());
        print_each(&newStat);
    } else {
        //entry is a file
        recursive_print(path.c_str(), &resVec, deep);
        std::sort(resVec.begin(), resVec.end(), compare_stats{});
        print_dir(&resVec);
    }
    return get_status();
}