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

    run_command(format_string("ls -l %s", kCameraTestBin), &out);
    if (out.find("No such file") != out.npos) {
        result(format_string("%s", out.c_str()));
        goto FAIL;
    }

    // start background test process
    pid = fork_and_execve((char**) kCameraTestArgs, environ);
    if (pid == -1) {
        result(format_string("fork fail"));
        goto FAIL;
    }

    // delay
    set_alarm(10);
    sleep(10);

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

    pass();
    return;

FAIL:
    fail();
}
