//
// Created by dev on 16-12-10.
//

#include "mem_test.h"
#include "../timer.h"

#include <stdlib.h>

static const int MEGA = 1024 * 1024;
static const int PAGE = 4096;

void MemTest::RunTest()
{
    uint8_t* ptr = NULL;
    Timer timer;
    std::vector<uint8_t> buffer;

    std::string out;
    run_command("free -m", &out);

    size_t total = 0, used = 0, avail = 0, count;
    if (out.find("Mem:") == out.npos) {
        goto FAILURE;
    }
    if (sscanf(out.c_str() + out.find("Mem:") + sizeof("Mem"),
               "%zu%zu%zu", &total, &used, &avail) != 3) {
        goto FAILURE;
    }
    XLOGI("total: %u, used: %u, free: %u", total, used, free);

    count = avail * 8 / 10 * MEGA;
    for (int i = 0; i < 5; i++) {
        ptr = (uint8_t*)malloc(count);
        if (ptr) break;
        XLOGI("alloc %luB failed, retry...", count);
    }
    if (!ptr) {
        result(format_string("alloc %luB failed!", count));
        goto FAILURE;
    }

    XLOGI("alloced %luB", count);
    result(format_string("alloced %luB", count));

    result("set to 0");
    buffer.resize(PAGE);
    for (size_t i = 0; i < buffer.size(); i++) {
        buffer[i] = 0;
    }
    timer.reset();
    for (size_t i = 0; i < count; i += buffer.size()) {
        buffer[i / PAGE % PAGE] &= 0;
        memcpy(ptr + i, &buffer[0], buffer.size());
    }
    result("set to 0 done\n" + format_string("use %.6fs", timer.elapsed()));
    sleep(1);

    result("set to random");
    buffer.resize(PAGE);
    for (size_t i = 0; i < buffer.size(); i++) {
        buffer[i] = static_cast<uint8_t>(rand());
    }
    timer.reset();
    for (size_t i = 0; i < count; i += buffer.size()) {
        buffer[i / PAGE % PAGE] ^= i;
        memcpy(ptr + i, &buffer[0], buffer.size());
    }
    result("set to random done\n" + format_string("use %.6fs", timer.elapsed()));
    sleep(1);

    free(ptr);

    result(format_string("Memory: %uM\nPASS", total));

    pass();
    return;

FAILURE:
    result("FAIL");
    fail();
}
