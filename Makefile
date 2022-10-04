# First target is default target, if you just type:  make

CC = gcc
CFLAGS = -g3 -O0 -std=c11

default: check

check: shki
	./$<

shki: shki.c
	${CC} $< ${CFLAGS} -o $@

clean:
	rm -f shki

# 'make' views $v as a make variable and expands $v into the value of v.
# By typing $$, make will reduce it to a single '$' and pass it to the shell.
# The shell will view $dir as a shell variable and expand it.
dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
	dir=`basename $$PWD`; ls -l ../$$dir.tar.gz
