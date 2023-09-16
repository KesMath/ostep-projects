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

int main(int argc, char* argv[])
{
    char * stdin_line = NULL;
    size_t stdin_len = 0;
	size_t chars_read;
	char *PATH = "/bin/";

    printf("wish> ");
    fflush( stdout );
    while (1){
      // obtain user cmd
		chars_read = getline(&stdin_line, &stdin_len, stdin);

		if(chars_read < 0){
			perror("Unable to read user input");
			free(stdin_line);
			exit(1);
		}

		int rc = fork();
		if (rc < 0){
			fprintf(stderr, "fork failed\n");
		}
		else if (rc == 0){
			//printf("Line: %s\n", stdin_line);
			//printf("Read: %li\n", chars_read);
			//printf("path: %li\n", strlen(PATH));

			char binPath[chars_read + strlen(PATH) + 1];
			strcpy(binPath, PATH);
			// TODO: must parse stdin_line for cmd + args!
			strcat(binPath, stdin_line);
			printf("BIN PATH:%s\n", binPath); 
			
			// determine if binary exists
			// TODO: create dircnt() to count number of directories in a path. 
			// Then use that number to generate parent directory prefix string (i.e. "../") dircnt() times!
			char *binPath1 = "../../../../bin/ls"; 
			printf("F access: %i\n", access(binPath1, F_OK));
			if (access(binPath1, F_OK) == 0){
				// determine executable permissions for binary
				if (access(binPath1, X_OK) == 0){
					// TODO: must parse input for args
					char *args[] = {"-all"};
					execv(binPath1, args);
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
			printf("wish> ");
			fflush( stdout );
			// reset state in order for getline() to work on next iteration.
			free(stdin_line);
			stdin_line = NULL;
			stdin_len = 0;
		}
   }

}
