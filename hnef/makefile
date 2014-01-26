# Configure the makefile by editing `config.mk`.

########################################################################
# Dependencies (other than GNU Make):
#	`dirname`
#	`find`
#	`rm`
#	`cp`
#	`test`
#	`mkdir`		(`mkdir -p` required only for `install`)
#	`cd`		(only for `dist`)
#	`bzip2`		(only for `dist`)
#	`mv`		(only for `dist`)
#	`tar`		(only for `dist`)
#	`gzip`		(only for `install`)
#	`cat`		(only for `install`, `man`, `mantxt`, `manhtml`)
#	`awk`		(only for `install`, `man`, `mantxt`, `manhtml`)
#	`echo`		(only for `install` and `uninstall`)
#	`col`		(only for `mantxt`)
#	`groff`		(only for `mantxt`)
#	`man2html`	(only for `manhtml`)
#	`valgrind`	(only for `testleak`)
#	`cppcheck`	(only for `testlint`)
#	`splint`	(only for `testlint`)
########################################################################

########################################################################
# `make` / `all`
#	Builds object files from `src/` (and its sub-directories,
#	recursively) to `obj/` and links them to an executable in
#	`bin/`.
#
# `clean`
#	Removes `obj/` recursively.
#
# `mrproper`
#	Runs all `clean*` procedures.
#
# `cleanbin`
#	Removes the binary created by `make`.
#
# `cleandist`
#	Removes the tarball created by `dist`.
#
# `cleanman`
#	Removes the final manual files that were created by `man`.
#
# `cleanmanhtml`
#	Removes the HTML manual files that were created by `manhtml`.
#
# `cleanmantxt`
#	Removes the TXT manual files that were created by `mantxt`.
#
# `dist`
#	Tars everything (except temporary files, plain text manual pages
#	and the binary) up. Assumes that we're allowed to `cd` into the
#	parent directory.
#
# `install`
#	Installs the program and all data files.
#	This is interactive if the files already exist and are to be
#	overwritten. Else it does not require user interaction.
#
# `man`
#	Makes final versions of manual pages in `man/`. These get the
#	suffix `.final` (id est: `manpage.1` -> `manpage.1.final`). The
#	purpose of this rule is to "finalize" all manual pages (of all
#	languages), which differs from `install` which only finalizes
#	the manual page indicated by `${LANGUAGE}`.
#
# `manhtml`
#	Creates HTML manual files from the groff files in `man/`.
#
# `mantxt`
#	Creates plain text manual files from the groff files in `man/`.
#
# `sloc`
#	Counts newlines and `;` characters in source files. This may not
#	include `src/lang.c`, but it's a very rough measurement anyway.
#
# `testall`
#	Runs all `test` procedures.
#
# `testleak`
#	Checks the program with Valgrind.
#
# `testlint`
#	Checks the program with cppcheck and splint.
#
# `teststyle`
#	Style checks on source and header files.
#
# `uninstall`
#	Removes the files installed by `install` (though it does not
#	remove the PREFIX directories, even if they were created).
#	This is always interactive (unless there is nothing to remove).
########################################################################

include config.mk

DIR_CUR=	$(shell basename `pwd`)
TARB=		$(NAME)$(VERSION)

DIR_BIN=	bin
DIR_MAN=	man
DIR_OBJ=	obj
DIR_SRC=	src
DIR_SRCLANG=	srclang
DIR_RULES=	rules

FILE_LANG=	$(DIR_SRC)/lang.c
FILES_RULES=	$(subst $(DIR_RULES), $(PREFIX_DATA_RULES), \
			$(shell find $(DIR_RULES) -maxdepth 1 -type f))

