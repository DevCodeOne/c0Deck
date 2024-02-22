#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <algorithm>
#include <cstdlib>
#include <mutex>
#include <chrono>
#include <iterator>
#include <future>

#include "commands.h"
#include "serveractions.h"

void Commands::initializeComponent(ServerActions &actions) {
    actions.registerFunc(&Commands::execute_command, "exec");

    {
        std::unique_lock processesGuard{processesMutex};
        processChecker = std::jthread([](std::stop_token token) {
            using namespace std::chrono_literals;
            while (!token.stop_requested()) {
                // Lock guard block
                {
                    std::unique_lock processesGuard{processesMutex};

                    auto newEnd = std::remove_if(processes.begin(), processes.end(), [](auto &currentProcess) {
                        if (!currentProcess.returnCode.valid()) {
                            return false;
                        }

                        if (currentProcess.returnCode.wait_for(10ms) == std::future_status::ready) {
                            const auto result = currentProcess.returnCode.get();
                            serverLogger->debug("Future is ready");
                            // TODO: do something with the result
                            return true;
                        }

                        return false;
                    });

                    if (newEnd != processes.cend()) {
                        serverLogger->debug("{} threads are done removing them", std::abs(std::distance(newEnd, processes.end())));
                        processes.erase(newEnd);
                    }
                }

                std::this_thread::sleep_for(500ms);
            }

            serverLogger->debug("Server should exit, stopping all processes ....");

            for (auto &currentProcess : processes) {
                kill(currentProcess.processId, SIGINT);
            }

            // Wait for x amount for all processes to stop
            std::this_thread::sleep_for(std::chrono::seconds(2));

            for (auto &currentProcess : processes) {
                kill(currentProcess.processId, SIGKILL);
            }

            serverLogger->debug("Stopped processes ...");

        });

    }
}

int Commands::execute_command(std::string command, bool postResults) {
    {
        // TODO: make configurable
        static constexpr size_t maxProcesses = 20;
        std::unique_lock processesGuard{processesMutex};
        if (processes.size() > maxProcesses) {
            // TOO many processes are already running
            return -1;
        }
    }

    int newOutput[2];

    if (pipe(newOutput) == -1) {
        // Couldn't create pipes
        serverLogger->debug("Couldn't create pipes");
    }

    int readPipe = newOutput[0];
    int writePipe = newOutput[1];

    serverLogger->debug("Executing command {} {} {} {} ", "/bin/sh", "sh", "-c ", command.c_str());

    pid_t createdProcess = fork();

    if (createdProcess == -1) {
        // Error forking
        return -1;
    } else if (createdProcess == 0) {
        // Child
        close(readPipe);
        
        // TODO: maybe handle output with files, which can be send, when requested
        dup2(writePipe, STDERR_FILENO);
        dup2(writePipe, STDOUT_FILENO);
        execl("/bin/sh", "sh", "-c", command.c_str(), nullptr);

        // If failed print error and return
        close(writePipe);
        exit(EXIT_SUCCESS);
    }

    // Parent
    auto returnValue = std::async(std::launch::async, 
    [createdProcess, readPipe, writePipe]() {
        // Wait for process to finish 
        serverLogger->debug("Parent waiting for child process {} to finish", (int) createdProcess);
        int status = 0;
        while (waitpid(createdProcess, &status, 0) < 0) {
            if (errno != EINTR) {
                status = -1;
                break;
            }
        }

        serverLogger->debug("Child process {} has finished with status code {}", (int) createdProcess, status);

        // When finished
        close(readPipe);
        close(writePipe);

        return status;
    });

    {
        std::unique_lock processGuard{processesMutex};
        processes.emplace_back(Process{.command = command, .returnCode = std::move(returnValue), .processId = createdProcess});
    }

    serverLogger->debug("Exiting command func");

    return 0;
}