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
				// execute cmds
				// TODO: must dynamically parse inputs
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