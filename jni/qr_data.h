//
// Created by dev on 16-12-12.
//

#ifndef RFTM_QR_DATA_H
#define RFTM_QR_DATA_H

#include <string>

class QrData
{
    std::string sn1_;
    std::string sn2_;
    std::string sn3_;
    std::string sn3sum_; //
    std::string version_;
    std::string sn_check_; //
    std::string results_;
    std::string index_;
    std::string data_;
    std::string checksum_;

public:
    QrData() = default;
    QrData(const QrData& rhs) = default;

    void set_sn1(std::string sn1) { sn1_ = sn1; }
    std::string get_sn1() const { return sn1_; }

    void set_sn2(std::string sn2) { sn2_ = sn2; }
    std::string get_sn2() const { return sn2_; }

    void set_sn3(std::string sn3);  // { sn3_ = sn3; }
    std::string get_sn3() const { return sn3_; }

//    void set_sn3sum(std::string sn3sum) { sn3sum_ = sn3sum; }
    std::string get_sn3sum() const { return sn3sum_; }

    void set_sn_check(std::string sn_check) { sn_check_ = sn_check; }
    std::string get_sn_check() const { return sn_check_; }

    void set_version(std::string v) { version_ = v; }
    std::string get_version() const { return version_; }

    void set_results(std::string results) { results_ = results; }
    std::string get_results() const { return results_; }

    void set_index(std::string idx) { index_ = idx; }
    std::string get_index() const { return index_; }

    void set_data(std::string data) { data_ = data; }
    std::string get_data() const { return data_; }

//    void set_checksum(uint32_t checksum) { checksum_ = checksum; }
    std::string get_checksum() const { return checksum_; }

    std::string to_string();

    std::string data();

    void update_checksum();

    void dump(); // for debug

    // util routines for qrdata
    static QrData parse(std::string encoded);

    static std::string sn3sum(std::string sn3);

    static std::string rand_sn();

    std::string check_sn123();

    struct Builder {
    private:
        Builder();
        ~Builder();
        QrData* current_data_;
    public:
        static Builder& instance() {
            static Builder stub;
            return stub;
        }

        Builder& set_sn1(std::string sn1) {
            current_data_->set_sn1(sn1);
            return *this;
        }

        Builder& set_sn2(std::string sn2) {
            current_data_->set_sn2(sn2);
            return *this;
        }

        Builder& set_sn3(std::string sn3) {
            current_data_->set_sn3(sn3);
            return *this;
        }

        Builder& set_version(std::string ver) {
            current_data_->set_version(ver);
            return *this;
        }

        Builder& set_results(std::string results) {
            current_data_->set_results(results);
            return *this;
        }

        Builder& set_index(std::string idx) {
            current_data_->set_index(idx);
            return *this;
        }

        Builder& set_data(std::string data) {
            current_data_->set_data(data);
            return *this;
        }

        QrData build();
    };
};


#endif // RFTM_QR_DATA_H
