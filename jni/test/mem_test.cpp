//
// Created by dev on 16-12-10.
//

#include "mem_test.h"

void MemTest::RunTest()
{
    std::string out;
    run_command("free -m", &out);

    size_t total = 0, used = 0, avail = 0;
    if (out.find("Mem:") < out.size()) {
        sscanf(out.c_str() + out.find("Mem:") + sizeof("Mem"), "%zu%zu%zu", &total, &used, &avail);

        XLOGI("total: %u, used: %u, free: %u", total, used, free);
        result(format_string("Total %uM\nFree: %uM", total, free));
    }

    uint8_t* ptr = NULL;
    size_t count = avail * 1024;
    for (int i = 0; i < 5; i++) {
        ptr = (uint8_t*)malloc(count);
        if (ptr) break;
        XLOGI("alloc %luB failed, retry...", count);
    }
    if (!ptr) {
        result(format_string("alloc %luB failed!", count));
        return;
    }

    XLOGI("alloced %luB", count);
    result(format_string("alloced %luB", count));

    result("set to 0");
    int start = clock();
    for (size_t i = 0; i < count; i++) {
        ptr[i] = 0;
    }
    int end = clock();
    result("set to 0 done\n" + format_string("use %d ms", end - start));
    sleep(1);

    result("set to 1");
    start = clock();
    for (size_t i = 0; i < count; i++) {
        ptr[i] = 1;
    }
    end = clock();
    result("set to 1 done\n" + format_string("use %d ms", end - start));
    sleep(1);

    result("random write test");
    start = clock();
    for (size_t i = 0; i < count; i++) {
        ptr[i] = static_cast<uint8_t>(rand());
    }
    end = clock();
    result("random write done\n" + format_string("use %d ms", end - start));
    sleep(1);

    free(ptr);
}
