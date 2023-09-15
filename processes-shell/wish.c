#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

struct user_input {
    char *stdin_line;
    size_t stdin_len;
    } *input;

int main(int argc, char* argv[])
{
    input->stdin_line = NULL;
    input->stdin_len = 0;
	 char *PATH = "/bin/";

    printf("wish>");
    fflush( stdout );
    while (1){
      // obtain user cmd
		getline(input->stdin_line, input->stdin_len, stdin);

		// there's no need to process cmds if input is empty
		if (input->stdin_len > 0){
			int rc = fork();
			if (rc < 0){
				fprintf(stderr, "fork failed\n");
			}
			else if (rc == 0){
				char binPath[input->stdin_len + strlen(PATH)];
				strcat(binPath, PATH);
				strcat(binPath, input->stdin_line);
			 
				// determine if binary exists
				if (access(binPath, F_OK) == 0){

					// determine executable permissions for binary
					if (access(binPath, X_OK) == 0){
						// TODO: must parse input for args
						execv(binPath, " ");
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
			}
		}
   }

}