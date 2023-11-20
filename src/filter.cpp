#include "filter.h"

#include <iostream>
#include <vector>
#include <string>

#include "ber.h"

bool match_filter(filter f, std::string uid, std::string cn, std::string mail) {
    std::string compared_value;
    std::string initial_substr;
    std::string final_substr;

    switch (f.tag) {
        case 0:
            return true;
            break;

        case FILTER_EQUALITY_MATCH: {
            if (f.attribute == "uid") {
                compared_value = uid;
            } else if (f.attribute == "cn") {
                compared_value = cn;
            } else if (f.attribute == "mail") {
                compared_value = mail;
            } else {
                return false;
            }

            return compared_value == f.value;
            break;
        }

        case FILTER_SUBSTRINGS: {
            int initial = FILTER_RESULT_UNDEFINED;
            int any = FILTER_RESULT_UNDEFINED;
            int final = FILTER_RESULT_UNDEFINED;

            if (f.attribute == "uid") {
                compared_value = uid;
            } else if (f.attribute == "cn") {
                compared_value = cn;
            } else if (f.attribute == "mail") {
                compared_value = mail;
            } else {
                return false;
            }

            // Initial
            if (f.initial.size() > 0) {
                initial_substr = f.initial[0];
                if (initial_substr.size() > compared_value.size()) {
                    initial = FILTER_RESULT_FALSE;
                } else {
                    initial = compared_value.substr(0, initial_substr.size()) == initial_substr;
                }
            }

            // Final
            if (f.final.size() > 0) {
                final_substr = f.final[0];
                if (final_substr.size() > compared_value.size()) {
                    final = FILTER_RESULT_FALSE;
                } else {
                    final = compared_value.substr(compared_value.size() - final_substr.size(), final_substr.size()) == final_substr;
                }
            }

            // Any
            if (f.any.size() > 0) {
                for (std::string substr : f.any) {
                    if (substr.size() > compared_value.size()) {
                        any = FILTER_RESULT_FALSE;
                        break;
                    }

                    if (initial == FILTER_RESULT_TRUE) {
                        compared_value = compared_value.substr(initial_substr.size(), compared_value.size() - initial_substr.size());
                    }

                    if (final == FILTER_RESULT_TRUE) {
                        compared_value = compared_value.substr(0, compared_value.size() - final_substr.size());
                    }

                    if (compared_value.find(substr) != std::string::npos) {
                        any = FILTER_RESULT_TRUE;
                        compared_value = compared_value.substr(0, compared_value.find(substr)) + compared_value.substr(compared_value.find(substr) + substr.size(), compared_value.size() - compared_value.find(substr) - substr.size());
                    } else {
                        any = FILTER_RESULT_FALSE;
                        break;
                    }
                }
            }

            bool result = true;

            if (initial != FILTER_RESULT_UNDEFINED) {
                result = result & initial;
            }
            if (any != FILTER_RESULT_UNDEFINED) {
                result = result & any;
            }
            if (final != FILTER_RESULT_UNDEFINED) {
                result = result & final;
            }

            return result;
            break;
        }

        case FILTER_AND: {
            for (filter f1 : f.filters) {
                if (!match_filter(f1, uid, cn, mail)) {
                    return false;
                }
            }
            return true;
            break;
        }

        case FILTER_OR: {
            std::cout << "parsing " << cn << std::endl;
            for (filter f1 : f.filters) {
                if (match_filter(f1, uid, cn, mail)) {
                    return true;
                }
            }
            return false;
            break;
        }

        case FILTER_NOT: {
            return !match_filter(f.filters[0], uid, cn, mail);
            break;
        }

        default:
            return false;
            break;
    }
}
