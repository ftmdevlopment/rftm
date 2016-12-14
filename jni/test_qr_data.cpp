//
// Created by dev on 16-12-14.
//

#include "qr_data.h"

#ifdef _GNU_SOURCE

#include <stdarg.h>
#include <string>
#include <iostream>
#include <vector>

std::string format_string(const char* fmt, ...)
{
    char fixed[1024];

    va_list ap;
    va_start(ap, fmt);

    va_list ap_bakup;
    va_copy(ap_bakup, ap);
    int count = vsnprintf(fixed, sizeof(fixed), fmt, ap_bakup);
    va_end(ap_bakup);


    if (count < (int)sizeof(fixed)) {
        return fixed;
    }

    int length = count + 1;
    char* buff = (char*)malloc(length);
    va_copy(ap_bakup, ap);
    vsnprintf(buff, length, fmt, ap_bakup);
    va_end(ap_bakup);

    va_end(ap);

    std::string result(buff);
    free(buff);
    return result;
}

int split_string(std::string* out, int size, const std::string& in, const char sep, const int max)
{
    if (!out) return -1;
    std::string::size_type last = 0, next = 0;
    for (int i = 0; i < size; i++) {
        next = in.find(sep, last);
        if (next == std::string::npos) {
            next = in.size();
        }

        bool broken = false;
        if (max > 0 && next > last + max) {
            next = last + max;
            broken = true;  // reach line limit
        }

        out[i] = in.substr(last, next - last);
        if (next >= in.size()) {
            return i;
        }

        last = next;
        if (!broken) ++last; // skip separator
    }
    return size;
}

uint32_t checksum32(std::string s)
{
    uint32_t result = 0;
    if (s.size() % 4) {
        s.resize((s.size()/4 + 1)*4, '\0');
    }
    for (size_t i = 0; i < s.size(); i += 4) {
        result ^= (static_cast<uint32_t>(s[i]) << 24)
                  + (static_cast<uint32_t>(s[i+1]) << 16)
                  + (static_cast<uint32_t>(s[i+2]) << 8)
                  + (static_cast<uint32_t>(s[i+3]));
    }
    return result;
}

uint16_t checksum16(std::string s)
{
    uint16_t result = 0;
    if (s.size() % 2) {
        s.resize((s.size()/2 + 1)*2, '\0');
    }
    for (size_t i = 0; i < s.size(); i += 2) {
        result ^= (static_cast<uint16_t>(s[i]) << 8)
                  + (static_cast<uint16_t>(s[i+1]));
    }
    return result;
}

#endif

int __test_qr_data__ = [](){
    for (int i = 0; i < 10; i++) {
        QrData qrdata = QrData::Builder::instance()
                .set_sn1("020f2c347174")
                .set_sn2("123456789012")
                .set_sn3("012345678901")
                .set_version("3.0.0")
                .set_results("00001111222200001111")
                .set_index("idx")
                .set_data("dat")
                .build();
//        qrdata.dump();
        printf("`%s`\n", qrdata.to_string().c_str());
//        printf("cs16: %04X\t", checksum16(qrdata.get_sn3()));
//        printf("cs32: %08X\n", checksum32(qrdata.to_string()));
    }
    return 0;
}();

#ifdef _GNU_SOURCE
int main()
{
    return 0;
}
#endif
