CC=	gcc
LIBS=	-L/usr/X11R6/lib -lX11 -lXpm
MACROS=
CFLAGS=	-g -fstack-protector -ftrapv -funsafe-loop-optimizations -O0\
	-pedantic-errors -std=c89 -Waggregate-return -Wall\
	-Wbad-function-cast -Wcast-align -Wcast-qual -Wcomment\
	-Wconversion -Wdeclaration-after-statement\
	-Wdisabled-optimization -Wdouble-promotion -Werror -Wextra\
	-Wfloat-equal -Wformat=2 -Wframe-larger-than=512 -Winline\
	-Winit-self -Winvalid-pch -Wjump-misses-init -Wlarger-than=64\
	-Wlogical-op -Wlong-long -Wmissing-declarations\
	-Wmissing-format-attribute -Wmissing-include-dirs\
	-Wmissing-prototypes -Wnested-externs -Wold-style-declaration\
	-Wold-style-definition -Wpacked -Wredundant-decls -Wshadow\
	-Wstack-protector -Wstack-usage=512 -Wstrict-overflow=5\
	-Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wsync-nand\
	-Wtrampolines -Wundef -Wuninitialized -Wunreachable-code\
	-Wunsafe-loop-optimizations -Wunused\
	-Wvector-operation-performance -Wvla -Wwrite-strings

all: testleak

# Remove binary.
clean:
	rm -vf testleak

testleak:
	$(CC) $(LIBS) $(CFLAGS) testleak.c -o testleak

lint: testleak
	splint -checks -infloopsuncon -nullpass -retvalint -retvalother\
		-uniondef testleak

# G_SLICE=always-malloc G_DEBUG=gc-friendly are for glib.
valgrind: testleak
	valgrind -v --leak-check=full --num-callers=40 testleak
#	valgrind -v --leak-check=full --show-reachable=yes\
#		--track-origins=yes testleak

.PHONY: clean lint testleak valgrind

