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
int parse(char *argv[], char *command_line)
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

/* 
 * Redirects the input from stdin to the given file and saves the old file
 * descriptor to saved_fd.
 */
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

/*
 * Resets stdin to the given file descriptor, which is ideally its original value.
 */
void reset_in(int *saved_fd) {
	//check that the given file descriptor is valid
	if(*saved_fd == -1) {
		perror("Failed to reset file descriptor");
		exit(EXIT_FAILURE);
	}

	//get the current fd for closing and change the fd to saved_fd
	int current_fd = dup(STDIN_FILENO);
	dup2(*saved_fd, STDIN_FILENO);
	close(current_fd);
}

/*
 * Resets stdout to the given file descriptor, which is ideally its original value.
 */
void reset_out(int *saved_fd) {
	//check that the given file descriptor is valid
	if(*saved_fd == -1) {
		perror("Failed to reset file descriptor");
		exit(EXIT_FAILURE);
	}

	//get the current fd for closing and change the fd to saved_fd
	int current_fd = dup(STDOUT_FILENO);
	dup2(*saved_fd, STDOUT_FILENO);
	close(current_fd);
}

/*
 * Redirects the output from stdout to the given file and saves the old file
 * descriptor to saved_fd.
 */ 
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

/*
 * Creates a pipe for one command to use the output of another as input. 
 */
void setup_pipe(char *argv2[], int *saved_fd) {
	//create the pipe
	int rc, fd[2] = {-1, -1};
    pipe(fd);

	//redirect the stdout to the right side of the pipe.
	*saved_fd = dup(STDOUT_FILENO);
	rc = dup2(fd[1], STDOUT_FILENO);
	if(rc == -1) {
		perror("Failed to create pipe");
		exit(EXIT_FAILURE);
	}
    close(fd[1]);

	//run to evaluate the right side. Reset stdout to original fd.
    run(&argv2, &saved_fd);
	reset_out(saved_fd);

	//redirect the stdin to the right side of the pipe.
	*saved_fd = dup(STDIN_FILENO);
    rc = dup2(fd[0], STDIN_FILENO);
	if(rc == -1) {
		perror("Failed to create pipe");
		exit(EXIT_FAILURE);
	}
    close(fd[0]);
}

void pipe2(char *p_args[], char *c_args[]) {
	int rc, pipefd[2] = {-1, -1}, in_fd, out_fd;
	pid_t pid;

	rc = pipe(pipefd);
	if(rc == -1) {
		perror("Failed to create pipe");
		exit(EXIT_FAILURE);
	}

	pid = fork();
	switch(pid) {
	case 0:
		/* Child */
		in_fd = dup(STDIN_FILENO);
		rc = dup2(pipefd[0], STDIN_FILENO);
		if(rc == -1) {
			perror("Child failed to dup");
			exit(EXIT_FAILURE);
		}

		close(pipefd[0]);
		close(pipefd[1]);

		//tr -d B
		execvp(c_args[0], c_args);

		// dup2(in_fd, STDIN_FILENO);
		perror("Failed to exec");
		exit(EXIT_FAILURE);
	case -1:
		perror("Failed to fork");
		exit(EXIT_FAILURE);
		break;
	default:
		/* Parent */
		out_fd = dup(STDOUT_FILENO);
		rc = dup2(pipefd[1], STDOUT_FILENO);
		if (rc == -1) {
			perror("Parent failed to dup");
			exit(EXIT_FAILURE);
		}

		close(pipefd[0]);
		close(pipefd[1]);

		printf("ABC");

		//echo ABC
		// execvp(p_args[0], p_args);

		dup2(out_fd, STDOUT_FILENO);
		break;
	}
}

/*
 * Responsible for forking and executing commands in the shell. 
 */
void run(char * argv2[], int *saved_fd) {
	int status;
	pid_t pid = fork();

	// continue depending on which process is active.
	switch (pid) {
		case 0: /* Child. */
			//execute given command and its arguments
			execvp(argv2[0], argv2);

			//print if an error occurs
			perror("error executing");
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

/*
 * Controls initial flow of logic and handles several user errors
 */
int main (void) {
	// Static Variabls:
	char  command_line[CMD_MAX];
	char *argv2[ARG_MAX];
	int HISTORY_LEN = 16;
	char history[HISTORY_LEN][ARG_MAX];
	char next_line[ARG_MAX];
	int history_count = 0;
	int i;
	
	// loop indefinately: 
	while (true) {
		int count;

		// display prompt
		fprintf(stderr, "$ ");

		// read command lines from stdin
		fgets(command_line, CMD_MAX, stdin);
		strcpy(next_line, command_line);

		// terminate with exit code zero when pressing Ctrl-D
		if(feof(stdin)) {
			return 0;
		}

		// parse the command line into an array of string pointers
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
		// Otherwise, move each string back one and add the new command to the front of the array.
		else {
			for(i = history_count; i >= 0; i--) {
				strcpy(history[i], history[i - 1]);
			}
			strcpy(history[0], next_line);
			history_count++;
		
		// Decide what to do with entered commands/arguments. 
		if(argv2[0] != NULL) {
			int j, saved_fd, should_continue = 1;
			input_redir_count = 0, output_redir_count = 0, pipe_count = 0;

			// Loop through the given args
			for(j = 0; j < count && should_continue == 1; j++) {
				if(strcmp(argv2[j], "<") == 0) {	/* input redirection */
					// handle user errors
					if(j == count - 1) {
						fprintf(stderr, "cannot use '<' without subsequent file\n");
						should_continue = 0;
					}
					else if(strcmp(argv2[j + 1], "<") == 0) {
						fprintf(stderr, "cannot use '<' twice\n");
						should_continue = 0;
					}
					else {
						// if used correctly, redirect the input. 
						input_redir_count = 1;
						redirect_in(argv2[j + 1], &saved_fd);
						argv2[j] = NULL;
					}
				}
				else if(strcmp(argv2[j], ">") == 0) {	/* output redirection */
					// handle user errors
					if(j == count - 1) {
						fprintf(stderr, "cannot use '>' without subsequent file\n");
						should_continue = 0;
					} 
					else if(strcmp(argv2[j + 1], ">") == 0) {
						fprintf(stderr, "cannot use '>' twice\n");
						should_continue = 0;
					}
					else {
						// if used correctly, redirect the output.
						output_redir_count++;
						redirect_out(argv2[j + 1], &saved_fd);
						argv2[j] = NULL;
					}
				}
				else if(strcmp(argv2[j], "|") == 0) {	/* piping */
					// handle user errors
					if(j == count - 1) {
						fprintf(stderr, "cannot use '|' without subsequent command\n");
						should_continue = 0;
					} 
					else if(strcmp(argv2[j + 1], "|") == 0) {
						fprintf(stderr, "cannot use '|' twice\n");
						should_continue = 0;
					}
					else {
						// if used correctly, setup the pipe.
						// pipe_count++;
						// argv2[j] = NULL;
						// setup_pipe(argv2, &saved_fd);

						char *parent_args[CMD_MAX], *child_args[CMD_MAX];
						int p_index, c_index;
						for(p_index = 0; p_index < j; p_index++) {
							parent_args[p_index] = argv2[p_index];
						}
						for(c_index = j + 1; c_index < count; c_index++) {
							child_args[c_index] = argv2[c_index];
						}
						pipe2(&parent_args, &child_args);
						should_continue = 0;
					}
				}
			}
			
			// if redirection or piping is not used, execute given command. 
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

}

