//
// Created by dev on 16-12-13.
//

#include "mic_test.h"
#include "../blocking_queue.h"

#include <tinyalsa/asoundlib.h>
#include <assert.h>

static bool show_frame_queue_size = false;

static double sample_diff_passed_threshold = 1e7;

static const unsigned int kPcmCard = 0;
static const unsigned int kPcmDevice = 0;

#ifdef RESOURCE_PATH
static const std::string kTestSound1 = RESOURCE_PATH "test1.wav";
static const std::string kTestSound2 = RESOURCE_PATH "test2.wav";
#else  // RESOURCE_PATH
static const std::string kTestSound1 = "/data/local/tmp/test1.wav";
static const std::string kTestSound2 = "/data/local/tmp/test2.wav";
#endif  // RESOURCE_PATH

static struct pcm_config config = {
        .channels = 16,
        .rate = 48000,
        .period_size = 8192,
        .period_count = 16,
        .format = PCM_FORMAT_S32_LE,
};

static void hex_dump(void* ptr, int size)
{
    unsigned char* c = (unsigned char*) ptr;
    for (int i = 0; i < size; i++) {
        printf("%02X%s", c[i], (i + 1) % 8 == 0 ? "\n" : " ");
    }
    puts("");
}

struct Frame
{
    // PCM_FORMAT_S32_LE
    using value_type = int32_t;
    std::vector<value_type> values_;

    Frame() = default;

    Frame(const Frame&) = default;

    Frame(char* buffer, unsigned size)
    {
        // PCM_FORMAT_S32_LE
        const size_t N = (sizeof(value_type) / sizeof(decltype(buffer[0])));
        size_t count = size / N;
        assert(count == config.channels);
        values_.resize(count);
        for (size_t i = 0; i < count; i++) {
            // Little Endian: least significant byte in the smallest address.
            values_[i] = (static_cast<uint32_t>(buffer[i*N]))
                         + (static_cast<uint32_t>(buffer[i*N + 1]) << 8)
                         + (static_cast<uint32_t>(buffer[i*N + 2]) << 16)
                         + (static_cast<uint32_t>(buffer[i*N + 3]) << 24);
        }
    }

    static constexpr double a = -1e8;

    // map value to (0, 1)
    static double normalize(int32_t x)
    {
        if (x < 0) {
            return -normalize(-x);
        }
        return a/(x - a) + 1.0;
    }

    static const int kFiltedSize = 8;  // config.channels / 2;
    Frame filt() { // filt half of channels
        Frame result;
        result.values_.reserve(kFiltedSize);
        for (int i = 0; i < values_.size(); i++) {
            if (i % 2 == 0) {
                result.values_.push_back(values_[i]);
            }
        }
        return result;
    }
};

static BlockingQueue<bool> ready;
static BlockingQueue<bool> finish;
static BlockingQueue<Frame> frame_queue;

static double sample_max[Frame::kFiltedSize];
static double sample_min[Frame::kFiltedSize];
static double sample_sum[Frame::kFiltedSize];
static double sample_2sum[Frame::kFiltedSize];
static double sample_mean[Frame::kFiltedSize];
static double sample_variance[Frame::kFiltedSize];
static int sample_count;

static void* capture_work(void* arg)
{
    char* buffer;
    unsigned int size, bytes_read = 0;
    struct pcm* pcm = static_cast<struct pcm*>(arg);

    size = pcm_frames_to_bytes(pcm, 1);
    buffer = (char*) malloc(size);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm);
        return reinterpret_cast<void*>(1);
    }

    XLOGI("Capturing sample: %u ch, %u hz, %u bit\n", config.channels, config.rate,
          pcm_format_to_bits(config.format));

    XLOGI("read size: %d", size);
    XLOGI("frame size: %d", pcm_frames_to_bytes(pcm, 1));
    XLOGI("buffer size: %d", pcm_get_buffer_size(pcm));

    ready.put(true);

    bool done = false;
    int count = 0;
    frame_queue.reserve(pcm_get_buffer_size(pcm));

    struct timespec ts_start, ts_end;
    clock_gettime(CLOCK_REALTIME, &ts_start);
    while (!done && !pcm_read(pcm, buffer, size)) {
        finish.peek(&done);
        frame_queue.put(Frame(buffer, size));
        bytes_read += size;
        count++;
    }
    clock_gettime(CLOCK_REALTIME, &ts_end);
    double elapsed = (ts_end.tv_sec + ts_end.tv_nsec/1e9
                      - (ts_start.tv_sec + ts_start.tv_nsec/1e9));
    XLOGI("captured %u bytes, %d frames, in %g sec", bytes_read, count, elapsed);

    return NULL;
}

