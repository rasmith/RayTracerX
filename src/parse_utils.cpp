/*
 * parse_utils.cpp
 *
 *  Created on: Sep 5, 2013
 *      Author: agrippa
 */
#include <cstring>
#include <climits>
#include <ctype.h>
#include "parse_utils.hpp"
namespace parse_utils {
bool ParseInt(const char* str, int* value) {
    int base = 10;
    int offset = 1;
    int pos = strlen(str);
    int digit = 0;
    bool success = (pos > 0);
    bool overflow = false;
    *value = 0;
    while (--pos >= 0 && (success = !overflow && isdigit(str[pos]))) {
        digit = static_cast<int>(str[pos] - '0');
        overflow |= (pos > 0 && digit != 0) && (offset > INT_MAX / digit);
        overflow |= (pos > 0 && digit != 0) && (*value > INT_MAX - digit * offset);
        *value += digit * offset;
        overflow |= (pos > 0) && (offset > INT_MAX / base);
        offset *= base;
    }
    return success;
}
}
