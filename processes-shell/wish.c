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

const char WHITESPACE[2] = " ";
const char PROMPT[6] = "wish> ";

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
// returns a list of args
// Given: "cmd -a -b -c"
// Returns: args = ["cmd", "a", "b", "c", NULL]
char** str_to_strList(char* arr[], char* str){
	char *token = strtok(str, WHITESPACE);
	int i = 0;
    while (token != NULL)
    {
		arr[i] = (char *) malloc((sizeof(char) * strlen(token)) + 1);
		if(!arr[i]){
			// TODO: call cleanup_list_alloc()
			perror("Unable to allocate heap space");
			exit(EXIT_FAILURE);
		}
        strcpy(arr[i++], token);
        token = strtok(NULL, WHITESPACE);
    }
	arr[i] = NULL;
	return arr;
}
// TODO: refactor to use strlen() 
void cleanup_list_alloc(char* arr[], int len){
	for(int i = 0; i < len; i++){
		free(arr[i]);
	}
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

// TODO: refactor to use strlen()
// TODO: refactor/remove this redundant function
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
			
			// Parsing stdin_line for cmd + args!
			size_t sz = index_of_char(stdin_line, *WHITESPACE) + 1;
			char cmd[sz];
			copy_up_to_delim(cmd, sz, stdin_line);
			strcat(binPath, cmd);
			
			if (access(binPath, F_OK) == 0){
				// determine executable permissions for binary
				if (access(binPath, X_OK) == 0){
					int len = cout_occurence(stdin_line, *WHITESPACE);

					if(len > 0){ // implies existence of args
						len+=2; // allocating space for ["cmd", ... , NULL]
						char* args[len];
						char buff[strlen(stdin_line) + 1];
					
						ptr_to_charArr(buff, stdin_line);
						str_to_strList(args, buff);

						execv(binPath, args);
						cleanup_list_alloc(args, len);
					}
					else{
						char *args[] = {" "}; // placeholder to satisfy compiler error
						execv(binPath, args);
						free(stdin_line);
					}
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
			printf("%s", PROMPT);
			fflush( stdout );
			// reset state in order for getline() to work on next iteration.
			free(stdin_line);
			stdin_line = NULL;
			stdin_len = 0;
		}
   }

}


//TEST MAIN
//int main(){
	// char* s = "cmd -a -b -c";
	// int len = cout_occurence(s, *WHITESPACE) + 2;
	// char buff[strlen(s) + 1];
	// char* arr[len];

	// ptr_to_charArr(buff, s);
	// str_to_strList(arr, buff);
	// for(int i = 0; i < len; i++){
	// 	printf("%s\n", arr[i]);
	// }
	// cleanup_list_alloc(arr, len);

	// ---------------------------
	// char* s = "ls -a -b -c";
	// int sz = index_of_char(s, *WHITESPACE) + 1;
	// printf("%i\n", sz);
    // char cmd[sz];
	// copy_up_to_delim(cmd, sz, s);
	// printf("%s\n", cmd);
	// printf("%li\n", strlen(cmd));
//}