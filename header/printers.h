#ifndef MYRLS_PRINTERS_H
#define MYRLS_PRINTERS_H

#include <iostream>
#include <vector>
#include <boost/format.hpp>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include "minor.h"
#include "structs.h"
#include "primary.h"


void print_each(const struct mystat* mainInfo);
int recursive_print(const char* dirname, std::vector<mystat>* resVec, int deep);
void print_dir(const std::vector<mystat>* resVec);

#endif //MYRLS_PRINTERS_H
