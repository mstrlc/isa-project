// filter_functions.h

#ifndef FILTER_FUNCTIONS_H
#define FILTER_FUNCTIONS_H

#include <string>
#include <vector>

struct filter {
    unsigned char tag;
    std::vector<filter> filters;
    std::string attribute;
    std::string value;
    std::vector<std::string> initial;
    std::vector<std::string> any;
    std::vector<std::string> final;
};

enum filter_result {
    FILTER_RESULT_FALSE,
    FILTER_RESULT_TRUE,
    FILTER_RESULT_UNDEFINED
};

bool match_filter(filter f, std::string uid, std::string cn, std::string mail);

#endif // FILTER_FUNCTIONS_H