BIN=		$(DIR_BIN)/$(NAME)
DIRS_SRC=	$(shell find $(DIR_SRC) -type d)
DIRS_OBJ=	$(subst $(DIR_SRC), $(DIR_OBJ), $(DIRS_SRC))
MANS=		$(shell find $(DIR_MAN) -name "*.$(MAN_CAT)")
MANTXTS=	$(MANS:.$(MAN_CAT)=.txt)
MANHTMLS=	$(MANS:.$(MAN_CAT)=.html)
MANGROFFS=	$(MANS:.$(MAN_CAT)=.$(MAN_CAT).final)
SRCS=		$(foreach SDIR, $(DIRS_SRC), $(wildcard $(SDIR)/*.c))
OBJS=		$(subst $(DIR_SRC), $(DIR_OBJ), $(SRCS:.c=.o))
DEPS=		$(OBJS:.o=.d)
INC=		$(foreach SDIR, $(DIRS_SRC), $(wildcard $(SDIR)/*.h))
INC_DIRS=	$(patsubst %, -I%, $(DIRS_SRC))

GROFF=		groff -D utf-8 -mandoc -Tutf8
COL=		col -bx
MAN_AWK=	awk " { gsub(\"GROFFHT_PATH_DATA_RULES\", \"$(PREFIX_DATA_RULES_CONF)\"); gsub(\"GROFFHT_PATH_RC\", \"\\\$${HOME}/$(FILE_RC)\"); gsub(\"GROFFHT_VERSION\", \"$(VERSION)\"); print; } "

all: $(FILE_LANG)
	make $(BIN)

$(FILE_LANG):
	cp -v $(DIR_SRCLANG)/lang_$(LANGUAGE).c $(FILE_LANG)

$(BIN): $(OBJS)
	$(CC) $(LIBS) $(CFLAGS) $(OBJS) -o $(BIN)

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c
	test -d $(shell dirname $@) || mkdir -v $(shell dirname $@)
	$(CC) $(MACROS) $(CFLAGS) $(INC_DIRS) -MMD -MP -MF $(@:.o=.d) \
		-c $< -o $@

clean:
	rm -rvf $(FILE_LANG) $(DIR_OBJ)

mrproper: clean cleanbin cleandist cleanman cleanmanhtml cleanmantxt

cleanbin:
	rm -vf $(BIN)

cleandist:
	rm -vf $(TARB).tar.bz2

cleanman:
	rm -vf $(MANGROFFS)

cleanmanhtml:
	rm -vf $(MANHTMLS)

cleanmantxt:
	rm -vf $(MANTXTS)

dist: mrproper
	cd .. && tar -cvvf $(TARB).tar $(DIR_CUR)/ --exclude=.git\
		--transform "s/$(DIR_CUR)/$(TARB)/" && bzip2 -v9\
		$(TARB).tar && mv -v $(TARB).tar.bz2 $(DIR_CUR)/

man: $(MANGROFFS)

$(DIR_MAN)/%.$(MAN_CAT).final: $(DIR_MAN)/%.$(MAN_CAT)
	cat $^ | $(MAN_AWK) > $@

manhtml: $(MANHTMLS)

$(DIR_MAN)/%.html: $(DIR_MAN)/%.$(MAN_CAT)
	cat $^ | $(MAN_AWK) | man2html - > $@

mantxt: $(MANTXTS)

$(DIR_MAN)/%.txt: $(DIR_MAN)/%.$(MAN_CAT)
	cat $^ | $(MAN_AWK) | $(GROFF) - | $(COL) > $@

sloc:
	.csloc.sh

testall: teststyle testlint testleak

testleak:
	valgrind -v --leak-check=full $(BIN)

########################################################################
# -boundsread and -boundswrite are disabled because they generate too
# many false warnings about non-array pointers.
#
# -systemdirerrors is present because XLib generates millions of errors.
# ("+skip-sys-headers" causes the system directories to not be checked,
# but will cause problems as soon as something is used. Instead, all
# XLib / XPM functions are suppressed when used -- see `uix.c`.)
#
# -warnposixheaders is necessary because when splint includes
# `unistd.h`, which includes `bits/confname.h`, there is a parse error.
# If it wasn't for this, `-posix-strict-lib` is preferred over
# `-posix-lib`.
########################################################################
testlint: lang
	cppcheck --enable=all $(DIR_SRC)
	splint -strict -strict-lib -ansi89limits \
		-booltype HT_BOOL -booltrue HT_TRUE \
		-boolfalse HT_FALSE $(MACROS) \
		-boundsread -boundswrite \
		+likelyboundsread +likelyboundswrite \
		-systemdirerrors \
		-exportheader -topuse \
		-warnposixheaders \
		$(INC_DIRS) $(SRCS) $(INC)

teststyle: lang
	.cstyle.sh

########################################################################
# You do not need root for installing: just change `PREFIX` to a base
# directory that you can write to -- preferably an absolute path (or you
# will have to change the `path` in all ruleset files to make them
# indicate an absolute path).
#
# To do it manually, or if my install rule isn't good enough for your
# fancy package manager:
#
# 1.	Run `make` as a normal user. This creates the binary.
# 2.	Copy `bin/hnefatafl` (the binary) to
#	`/usr/local/games/hnefatafl`, or wherever you want it.
# 3.	Copy all files in `rules/*` to
#	`/usr/local/share/games/hnefatafl/rules/`, or wherever you want
#	them. Change the `path` argument in every ruleset file to
#	indicate the path of the directory containing the XPM files that
#	you want to use.
# 4.	Copy whichever manual page you want (`man/hnefatafl_*.6`) to
#	`/usr/local/share/man/man6/hnefatafl.6` (or wherever you want
#	it).
# 5.	Optionally replace the following strings in the manual page:
#	*	`GROFFHT_PATH_DATA_RULES` by `PREFIX_DATA_RULES_CONF`.
#	*	`GROFFHT_PATH_RC` by `${HOME}/$(FILE_RC)`.
#		Note that `${HOME}` is literal, but `$(FILE_RC)` is not.
#	*	`GROFFHT_VERSION` by `VERSION`.
# 6.	`gzip -9` on the manual page
#	(`/usr/local/share/man/man6/hnefatafl.6`, or wherever you put
#	it).
# 7.	Set permission 755 for the binary and 644 for all other files
#	that were installed (noting that directories need 755).
# 8.	Optionally run `make mrproper` (though this is not done by
#	`install`) to remove temporary build files.
#
# To uninstall it later, just remove the files that you installed in the
# above steps.
########################################################################
install: all installdirs
	@make installbin
	@make installdata
	@make installman
	@echo -e "\nInstall success."
	@echo -e "\n\"\$${HOME}/$(FILE_RC)\" is just a symlink to"\
		"a ruleset file. You may change it."
ifeq ($(MACRO_UI_XLIB), -DHT_UI_XLIB)
	@echo -e "\nYou should verify that \"path\" in all ruleset"\
		"files (in \"$(PREFIX_DATA_RULES)/\") is correct."
else
	@echo -e "\nXPM files for HT_UI_XLIB were not installed."
endif

installbin:
	@cp -iv		$(BIN)		$(PREFIX_BIN)/$(NAME)
	@chmod -c	755		$(PREFIX_BIN)/$(NAME)
	@strip -v --strip-unneeded	$(PREFIX_BIN)/$(NAME)

installdata:
	@make		$(FILES_RULES)
ifeq ($(MACRO_UI_XLIB), -DHT_UI_XLIB)
	@cp -nvr	$(DIR_RULES)/*		$(PREFIX_DATA_RULES)
	@find $(PREFIX_DATA_RULES) -type f -name *.xpm -exec \
		chmod -c 644 {} \;
endif

installdirs:
	@mkdir -vp	$(PREFIX_BIN)
	@mkdir -vp	$(PREFIX_SHARE)
	@mkdir -vp	$(PREFIX_SHARE_MAN)
	@mkdir -vp	$(PREFIX_DATA)
	@mkdir -vp	$(PREFIX_DATA_RULES)

$(PREFIX_DATA_RULES)/%: $(DIR_RULES)/%
	@awk \
	"{ sub(\"HT_RULES_PATH\", \"$(PREFIX_DATA_RULES_CONF)\"); "\
	"print; }"	$^	>	$@
	@chmod -c		644	$@

installman:
	@cat $(DIR_MAN)/$(NAME)_$(LANGUAGE).$(MAN_CAT) | $(MAN_AWK) \
		> $(PREFIX_SHARE_MAN)/$(NAME).$(MAN_CAT)
	@gzip -v9	$(PREFIX_SHARE_MAN)/$(NAME).$(MAN_CAT)
	@chmod -c 644	$(PREFIX_SHARE_MAN)/$(NAME).$(MAN_CAT).gz

########################################################################
# Uninstall simply removes all the files that were installed.
########################################################################
uninstall:
	@make uninstallbin
	@make uninstalldata
	@make uninstallman
	@echo -e "\nUninstall success."
	@echo -e "\nNo user-specific files (\$${HOME}/$(FILE_RC)) were"\
		"removed."
	@echo -e "\nThese directories were not removed:"
	@echo -e "\t$(PREFIX)\n\t$(PREFIX_BIN)\n\t$(PREFIX_SHARE)\n"\
		"\t$(PREFIX_SHARE_MAN)"

uninstallbin:
	@test -f		$(PREFIX_BIN)/$(NAME)\
		&& rm -vfi	$(PREFIX_BIN)/$(NAME)	|| true

uninstalldata:
	@test -d		$(PREFIX_DATA_RULES) \
		&& echo -n	"($(PREFIX_DATA_RULES)) " \
		&& rm -rvfI	$(PREFIX_DATA_RULES)		|| true
	@test -d		$(PREFIX_DATA) \
		&& echo -n	"($(PREFIX_DATA)) " \
		&& rm -rvfI	$(PREFIX_DATA)			|| true

uninstallman:
	@test -f	$(PREFIX_SHARE_MAN)/$(NAME).$(MAN_CAT).gz\
	&& rm -vfi	$(PREFIX_SHARE_MAN)/$(NAME).$(MAN_CAT).gz\
								|| true

echo_version:
	@echo $(VERSION)

.PHONY: clean cleanbin cleandist cleanman cleanmanhtml cleanmantxt\
	echo_version dist install installbin installdata installdirs\
	installman lang man manhtml mantxt mrproper sloc testall\
	testleak testlint teststyle uninstall uninstallbin\
	uninstalldata uninstallman

