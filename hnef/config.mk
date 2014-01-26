# Tune these options to your system.

########################################################################
# Info.
########################################################################
NAME=                hnefatafl
VERSION=        131214

########################################################################
# The language that you want the program in. You may have to `clean`
# after changing this for the language source file to be updated.
#
# eng        English
# swe        Svenska
#
# The appropriate `srclang/lang_XYZ.c` file must exist, where XYZ is an
# ISO 639-2 (three-letter) language code.
#
# If you want to translate, do NOT modify `src/lang.c` since that file
# is automatically generated and deleted.
########################################################################
LANGUAGE=        eng

########################################################################
# Install files.
#
# MAN_CAT        Manual page category.
# FILE_RC        Name of default RC file in $(HOME)/$(FILE_RC).
# FILE_RULE_DEF        Name of a ruleset file in $(PREFIX_DATA_RULES).
#                $(FILE_RC) will be symlink to this file.
########################################################################
MAN_CAT=        6
FILE_RC=        .$(NAME)rc
FILE_RULE_DEF=        hnefatafl11_v1

########################################################################
# Paths.
#
# PREFIX                        Base install path.
# PREFIX_BIN                        Install path for binary.
# PREFIX_SHARE                        Install path for shared files.
# PREFIX_SHARE_MAN                Install path for manual files.
# PREFIX_DATA                        Install path for shared data files.
# PREFIX_DATA_RULES                Install path for ruleset data files.
# PREFIX_DATA_RULES_CONF        The PREFIX_DATA_RULES that will be used
#                                in the manual page and ruleset files.
# PREFIX_X11_LIB                Location of XLib and XPM library.
#
# PREFIX_DATA_RULES_CONF is separate to make it easier to package the
# program, when you need to install to a temporary directory during
# packaging but have the manual page and ruleset file paths point to the
# final install directory. If you just use `make install`, then this
# should (must) be `$(PREFIX_DATA_RULES)`.
#
# PREFIX_X11_LIB is where XLib and the XPM library are:
# Slackware 14.0        /usr/X11R6/lib
# Debian 7.0                /usr/include/X11
########################################################################
PREFIX=                        /usr/local
PREFIX_BIN=                $(PREFIX)/games
PREFIX_SHARE=                $(PREFIX)/share/games
PREFIX_SHARE_MAN=        $(PREFIX)/man/man$(MAN_CAT)
PREFIX_DATA=                $(PREFIX_SHARE)/$(NAME)
PREFIX_DATA_RULES=        $(PREFIX_DATA)/rules
PREFIX_DATA_RULES_CONF=        $(PREFIX_DATA_RULES)
PREFIX_X11_LIB=                /usr/X11R6/lib

########################################################################
# Preprocessor macros for determining what to compile.
#
# HT_C99                        C99 functions.
# HT_POSIX                        POSIX functions.
# HT_UNSAFE                        General micro-optimizations.
# HT_AI_MINIMAX                        Computer player.
# HT_AI_MINIMAX_UNSAFE                AI micro-optimizations.
#                                Depends: HT_AI_MINIMAX.
# HT_AI_MINIMAX_HASH                AI transposition table (hashing).
#                                Depends: HT_AI_MINIMAX.
# HT_AI_MINIMAX_HASH_MEM_TAB        AI hash memory for hash table.
#                                Depends: HT_AI_MINIMAX_HASH.
# HT_AI_MINIMAX_HASH_MEM_COL        AI hash memory for collision lists.
#                                Depends: HT_AI_MINIMAX_HASH.
# HT_UI_CLI                        Command line interface.
# HT_UI_XLIB                        XLib interface.
#                                Depends: LIB_XLIB.
########################################################################
MACRO_C99=                        -DHT_C99
MACRO_POSIX=                        -DHT_POSIX
#MACRO_UNSAFE=                        -DHT_UNSAFE
MACRO_AI_MINIMAX=                -DHT_AI_MINIMAX
#MACRO_AI_MINIMAX_UNSAFE=        -DHT_AI_MINIMAX_UNSAFE
MACRO_AIM_HASH=                        -DHT_AI_MINIMAX_HASH
MACRO_AIM_HASH_MEM_TAB=                -DHT_AI_MINIMAX_HASH_MEM_TAB=32000000
MACRO_AIM_HASH_MEM_COL=                -DHT_AI_MINIMAX_HASH_MEM_COL=48000000
MACRO_UI_CLI=                        -DHT_UI_CLI
MACRO_UI_XLIB=                        -DHT_UI_XLIB
MACRO_UI_MSQU=                        -DHT_UI_MSQU
MACROS=                                -DHT_VERSION=$(VERSION)\
                                -DHT_FILE_RC_DEF=\"$(FILE_RC)\"\
-DHT_FILE_RULE_DEF=\"$(PREFIX_DATA_RULES_CONF)/$(FILE_RULE_DEF)\"\
                                $(MACRO_POSIX)\
                                $(MACRO_UNSAFE) $(MACRO_AI_MINIMAX)\
                                $(MACRO_AI_MINIMAX_UNSAFE)\
                                $(MACRO_UI_MSQU)
                                #$(MACRO_UI_CLI) $(MACRO_UI_XLIB) 
                                

