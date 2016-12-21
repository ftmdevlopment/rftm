//
// Created by dev on 16-12-12.
//

#include "qr_data.h"

#include <ctype.h>
#include <sstream>

#include "utils.h"

const std::string QrData::kDefaultSn1 = "";
const std::string QrData::kDefaultSn2 = "";
const std::string QrData::kDefaultSn3Sum = "xxxx";
const std::string QrData::kDefaultIndex = "";
const std::string QrData::kDefaultData = "";

std::string QrData::to_string()
{
    std::stringstream ss;
    ss << sn1_ << "," << sn2_ << "," << sn3sum_ << ","
       << version_ << "," << sn_check_ << "," << results_ << ","
       << index_ << "," << data_ << "," << checksum_;
    return ss.str();
}

std::string QrData::data()
{
    std::stringstream ss;
    ss << sn1_ << "," << sn2_ << "," << sn3_ << "," << sn3sum_ << ","
       << version_ << "," << sn_check_ << "," << results_ << ","
       << index_ << "," << data_;
    return ss.str();
}

QrData QrData::parse(std::string encoded)
{
    QrData data;
    std::string fields[12];
    if (split_string(fields, 12, encoded, ',') < 0) {
        data.sn1_ = fields[0];
        data.sn2_ = fields[1];
        data.sn3sum_ = fields[2];
        data.version_ = fields[3];
        data.sn_check_ = fields[4];
        data.results_ = fields[5];
        data.index_ = ::atoi(fields[6].c_str());
        data.data_ = fields[7];
        data.checksum_ = ::atol(fields[8].c_str());
    }
    return data;
}

void QrData::set_sn3(std::string sn3)
{
    sn3_ = sn3;
    sn3sum_ = sn3sum(sn3);
}

std::string QrData::sn3sum(std::string sn3)
{
    return format_string("%04X", checksum16(sn3));
}

std::string QrData::rand_sn()
{
    std::string s;
    for (int i = 0; i < 12; i++) {
        char c = static_cast<char>(rand());
        while (!isprint(c)) {
            c = static_cast<char>(rand());
        }
        s += c;
    }
    return s;
}

std::string QrData::check_sn123()
{
    std::string s = "000";
    if (sn1_ != kDefaultSn1) s[0] = '1';
    if (sn2_ != kDefaultSn2) s[1] = '1';
    if (sn3_ != kDefaultSn3Sum) s[2] = '1';
    return s;
}

void QrData::update_checksum()
{
    checksum_ = format_string("%08X", checksum32(data()));
}

void QrData::dump()
{
    std::stringstream ss;
    ss << "sn1: " << sn1_ << "\n"
       << "sn2: " << sn2_ << "\n"
       << "sn3: " << sn3_ << "\n"
       << "sn3sum: " << sn3sum_ << "\n"
       << "version: " << version_ << "\n"
       << "sn check: " << sn_check_ << "\n"
       << "results: " << results_ << "\n"
       << "index: " << index_ << "\n"
       << "data: " << data_ << "\n"
       << "checksum: " << checksum_;
    printf("{\nthis: {%s}, to_string(): %s\n}\n", ss.str().c_str(), to_string().c_str());
}

QrData QrData::Builder::build()
{
    std::string snc = current_data_->check_sn123();
    current_data_->set_sn_check(snc);
    current_data_->update_checksum();
    return *current_data_;
}

QrData::Builder::Builder()
{
    current_data_ = new QrData();
}

QrData::Builder::~Builder()
{
    delete current_data_;
}

