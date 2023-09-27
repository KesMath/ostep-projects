#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

// ========= FEATURES TO IMPLEMENT =========
// -(1) support built-ins (path)
// -(2) support batch-mode
// -(3) support redirections from std_out -> user file

// -(4) support parallel cmds
// -(5) support add error msg per README

// ========= TECHNICAL DEBT =========
// support null std_line

const char WHITESPACE_DELIMITER[2] = " ";
const char PROMPT[6] = "wish> ";
const int ERRONEOUS_CMD = 3;
const int USER_EXIT = 4;
const int CHDIR_EXIT = 5;
const char *BUILT_IN_CMDS[] = { "cd", "exit", "path" };

int cout_occurence(char * str, char c){
	int cout = 0;
    for(int i = 0; i < strlen(str); i++){
        if (str[i] == c) {
            cout++;
        }
    }
	return cout;
}

char* ptr_to_charArr(char dest[], char* src){
	int i = 0;
	for(; i < strlen(src); i++){
		dest[i] = src[i]; 
	}
	dest[i] = '\0';
	return dest;
}

// TODO: refactor to use arrlen() 
void cleanup_list_alloc(char* arr[], int len){
	for(int i = 0; i < len; i++){
		//printf("Freeing %p\n", arr[i]);
		free(arr[i]);
	}
}
// returns a list of args
// Given: "cmd -a -b -c"
// Returns: args = ["cmd", "a", "b", "c", NULL]
char** str_to_strList(char* arr[], char* str){
	char *token = strtok(str, WHITESPACE_DELIMITER);
	int i = 0;
    while (token != NULL)
    {
		arr[i] = (char *) malloc((sizeof(char) * strlen(token)) + 1);
		if(!arr[i]){
			cleanup_list_alloc(arr, i); // counter only increases per successful alloc. Thus, we only cleanup 'counter' times
			perror("Unable to allocate heap space");
			exit(EXIT_FAILURE);
		}
        strcpy(arr[i++], token);
        token = strtok(NULL, WHITESPACE_DELIMITER);
    }
	arr[i] = NULL;
	return arr;
}

int index_of_char(char* src, char delim){
	int i = 0;
	for(; i < strlen(src); i++){
		if(src[i] == delim){
			return i;
		}
	}
	// assuming cmd is without args, we want to capture the entire string
	return strlen(src);
}

char* copy_up_to_delim(char dest[], size_t index, char* src){
	for(int i = 0; i < index; i++){
		dest[i] = src[i];
	}
	dest[index - 1] = '\0';
	return dest;
}

int main(int argc, char* argv[])
{
    char * stdin_line = NULL;
    size_t stdin_len = 0;
	size_t chars_read;
	char *PATH = "/bin/";

    printf("%s", PROMPT);
    fflush( stdout );
    while (1){
      // obtain user cmd
		chars_read = getline(&stdin_line, &stdin_len, stdin);
		stdin_line[strlen(stdin_line) - 1] = '\0';

		if(chars_read == 1 && stdin_line[0] == '\0'){
			free(stdin_line);
			exit(ERRONEOUS_CMD);
		}
		char binPath[chars_read + strlen(PATH) + 1];
		strcpy(binPath, PATH);
		
		// Parsing stdin_line for cmd + args!
		size_t sz = index_of_char(stdin_line, *WHITESPACE_DELIMITER) + 1;
		char cmd[sz];
		copy_up_to_delim(cmd, sz, stdin_line);
		strcat(binPath, cmd);
		int len = cout_occurence(stdin_line, *WHITESPACE_DELIMITER);
		len+=2; // allocating space for ["cmd", ... , NULL]
		char* args[len];
		char buff[strlen(stdin_line) + 1];
	
		ptr_to_charArr(buff, stdin_line);
		str_to_strList(args, buff);

		int rc = fork();
		if (rc < 0){
			fprintf(stderr, "fork failed\n");
		}
		else if (rc == 0){ // CHILD PROCESS CODE BLOCK
			
			if(strcasecmp(BUILT_IN_CMDS[0], args[0]) == 0){ // stdin_line is 'cd'
				// NOTE: when a child process calls chdir(), the effects are ephemeral
				// and only persists within the scope of this process. To have chdir() persist
				// for subsequent cmds, we delegate this task to the parent so iterative calls
				// to fork() copies processes' memory image onto newer children.
				exit(CHDIR_EXIT);

			}
			else if (strcasecmp(BUILT_IN_CMDS[1], args[0]) == 0){ // stdin_line is 'exit'
				exit(USER_EXIT);
			}
			else if (strcasecmp(BUILT_IN_CMDS[2], args[0]) == 0){ // stdin_line is 'path'
				// create new global char* [] BINPATHS = ["bin/",]
				// append args to this new list and in else statement, wrap logic in a for-loop
				// where each user cmd is concatenated to BINPATHS[i] and ran (assuming if file exists and is executable permissions)
				return 3;
			}
			else{ // non-builtin cmd
				if (access(binPath, F_OK) == 0){
					// determine executable permissions for binary
					if (access(binPath, X_OK) == 0){
						// NOTE: no need to call cleanup_list_alloc()
						// since this execv() overwrites both heap and stack space of child process
						// thereby any allocation is indirectly freed for free!
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
			// When child process exits (in this case due to erroneous cmd), we need to use a different error code
			// and be cautious to avoid using the one's specified below as they are conventionally reserved for special meanings
			// https://tldp.org/LDP/abs/html/exitcodes.html
			exit(ERRONEOUS_CMD);
		}
		else{ // PARENT PROCESS CODE BLOCK

			// https://linux.die.net/man/2/exit
			// The value status is returned to the parent process as the process's exit status, and can be collected using one of the wait(2) family of calls.
			// SYNTAX GUIDE: https://www.geeksforgeeks.org/exit-status-child-process-linux/ 
			int status;
			wait(&status);
			if(WIFEXITED(status)){
				if (WEXITSTATUS(status) == USER_EXIT){
					exit(0);
				}
				else if(WEXITSTATUS(status) == CHDIR_EXIT){
					if(args[1] == NULL){
						perror("An error has occurred\n");
					}
					else{
						int ret_code = chdir(args[1]);
						if(ret_code == -1){
							perror("Unable to change to directory.\n");
						}
					}
				}
			}
			printf("%s", PROMPT);
			fflush( stdout );
			// reset state in order for getline() to work on next iteration.
			free(stdin_line);
			cleanup_list_alloc(args, len);
			stdin_line = NULL;
			stdin_len = 0;
		}
   }
}


//TEST MAIN
//int main(){
	// char* s = "cmd -a -b -c";
	// int len = cout_occurence(s, *WHITESPACE_DELIMITER) + 2;
	// char buff[strlen(s) + 1];
	// char* arr[len];

	// ptr_to_charArr(buff, s);
	// str_to_strList(arr, buff);
	// for(int i = 0; i < len; i++){
	// 	printf("%s\n", arr[i]);
	// }
	// cleanup_list_alloc(arr, len);

	// // ---------------------------
	// char* s = "ls -a -b -c";
	// int sz = index_of_char(s, *WHITESPACE_DELIMITER) + 1;
	// printf("%i\n", sz);
    // char cmd[sz];
	// copy_up_to_delim(cmd, sz, s);
	// printf("%s\n", cmd);
	// printf("%li\n", strlen(cmd));
//}