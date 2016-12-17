//
// Created by dev on 16-12-15.
//

#include <wait.h>
#include <fcntl.h>
#include "camera_test.h"

static const char* kCameraTestBin = "/system/bin/camera_test";
static const char* kCameraTestArgs[] = {kCameraTestBin, "0", NULL};

static void zap_stdio(void)
{
    int fd;
    fd = open("/dev/null", O_RDWR);
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    close(fd);
}

static pid_t fork_and_execve(char** argv, char** envp)
{
    const pid_t pid = fork();
    if (pid < 0) {
        return pid;
    } else if (pid == 0) {
        zap_stdio();
        execve(argv[0], argv, envp);
        _exit(-1);  // never should been here
    }
    return pid;
}

static pid_t get_pid_status(pid_t pid, int retries, int* status)
{
    pid_t child = -1;
    for (int i = 0; i < retries; i++) {
        child = waitpid(pid, status, WNOHANG);
        if (child == pid || child == -1) {
            break;
        }
    }
    return child;
}

void CameraTest::RunTest()
{
    pid_t pid = -1;
    pid_t child = -1;
    int status = 0;
    std::string out;

    if (!file_exists(kCameraTestBin)) {
        result(format_string("%s not exists!", kCameraTestBin));
        fail();
        return;
    }

    gr_color(0, 0, 0, 0xff);
    gr_clear();
    gr_flip();

    // start background test process
    pid = fork_and_execve((char**) kCameraTestArgs, environ);
    if (pid == -1) {
        result(format_string("fork fail"));
        fail();
        return;
    }
    XLOGI("%s start as %d.", kCameraTestBin, pid);

    // wait for user judge
    set_alarm(0);
    sleep(2);
    XLOGI("wait for user judge...");
    wait_for_judge_result();
    XLOGI("got user judge...");

    // kill background test process
    kill(pid, SIGKILL);

    // check stop or not ...
    child = get_pid_status(pid, 10, &status);
    if (WIFEXITED(status)) {
        XLOGI("child %d exited with %d", child, WEXITSTATUS(status));
    }
    if (WIFSIGNALED(status)) {
        XLOGI("child %d exited with %d term signals, %d stop signals",
              child, WTERMSIG(status), WSTOPSIG(status));
    }
    if (WCOREDUMP(status)) {
        XLOGI("child %d core dumped!", child);
    }
    set_alarm_ms(1);
    clear_judge_result();
}

