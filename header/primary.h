#ifndef MYRLS_PRIMARY_H
#define MYRLS_PRIMARY_H

#include <iostream>
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>
#include <pwd.h>
#include "structs.h"
#include "file_manip.h"

void getInfo(const struct stat* primInfo, struct mystat* mainInfo, const int* deep, const char* name);

#endif //MYRLS_PRIMARY_H
