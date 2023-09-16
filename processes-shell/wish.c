#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>


// TODO LIST
// -support batch-mode
// -support built-ins
// -support redirections from std_out -> user file
// -support parallel cmds

// -support add error msg per README

struct user_input {
    char **stdin_line;
    size_t *stdin_len;
    } *input;

int main(int argc, char* argv[])
{
    input->stdin_line = NULL;
    input->stdin_len = 0;
	 ssize_t chars_read;
	 char *PATH = "/bin/";

    printf("wish>");
    fflush( stdout );
    while (1){
      // obtain user cmd
		chars_read = getline(input->stdin_line, input->stdin_len, stdin);

		if(chars_read < 0){
			perror("Unable to read user input");
			free(input->stdin_line);
			exit(1);
		}

		int rc = fork();
		if (rc < 0){
			fprintf(stderr, "fork failed\n");
		}
		else if (rc == 0){
			char binPath[chars_read + strlen(PATH)];
			strcat(binPath, PATH);
			// TODO: must parse input->stdin_line for cmd + args!
			strcat(binPath, *input->stdin_line); 
			
			// determine if binary exists
			if (access(binPath, F_OK) == 0){

				// determine executable permissions for binary
				if (access(binPath, X_OK) == 0){
					// TODO: must parse input for args
					char *args[] = {NULL};
					execv(binPath, args);
				}
				else{
					perror("Unable to execute binary\n");
				}
			}
			else{
				perror("Command not found\n");
			}
		}
		else{
			// wait for child to finish and print shell prompt again
			wait(NULL);
			printf("wish>");
			fflush( stdout );
			// reset state in order for getline() to work on next iteration.
			free(input->stdin_line);
			input->stdin_line = NULL;
			input->stdin_len = 0;
		}
   }

}