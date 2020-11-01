#ifndef MYRLS_FILE_MANIP_H
#define MYRLS_FILE_MANIP_H

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <string>

void permissions(const mode_t* perm, std::string* resStr);
void time_mod(const timespec* ts, std::string* res);
void if_s_link(const char* name, std::string* type);
void type_of_file(const mode_t* mode, std::string* type, const char* name);
std::string cur_dir_extractor();

#endif //MYRLS_FILE_MANIP_H
