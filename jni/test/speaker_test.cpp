//
// Created by dev on 16-12-9.
//

#include "speaker_test.h"

#ifdef FTM_SND_PATH
std::string left_sound = FTM_SND_PATH "left_test.wav";
std::string right_sound = FTM_SND_PATH "right_test.wav";
#else  // FTM_SND_PATH
std::string left_sound = "/data/local/tmp/left_test.wav";
std::string right_sound = "/data/local/tmp/right_test.wav";
#endif  // FTM_SND_PATH

void SpeakerTest::RunTest()
{
    set_alarm(0);
    std::string out;

    // check sound file exists
    if (!file_exists(left_sound.c_str()) || !file_exists(right_sound.c_str())) {
        result("sound file not exist!");
        goto FAIL;
    }

    run_command("tinymix 2 100", &out);
    out = trim_string(out);
    if (out.size()) { // on success, no output
        result(out);
        goto FAIL;
    }

    result("left channel");
    run_command(("tinyplay " + left_sound).c_str(), &out);
    out = trim_string(out);

    // on success, output:
    // Playing sample: 2 ch, 48000 hz, 16 bit
    if (out.find("Playing")) {
        result(out);
        goto FAIL;
    }

    result("right channel");
    run_command(("tinyplay " + right_sound).c_str(), &out);
    out = trim_string(out);

    // on success, output:
    // Playing sample: 2 ch, 48000 hz, 16 bit
    if (out.find("Playing")) {
        result(out);
        goto FAIL;
    }

    result("PASS/FAIL?");
    wait_for_judge_result();
    set_alarm_ms(1);
    clear_judge_result();
    return;

FAIL:
    fail();
}
