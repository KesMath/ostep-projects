# COMPILED ON: x86_64 GNU/Linux

SANITIZER_FLAGS = -fsanitize=undefined -fsanitize=bounds -fsanitize=address -fsanitize=leak


gzip:
	gcc -std=c11 -Wall -Werror -Wshadow ${SANITIZER_FLAGS} -g -O3 gzip.c -o gzip