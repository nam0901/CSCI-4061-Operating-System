all: shell.c commands.c directory.c util.c grep.c du.c
	gcc shell.c commands.c directory.c util.c -o shell
	gcc grep.c util.c -o grep
	gcc du.c -o du

shell: shell.c commands.c directory.c util.c
	gcc shell.c commands.c directory.c util.c -o shell

grep: grep.c
	gcc grep.c util.c -o grep

du: du.c
	gcc du.c -o du

clean:
	rm -f *.o shell grep du
