#include <linux/limits.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <fcntl.h>

/* Referenced code from Prof. W. Michael Petullo's exec.c code example to complete this assignment. */

const int CMD_MAX = 1024;
int input_redir_count = 0, output_redir_count = 0, pipe_count = 0;

/*
 * Takes a string, command_line, breaks command_line into tokens as
 * delimited by strings, and fills the array argv with pointers
 * to each token. This leaves argv as an array of pointers to NULL-terminated
 * strings representing the tokens. The argv array is itself NULL-terminated.
 * Modifies command_line.
 *
 * Returns the number of tokens.
 *
 * WARNING: this function assumes command_line ends with a '\n'; you must
 * arrange for this to be the case. It is okay to reject overly-long
 * command lines (> CMD_MAX) elsewhere in your program.
 */
int
parse(char *argv[], char *command_line)
{
	int i;
	char *saveptr = NULL;

	assert(command_line[strlen(command_line) - 1] == '\n');

	/* Drop newline. */
	command_line[strlen(command_line) - 1] = '\0';

	/* strtok_r is thread-safe; strtok is not. */
	argv[0] = strtok_r(command_line, " ", &saveptr);

	/*
	 * Careful with those buffer sizes!
	 * + 2: cover duplicate AND '\0' terminator.
	 */
	for (i = 0; i + 2 < ARG_MAX && argv[i]; i++) {
		argv[i + 1] = strtok_r(NULL, " ", &saveptr);
	}

	argv[i + 1] = '\0';

	return i;
}

/*
 * Consume characters from stdin up to and including reading a newline
 * or detecting end of file.
 */
void eat_up_to_nl() {
	int c;

	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}

void redirect_in(char *file, int *saved_fd) {
	//open file and check file descriptor(fd)
	int fd = open(file, O_RDONLY);
	if(fd == -1) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	//save the current fd and set it to the opened file using dup2
	*saved_fd = dup(STDIN_FILENO);
	int rc = dup2(fd, STDIN_FILENO);
	if(rc == -1) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}
	close(fd);
}

void reset_in(int *saved_fd) {
	if(*saved_fd == -1) {
		perror("Failed to reset file descriptor");
		exit(EXIT_FAILURE);
	}
	int current_fd = dup(STDIN_FILENO);
	dup2(*saved_fd, STDIN_FILENO);
	close(current_fd);
}

void reset_out(int *saved_fd) {
	if(*saved_fd == -1) {
		perror("Failed to reset file descriptor");
		exit(EXIT_FAILURE);
	}
	int current_fd = dup(STDOUT_FILENO);
	dup2(*saved_fd, STDOUT_FILENO);
	close(current_fd);
}

void redirect_out(char *file, int *saved_fd) {
	//open file and check file descriptor(fd)
	int fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd == -1) {
		perror("Failed to open file");
		exit(EXIT_FAILURE);
	}

	//save the current fd and set it to the opened file using dup2
	*saved_fd = dup(STDOUT_FILENO);
	int rc = dup2(fd, STDOUT_FILENO);
	if(rc == -1) {
		perror("Failed to duplicate file descriptor");
		exit(EXIT_FAILURE);
	}
	close(fd);
}

void setup_pipe(char *argv2[], int *saved_fd) {
	//create the pipe
	int fd[2];
    pipe(fd);

	//redirect the stdout to the right side of the pipe.
	*saved_fd = dup(STDOUT_FILENO);
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);

	//run to evaluate the right side. Reset stdout to original fd.
    run(&argv2, &saved_fd);
	reset_out(saved_fd);

	//redirect the stdin to the right side of the pipe.
	*saved_fd = dup(STDIN_FILENO);
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
}

void run(char * argv2[], int *saved_fd) {
	int status;
	pid_t pid = fork();
	// continue depending on which process is active.
	switch (pid) {
		case 0: /* Child. */
			execvp(argv2[0], argv2);
			
			perror("error executing"); /* Prints if the command does not exist. */
			if(errno == 2) { /* Command does not exist */
				exit(127);
			}
			else if(errno == 13) { /* Permission denied */
				exit(126);
			}
			else {
				exit(1);
			}
		case -1: /* Error. */
			perror("Fork failed");
			exit(EXIT_SUCCESS);
		default: /* Parent. */
			// If the second argument is "&" run in the background.
			// if(argv2[1] != NULL && strcmp(argv2[1], "&") == 0) {
			// 	continue;
			// }

			// After a process executes, print child exit code if exited.
			waitpid(pid, &status, 0);
			if(WIFEXITED(status)) {
				int child_exit_code = WEXITSTATUS(status);
				fprintf(stderr, "child exit code: %d\n", child_exit_code);
			}

			// Redirect input/output to original source.
			if(output_redir_count == 1) {
				reset_out(saved_fd);
			}
			else if(input_redir_count == 1) {
				reset_in(saved_fd);
			}
			else if(pipe_count == 1) {
				reset_in(saved_fd);
			}
	}
}