########################################################################
# Libraries.
#
# LIB_C                C library.
# LIB_POSIX        POSIX C library.
# LIB_XLIB        XLib and XPM library
#                Depends: HT_UI_XLIB.
#
# -lm is the math library.
#
# LIB_XLIB should be commented out if you don't use HT_UI_XLIB (the XLib
# interface).
########################################################################
LIB_C=                -lm
LIB_XLIB=        -L$(PREFIX_X11_LIB) -lX11 -lXpm
LIBS=                $(LIB_C) $(LIB_XLIB) -lpthread

########################################################################
# Compiler options.
#
# CFLAGS_GEN                General options.
# CFLAGS_MORE                Optional extra options. Can all be removed.
# CFLAGS_OPT_ARC        Machine-dependent options. Can all be removed.
# CFLAGS_OPT_LVL        Optimization level. Can all be removed.
# CFLAGS_OPT                Optimization options. Can all be removed.
# CFLAGS_WARN                Warning options. Can all be removed.
#
# Assumes use of -O3 or -Ofast in GCC, and the options implied by them.
# The following optimization options are not enabled:
# -fmudflap: nice feature, but the program is checked with Valgrind
# (www.valgrind.org).
#
# The following warning options are not enabled:
# -Wframe-larger-than -Wlarger -Wstack-usage are arbitrarily chosen.
# -Wnormalized: ISO C -- this is C89.
# -Wpadded: irresolvable warnings (all warnings are errors).
# -Wsuggest-attribute: attributes are not portable.
# -Wsystem-headers -Wunknown-pragmas: causes errors from system headers.
# -Wtraditional -Wtraditional-conversion: old-style -- this is C89.
# -Wunsuffixed-float-constants: forbids "0.0" and therefore makes it
# impossible to use doubles, since the "d" suffix is a GCC extension.
#
# -Wstack-usage changed from 128 to 256 for O3.
# -Wframe-larger-than changed from 128 to 256 for O3.
#
# When not using HT_C99 or HT_POSIX, you can compile with -std=c89.
# When not using HT_POSIX, you can compile with -std=c99.
# When using HT_C99 and HT_POSIX, you should compile with std=gnu99, or
# it will not find `symlink` in `unistd.h`.
########################################################################
CFLAGS_GEN=        -g -std=gnu99
CFLAGS_MORE=
CFLAGS_OPT_ARC=        -march=native -mtune=native
CFLAGS_OPT_LVL=        -O3
CFLAGS_OPT=        $(CFLAGS_OPT_ARC) $(CFLAGS_OPT_LVL)\
                -fmerge-all-constants -fstrict-aliasing\
                -fstrict-overflow -ftree-loop-ivcanon -funroll-loops\
                -funsafe-loop-optimizations -funswitch-loops -fwrapv
CFLAGS_WARN=        -pedantic-errors -Waggregate-return -Wall\
                -Wbad-function-cast -Wcast-align -Wcast-qual -Wcomment\
                -Wconversion -Wdeclaration-after-statement\
                -Wdisabled-optimization -Wdouble-promotion -Werror\
                -Wextra -Wfloat-equal -Wformat=2\
                -Wframe-larger-than=256 -Winline -Winit-self\
                -Winvalid-pch -Wjump-misses-init -Wlarger-than=64\
                -Wlogical-op -Wlong-long -Wmissing-declarations\
                -Wmissing-format-attribute -Wmissing-include-dirs\
                -Wmissing-prototypes -Wnested-externs\
                -Wold-style-declaration -Wold-style-definition -Wpacked\
                -Wredundant-decls -Wshadow -Wstack-protector\
                -Wstack-usage=256 -Wstrict-aliasing -Wstrict-overflow=5\
                -Wstrict-prototypes -Wswitch-default -Wswitch-enum\
                -Wsync-nand -Wtrampolines -Wundef -Wuninitialized\
                -Wunreachable-code -Wunsafe-loop-optimizations -Wunused\
                -Wvector-operation-performance -Wvla -Wwrite-strings
CFLAGS=                $(CFLAGS_GEN) $(CFLAGS_OPT) $(CFLAGS_MORE) \
#                $(CFLAGS_WARN)

########################################################################
# Compiler and shell.
########################################################################
CC=                gcc
