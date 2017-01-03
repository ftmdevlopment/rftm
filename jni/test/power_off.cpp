//
// Created by dev on 16-12-10.
//

#include "power_off.h"
#include <stdlib.h>

void PowerOff::RunTest()
{
    result("power off...");
    sleep(1);
    system("reboot -p");
}
