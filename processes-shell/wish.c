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


int cout_occurence(char * str, char c){
	int cout = 0;
    for(int i = 0; i < strlen(str); i++){
        if (str[i] == c) {
            cout++;
        }
    }
	return cout;
}

// returns a list of args
// Given: "cmd -a -b -c"
// Returns: args = ["cmd", "a", "b", "c", NULL]
char** str_to_strlist(char* str){
	int cout = cout_occurence(str, ' ') + 2; // for args[0] and args[len(args) - 1]! 
	char ** arr = (char**) malloc(cout * sizeof(char*)); // 8 bytes per char* in arr 
	if(arr == NULL){
		perror("Unable to allocate heap space");
		exit(EXIT_FAILURE);
	}
	char *token = strtok(str, " ");
	int i = 0;
    while (token != NULL)
    {
		//TODO: may have to free(arr[i]) then free(arr).
		// in other words, may have to free the 'nested' pointers then the 'main' pointer
		arr[i] = malloc(sizeof(char) * strlen(token) + 1);
        strcpy(arr[i++], token);
        token = strtok(NULL, " ");
    }
	arr[i] = NULL;
	return arr;
}

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
		stdin_line[strlen(stdin_line) - 1] = '\0';

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
			
			if (access(binPath, F_OK) == 0){
				// determine executable permissions for binary
				if (access(binPath, X_OK) == 0){
					//char* args[] = {"ls", "-l", "--author", NULL};
					char* args[] = str_to_strlist(stdin_line);
					execv(binPath, args);
					free(args);
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

// // TEST MAIN
// int main(){
// 	char** args = str_to_strlist("cmd -a -b -c");
// }