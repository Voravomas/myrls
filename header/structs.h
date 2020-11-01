#ifndef MYRLS_STRUCTS_H
#define MYRLS_STRUCTS_H

#include <iostream>
#include <string>
#include <algorithm>

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

#endif //MYRLS_STRUCTS_H
