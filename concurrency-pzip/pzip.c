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
// in parallel, each thread will compute their diffs = {4,3,2} along with S[b] = {a,b,c} thus 4a3b2c