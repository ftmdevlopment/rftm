//
// Created by dev on 16-12-10.
//

#include "wifi_test.h"

#include <sstream>

static bool get_wlan0_state(std::string* state)
{
    std::string out;
    if (!state) return false;
    run_command("netcfg | grep wlan0", &out);
    if (out.find("wlan0") == 0) {
        std::istringstream ss(out.substr(sizeof("wlan0")-1));
        ss >> *state;
        return true;
    }
    XLOGI("get wlan0 state failed!");
    return false;
}

struct ScanResult
{
    int level;
    std::string SSID;
    std::string MAC;

    ScanResult() : level(0) {  }
    ScanResult(const ScanResult& rhs)
            : level(rhs.level),
              SSID(rhs.SSID),
              MAC(rhs.MAC)
    {
    }
};

static bool parse_scan_result(std::vector<ScanResult>* result, const std::string& input)
{
    if (!result) return false;

    result->clear();
    std::string line;
    std::istringstream ss(input);
    std::string::size_type pos = line.npos;
    ScanResult* current = NULL, stub;
    while (std::getline(ss, line)) {
        if ((pos = line.find("          Cell")) == 0) {
            if (current) {
                result->push_back(*current);
            } else {
                current = &stub;
            }
            if ((pos = line.find("Address: ")) != line.npos) {
                current->MAC = line.substr(pos + sizeof("Address: ")-1);
            }
        }
        if ((pos = line.find("ESSID:")) != line.npos) {
            current->SSID = line.substr(pos + sizeof("ESSID:")-1);
        }
        if ((pos = line.find("ESSID:")) != line.npos) {
            current->SSID = line.substr(pos + sizeof("ESSID:")-1);
        }
        if ((pos = line.find("Signal level=")) != line.npos) {
            current->level = ::atoi(line.substr(pos + sizeof("Signal level=")-1).c_str());
        }
    }
    if (current) {
        result->push_back(*current);
    }
    return result->size() > 0;
}

static std::string pick_n_ssid(const std::vector<ScanResult>& ap_list, int npick)
{
    std::string result;
    npick = min(static_cast<int>(ap_list.size()), npick);
    for (int i = 0; i < npick; i++) {
        result += ap_list[i].SSID + "\n";
    }
    return result;
}

static void wifi_up()
{
    std::string out, state;
    if (!get_wlan0_state(&state)) return;
    XLOGI("wlan0 state: %s", state.c_str());

    if (state == "DOWN") {
        run_command("netcfg wlan0 up", &out);
    }

    if (!get_wlan0_state(&state)) return;
    XLOGI("wlan0 state: %s", state.c_str());
}

static void wifi_down()
{
    std::string out, state;
    if (!get_wlan0_state(&state)) return;
    XLOGI("wlan0 state: %s", state.c_str());

    if (state == "UP") {
        run_command("netcfg wlan0 down", &out);
    }

    if (!get_wlan0_state(&state)) return;
    XLOGI("wlan0 state: %s", state.c_str());
}

void WifiTest::RunTest()
{
    std::string out;

    result("wifi up");
    wifi_up();

    result("scan...");
    run_command("iwlist wlan0 scan", &out);

    std::vector<ScanResult> ap_list;
    XLOGI("wifi scan result: ```%s```", out.c_str());
    if (parse_scan_result(&ap_list, out)) {
        XLOGI("parsed scan result: %lu", ap_list.size());
        std::sort(ap_list.begin(), ap_list.end(), [](const ScanResult& a, const ScanResult& b) {
            return a.level > b.level;
        });
        result(pick_n_ssid(ap_list, 3));
        pass();
    } else {
        fail();
    }
    sleep(3);

    wifi_down();
}
