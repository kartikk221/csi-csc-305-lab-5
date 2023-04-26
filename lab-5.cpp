#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <cstring>

int main() {
    // Define the number N up to which the summation will be computed
    int N = 10;

    // Create 3 unique pipes for two-way communication
    int pipe_1[2], pipe_2[2], pipe_3[2];

    // Initialize pipes using the pipe() system call
    pipe(pipe_1);
    pipe(pipe_2);
    pipe(pipe_3);

    // Fork a new process using the fork() system call
    pid_t pid = fork();

    if (pid == -1) {
        std::cerr << "Fork failed" << std::endl;
        exit(1);
    }

    if (pid > 0) { // Parent process
        // Close the read end of the first pipe
        close(pipe_1[0]);

        // Send message to the child process
        const char* parent_msg = "Hello from Parent!";
        write(pipe_1[1], parent_msg, strlen(parent_msg) + 1);

        // Close the write end of the second pipe
        close(pipe_2[1]);

        // Read and print the message from the child
        char child_msg[100];
        read(pipe_2[0], child_msg, 100);
        std::cout << "Parent received: " << child_msg << std::endl;

        // Compute the partial sum of 1 through N/2
        int parent_sum = 0;
        for (int i = 1; i <= N / 2; i++) {
            parent_sum += i;
        }

        // Close the read end of the third pipe
        close(pipe_3[0]);

        // Read the child's result from the third pipe
        int child_sum;
        read(pipe_3[1], &child_sum, sizeof(child_sum));

        // Compute the total sum
        int total_sum = parent_sum + child_sum;

        // Print the total sum
        std::cout << "Total sum: " << total_sum << std::endl;
    } else { // Child process
        // Close the write end of the first pipe
        close(pipe_1[1]);

        // Read and print the message from the parent
        char parent_msg[100];
        read(pipe_1[0], parent_msg, 100);
        std::cout << "Child received: " << parent_msg << std::endl;

        // Close the read end of the second pipe
        close(pipe_2[0]);

        // Send message to the parent process
        const char* child_msg = "Hello from Child!";
        write(pipe_2[1], child_msg, strlen(child_msg) + 1);

        // Compute the partial sum of N/2+1 through N
        int child_sum = 0;
        for (int i = N / 2 + 1; i <= N; i++) {
            child_sum += i;
        }

        // Close the write end of the third pipe
        close(pipe_3[1]);

        // Send the child's result to the parent through the third pipe
        write(pipe_3[0], &child_sum, sizeof(child_sum));
    }

    return 0;
}