int
main (void)
{
	/* Be careful using these sized arrays! */
	char  command_line[CMD_MAX];
	char *argv2[ARG_MAX];
	int HISTORY_LEN = 16;
	char history[HISTORY_LEN][ARG_MAX];
	char next_line[ARG_MAX];
	int history_count = 0;
	int i;
	
	while (true) {
		int count;

		/* Prompt. */
		fprintf(stderr, "$ ");

		/* Read command lines from stdin. */
		fgets(command_line, CMD_MAX, stdin);
		strcpy(next_line, command_line);

		/* Terminate with exit code zero when pressing Ctrl-D  */
		if(feof(stdin)) {
			return 0;
		}

		/*
		* This is how you parse the command line into an
		* array of string pointers.
		*/
		count = parse(argv2, command_line);

		/* Check if there is enough space in the history array. If there are more 
		 than 16 strings, move each string back and index to make room at the 
		 front of the array. Add string from command_line to the history array */
		if(history_count >= HISTORY_LEN) {
			for( i = HISTORY_LEN - 1; i >= 0; i--) {
				strcpy(history[i], history[i - 1]);
			} 
			strcpy(history[0], next_line);
		} 
		/* Otherwise, move each string back one and add the new command to the front of the array */
		else {
			for(i = history_count; i >= 0; i--) {
				strcpy(history[i], history[i - 1]);
			}
			strcpy(history[0], next_line);
			//Increment counter
			history_count++;
		}

		/*
		* You need to properly process the command line; here we
		* merely print it as an example.
		*/
		if(argv2[0] != NULL) {
			int j;
			int saved_fd, should_continue = 1;
			input_redir_count = 0, output_redir_count = 0, pipe_count = 0;
			for(j = 0; j < count && should_continue == 1; j++) {
				if(strcmp(argv2[j], "<") == 0) {
					if(j == count - 1) {
						fprintf(stderr, "cannot use '<' without subsequent file\n");
						should_continue = 0;
					}
					else if(strcmp(argv2[j + 1], "<") == 0) {
						fprintf(stderr, "cannot use '<' twice\n");
						should_continue = 0;
					}
					else {
						input_redir_count++;
						redirect_in(argv2[j + 1], &saved_fd);
						argv2[j] = NULL;
					}
				}
				else if(strcmp(argv2[j], ">") == 0) {
					if(j == count - 1) {
						fprintf(stderr, "cannot use '>' without subsequent file\n");
						should_continue = 0;
					} 
					else if(strcmp(argv2[j + 1], ">") == 0) {
						fprintf(stderr, "cannot use '>' twice\n");
						should_continue = 0;
					}
					else {
						output_redir_count++;
						redirect_out(argv2[j + 1], &saved_fd);
						argv2[j] = NULL;
					}
				}
				else if(strcmp(argv2[j], "|") == 0) {
					if(j == count - 1) {
						fprintf(stderr, "cannot use '|' without subsequent command\n");
						should_continue = 0;
					} 
					else if(strcmp(argv2[j + 1], "|") == 0) {
						fprintf(stderr, "cannot use '|' twice\n");
						should_continue = 0;
					}
					else {
						pipe_count++;
						argv2[j] = NULL;
						setup_pipe(argv2, &saved_fd);
					}
				}
			}
			
			if (strcmp(argv2[0], "exit") == 0) {
				return 0;
			}
			else if(strcmp(argv2[0], "history") == 0) {
				for(i = 15; i >= 0; i --) {
					if((strcmp(history[i], "\n") != 0) || (strcmp(history[i], "") != 0)) {
						printf("%s", history[i]);
					}
				}
			}
			else if(strcmp(argv2[0], "&") == 0) {
				fprintf(stderr, "cannot use '&' without preceding command\n");
			}
			else if(should_continue) {
				run(argv2, &saved_fd);
			}
		}
	}
	exit(EXIT_SUCCESS);

done:
	exit(EXIT_FAILURE);
}

