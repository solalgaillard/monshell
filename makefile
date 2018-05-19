all : monshell text_entry

monshell : src/co-main.c src/cp-moncd.c src/cn-decouper.c src/redirection.c src/text_entry.c
	gcc -Wall src/co-main.c src/cp-moncd.c src/cn-decouper.c src/redirection.c src/text_entry.c -o src/monshell

text_entry : src/text_entry.c
	gcc -Wall src/text_entry.c -DTEST -o src/text_entry