void MicTest::RunTest()
{
    set_alarm(0);
    ready.clear();
    finish.clear();
    frame_queue.clear();

#define FILL_ARRAY(a, v) std::fill(a, a + sizeof(a)/sizeof(a[0]), v)

    FILL_ARRAY(sample_max, 0.0);
    FILL_ARRAY(sample_min, 0.0);
    FILL_ARRAY(sample_sum, 0.0);
    FILL_ARRAY(sample_2sum, 0.0);
    FILL_ARRAY(sample_mean, 0.0);
    FILL_ARRAY(sample_variance, 0.0);
    sample_count = 0;

#undef FILL_ARRAY

    struct pcm* pcm = pcm_open(kPcmCard, kPcmDevice, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device (%s)\n",
                pcm_get_error(pcm));
        return;
    }

    pthread_t bg_worker;
    pthread_create(&bg_worker, NULL, capture_work, pcm);

    std::string out;
#define EXEC(cmd) \
    do { \
        run_command(cmd, &out); \
        if (out.find("not found") != out.npos) { \
            goto DONE; \
        } \
    } while (0)

    EXEC("tinymix 2 100");

    XLOGI("wait for bg_worker ready...");
    ready.take();

    sleep(3);

    XLOGI("play....");
    EXEC("tinyplay " + kTestSound1);

    sleep(3);
    EXEC("tinyplay " + kTestSound2);

    sleep(3);
#undef EXEC

DONE:
    XLOGI("notify bg_worker done!");
    finish.put(true);

    XLOGI("wait for bg_worker done....");
    frame_queue.clear();
    pthread_join(bg_worker, NULL);
    XLOGI("close pcm...");
    pcm_close(pcm);

    // check sample diff for each channel.
    for (int i = 0; i < Frame::kFiltedSize; i++) {
        XLOGI("%d: %g\t%g\t%g\n", i, sample_variance[i], sample_max[i] - sample_min[i], sample_mean[i]);
        if (sample_max[i] - sample_min[i] < sample_diff_passed_threshold) {
            goto FAILURE;
        }
    }

    pass();
    set_alarm_ms(1);
    return;

FAILURE:
    fail();
    set_alarm_ms(1);
}

static const int kBarGap = 5;
static const int kBarWidth = 15;
static const int kMaxBarHeight = 70;
static rect_t bg_rect = {0, 0, 168, 160};

static const color_t bg_color = {0x00, 0x00, 0x00, 0xff};
static const color_t bar_color = {0xff, 0xff, 0xff, 0xff};
static const color_t neg_color = {0x00, 0xff, 0xff, 0xff};

void MicTest::Draw()
{
    if (bg_rect.x == 0) {
        bg_rect.x = (gr_fb_width() - bg_rect.w) / 2;
        bg_rect.y = (gr_fb_height() - bg_rect.h) / 2;
    }

    if (show_frame_queue_size) {
        char text[64];
        snprintf(text, sizeof(text), "pending: %6zu", frame_queue.size());

        int tw = gr_measure(text);
        int fw = 0, fh = 0;
        gr_font_size(&fw, &fh);
        int x = (gr_fb_width() - tw) / 2;
        int y = gr_fb_height() / 2 + bg_rect.h / 2;

        set_color(&bg_color);
        gr_fill(x, y, x + tw, y + fh);

        set_color(&bar_color);
        fill_text(x + tw/2, y + fh/2, text, 0);
    }

    auto frames = frame_queue.peek_all();
    if (frames.size() > 0) {
        set_color(&bg_color);
        fill_rect(&bg_rect);

        Frame frame = frames[0].filt();
        for (size_t i = 0; i < frame.values_.size(); i++) {
            auto val = frame.values_[i];

            // statics counting
            double x = val; // Frame::normalize(val);
            sample_sum[i] += x;
            sample_max[i] = std::max(x, sample_max[i]);
            sample_min[i] = std::min(x, sample_min[i]);
            sample_2sum[i] += x * x;
            sample_mean[i] = sample_sum[i] / sample_count;
            sample_variance[i] = (sample_2sum[i] + sample_mean[i] * sample_mean[i]) / sample_count;

            // calculate bars
            rect_t rect;
            const color_t* color = &bar_color;
            rect.w = kBarWidth;
            rect.x = (gr_fb_width() - bg_rect.w) / 2
                     + static_cast<int>(i) * (kBarWidth + kBarGap) + kBarGap;
            if (val < 0) {
                rect.h = static_cast<int>(Frame::normalize(-val) * kMaxBarHeight);
                rect.y = gr_fb_height() / 2;
                color = &neg_color;
            } else {
                rect.h = static_cast<int>(Frame::normalize(val) * kMaxBarHeight);
                rect.y = gr_fb_height() / 2 - rect.h;
            }

            // draw bars
            set_color(color);
            fill_rect(&rect);
        }
        sample_count++;
    }
    gr_flip();
}
