#include "execute.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <vector>

namespace {
std::vector<char*> makeArgv(const std::vector<std::string>& input) {
    std::vector<char*> argv;
    argv.reserve(input.size() + 1);
    for (const auto& arg : input) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    return argv;
}
}

std::vector<pid_t> bgProcesses;

string executeCommand(vector<string> input) {
    if (input.empty()) return "";

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return "";
    }

    if (pid == 0) {
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            _exit(127);
        }
        close(pipefd[1]);

        std::vector<char*> argv = makeArgv(input);
        execvp(argv[0], argv.data());
        std::cerr << "cpp-shell: " << input[0] << ": "
                  << std::strerror(errno) << "\n";
        _exit(127);
    }

    close(pipefd[1]);
    string result;
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        result.append(buffer, static_cast<size_t>(bytesRead));
    }
    close(pipefd[0]);

    int status = 0;
    while (waitpid(pid, &status, 0) == -1 && errno == EINTR) {}
    return result;
}

string executeCommand(vector<string> input, bool background) {
    if (input.empty()) return "";
    if (background && input.back() == "&") input.pop_back();
    if (input.empty()) return "";

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return "";
    }

    if (pid == 0) {
        int devNull = open("/dev/null", O_RDONLY);
        if (devNull != -1) {
            dup2(devNull, STDIN_FILENO);
            close(devNull);
        }

        std::vector<char*> argv = makeArgv(input);
        execvp(argv[0], argv.data());
        std::cerr << "cpp-shell: " << input[0] << ": "
                  << std::strerror(errno) << "\n";
        _exit(127);
    }

    if (background) {
        bgProcesses.push_back(pid);
        cout << "Started background process with PID: " << pid << "\n";
        return "";
    }

    int status = 0;
    while (waitpid(pid, &status, 0) == -1 && errno == EINTR) {}
    return "";
}

void checkBackgroundProcesses() {
    for (auto it = bgProcesses.begin(); it != bgProcesses.end();) {
        int status = 0;
        pid_t result = waitpid(*it, &status, WNOHANG);
        if (result > 0) {
            cout << "Process " << *it << " completed\n";
            it = bgProcesses.erase(it);
        } else if (result == -1 && errno == ECHILD) {
            it = bgProcesses.erase(it);
        } else {
            ++it;
        }
    }
}

string executeCommand(string input, vector<string> commands) {
    if (commands.empty()) return input;

    int inPipe[2] = {-1, -1};
    int outPipe[2] = {-1, -1};
    if (pipe(inPipe) == -1) {
        perror("pipe");
        return "";
    }
    if (pipe(outPipe) == -1) {
        perror("pipe");
        close(inPipe[0]); close(inPipe[1]);
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(inPipe[0]); close(inPipe[1]);
        close(outPipe[0]); close(outPipe[1]);
        return "";
    }

    if (pid == 0) {
        close(inPipe[1]);
        close(outPipe[0]);
        if (dup2(inPipe[0], STDIN_FILENO) == -1) _exit(127);
        if (dup2(outPipe[1], STDOUT_FILENO) == -1) _exit(127);
        close(inPipe[0]);
        close(outPipe[1]);

        std::vector<char*> argv = makeArgv(commands);
        execvp(argv[0], argv.data());
        std::cerr << "cpp-shell: " << commands[0] << ": "
                  << std::strerror(errno) << "\n";
        _exit(127);
    }

    close(inPipe[0]);
    close(outPipe[1]);

    if (!input.empty()) {
        size_t sent = 0;
        while (sent < input.size()) {
            ssize_t written = write(inPipe[1], input.data() + sent, input.size() - sent);
            if (written > 0) sent += static_cast<size_t>(written);
            else if (written == -1 && errno == EINTR) continue;
            else break;
        }
    }
    close(inPipe[1]);

    string result;
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
        result.append(buffer, static_cast<size_t>(bytesRead));
    }
    close(outPipe[0]);
    while (waitpid(pid, nullptr, 0) == -1 && errno == EINTR) {}
    return result;
}
