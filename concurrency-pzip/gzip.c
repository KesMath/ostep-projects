#include <stdio.h>
#include <string.h>
// STEPS:
// (1) - Read in File as a string, S
// (2) - Pass S into a function that returns number of unique chars, K
// (3) - create an empty list of integers where we malloc(sizeof(int) * K). Call this L
// (4) - pass S into a function where we append the index of a character transition into L
// (5) - create K threads. Each thread, will be assigned to a slice of L called [a,b], where b > a
// (6) - each thread, in parallel, will compute diff(b,a) and return the integer as a string, along with S[b]
// (7) - For threads T1 -> TN, write to STDOUT in proper ordering

// EG: S = [aaaabbbcc]
// Number of unique chars = 3
// L = malloc(sizeof(int) * 3)
// L = [3, 6, 8], thread cout = 3
// T1 = [0,3], T2 = [3,6], T3 = [6,8]
// in parallel, each thread will compute their diffs = {4,3,2} along with S[b] = {a,b,c} thus
// Unordered_Result = 2c4a3b, as order output is not guaranteed with threads
// Hence for every S[b], we get index(S[b]), and write {Unordered_Result[index(S[b]) - 1], S[b]} to ensure proper ordering

int cout_unique_chars(char *s){
    if (strlen(s) == 0){return 0;}
    else{
        int cout = 1;
        for(int i = 0; i < strlen(s) - 1; i++){
            if(s[i] != s[i+1]){
                cout++;
            }
    }
    return cout;
    }
}

// int** get_transitional_indicies(char *s){
  
// }

int main(int argc, char* argv[])
{
	// if (argc == 2){
	// 	FILE *file = fopen(argv[1], "r");
    // 	if(file == NULL){
    //     	printf("Unable to open file: %s\n", argv[1]);
    //     	exit(-1);
    // 	}
    //     else{

    //     }
	// }
    // else{
    //     printf("File path not supplied. Exiting...");
    //     	exit(-1);
    // }
    printf("%i\n", cout_unique_chars(""));
    printf("%i\n", cout_unique_chars("abcdefghijklmnopqrstuvqxyz"));
    printf("%i\n", cout_unique_chars("aaaabbbcc"));
}