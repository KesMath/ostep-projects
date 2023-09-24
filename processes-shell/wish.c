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

int cout_occurence(char * str, char c){
	int cout = 0;
    for(int i = 0; i < strlen(str); i++){
        if (str[i] == c) {
            cout++;
        }
    }
	return cout;
}

char* ptr_to_strArr(char dest[], char* src){
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
char** str_to_strlist(char* arr[], char* str){
	//int cout = cout_occurence(str, *WHITESPACE) + 2; // for args[0] and args[len(args) - 1]! 
	char *token = strtok(str, WHITESPACE);
	int i = 0;
    while (token != NULL)
    {
		//TODO: may have to free(arr[i]) then free(arr).
		// in other words, may have to free the 'nested' pointers then the 'main' pointer
		arr[i] = (char *) malloc((sizeof(char) * strlen(token)) + 1);
		if(!arr[i]){
			perror("Unable to allocate heap space");
			exit(EXIT_FAILURE);
		}
        strcpy(arr[i++], token);
        token = strtok(NULL, WHITESPACE);
    }
	arr[i] = NULL;
	return arr;
}

// int main(int argc, char* argv[])
// {
//     char * stdin_line = NULL;
//     size_t stdin_len = 0;
// 	size_t chars_read;
// 	char *PATH = "/bin/";

//     printf("wish> ");
//     fflush( stdout );
//     while (1){
//       // obtain user cmd
// 		chars_read = getline(&stdin_line, &stdin_len, stdin);
// 		stdin_line[strlen(stdin_line) - 1] = '\0';

// 		if(chars_read < 0){
// 			perror("Unable to read user input");
// 			free(stdin_line);
// 			exit(1);
// 		}

// 		int rc = fork();
// 		if (rc < 0){
// 			fprintf(stderr, "fork failed\n");
// 		}
// 		else if (rc == 0){
// 			//printf("Line: %s\n", stdin_line);
// 			//printf("Read: %li\n", chars_read);
// 			//printf("path: %li\n", strlen(PATH));

// 			char binPath[chars_read + strlen(PATH) + 1];
// 			strcpy(binPath, PATH);
// 			// TODO: must parse stdin_line for cmd + args!
// 			strcat(binPath, stdin_line); 
			
// 			if (access(binPath, F_OK) == 0){
// 				// determine executable permissions for binary
// 				if (access(binPath, X_OK) == 0){
// 					//char arr[cout_occurence(stdin_line, *WHITESPACE)];
// 					//char** args = str_to_strlist(arr, stdin_line);
// 					char* args[4];
// 					args[0] = "ls";
// 					args[1] = "-l";
// 					args[2] = "--author";
// 					args[3] = NULL;
// 					execv(binPath, args);
// 					//free(args);
// 				}
// 				else{
// 					perror("Unable to execute binary\n");
// 				}
// 			}
// 			else{
// 				perror("Command not found\n");
// 			}
// 		}
// 		else{
// 			// wait for child to finish and print shell prompt again
// 			wait(NULL);
// 			printf("wish> ");
// 			fflush( stdout );
// 			// reset state in order for getline() to work on next iteration.
// 			free(stdin_line);
// 			stdin_line = NULL;
// 			stdin_len = 0;
// 		}
//    }

// }


//TEST MAIN
int main(){
	char* s = "cmd -a -b -c";

	char buff[strlen(s) + 1];
	char* arr[cout_occurence(s, *WHITESPACE)];
	
	ptr_to_strArr(buff, s);
	str_to_strlist(arr, buff);
}