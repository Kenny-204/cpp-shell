#include "pipes.h"
#include "execute.h"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

vector<vector<string>> seprateCommands(vector<string> input) {
    vector<vector<string>> commands;
    vector<string> current;

    for (const string& token : input) {
        if (token == "|") {
            if (!current.empty()) commands.push_back(current);
            current.clear();
        } else {
            current.push_back(token);
        }
    }
    if (!current.empty()) commands.push_back(current);
    return commands;
}

string handlePipes(vector<string> input) {
    vector<vector<string>> commands = seprateCommands(input);
    if (commands.empty()) return "";
    if (commands.size() == 1) return executeCommand(commands[0]);

    const size_t count = commands.size();
    vector<vector<int>> pipes(count - 1, vector<int>(2));
    for (auto& pipefd : pipes) {
        if (::pipe(pipefd.data()) == -1) {
            perror("pipe");
            return "";
        }
    }

    int outputPipe[2];
    if (::pipe(outputPipe) == -1) {
        perror("pipe");
        for (auto& pipefd : pipes) { close(pipefd[0]); close(pipefd[1]); }
        return "";
    }

    vector<pid_t> children;
    for (size_t i = 0; i < count; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return "";
        }

        if (pid == 0) {
            if (i > 0) dup2(pipes[i - 1][0], STDIN_FILENO);
            if (i + 1 < count) dup2(pipes[i][1], STDOUT_FILENO);
            else dup2(outputPipe[1], STDOUT_FILENO);

            for (auto& pipefd : pipes) {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            close(outputPipe[0]);
            close(outputPipe[1]);

            vector<char*> argv;
            argv.reserve(commands[i].size() + 1);
            for (const string& arg : commands[i]) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);
            execvp(argv[0], argv.data());
            cerr << "cpp-shell: " << commands[i][0] << ": "
                 << strerror(errno) << "\n";
            _exit(127);
        }
        children.push_back(pid);
    }

    close(outputPipe[1]);
    for (auto& pipefd : pipes) {
        close(pipefd[0]);
        close(pipefd[1]);
    }

    string result;
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(outputPipe[0], buffer, sizeof(buffer))) > 0) {
        result.append(buffer, static_cast<size_t>(bytesRead));
    }
    close(outputPipe[0]);

    for (pid_t child : children) {
        int status = 0;
        while (waitpid(child, &status, 0) == -1 && errno == EINTR) {}
    }
    return result;
}


