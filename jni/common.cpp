//
// Created by dev on 16-12-21.
//

#include "common.h"
#include "utils.h"

#include <string>

const char* get_serial()
{
    static char buffer[128];
    std::string out;
    run_command("getprop ro.serialno", &out);
    strncpy(buffer, trim_string(out).c_str(), sizeof(buffer));
    return buffer;
}