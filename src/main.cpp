#include <iostream>
#include <boost/program_options.hpp>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

//-----
#include "../header/structs.h"
#include "../header/printers.h"

namespace po = boost::program_options;

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