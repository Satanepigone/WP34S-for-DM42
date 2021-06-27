# This file is part of 34S.
# 
# 34S is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# 34S is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with 34S.  If not, see <http://www.gnu.org/licenses/>.

.EXPORT_ALL_VARIABLES:

DM42 := 1

# This file is intended only to make the auto-generated source files.

# These are:
#	charmap.c
#	consts.c
#	font.c
#	translate.c
#	catalogues.h
#	charset7.h
#	pretty.h
#	revision.h
#	user_consts.h

###################################
###################################
###################################
# Note: this Makefile makes the libdecNum34s.a library file
# for the host machine.
# After running this you need to (re)run the Makefile_calc_obj
# in decNumber/ as it overwrites the calculator decNumber files.
# I don't think you do need to run it yourself, actually.
# This job seems to be done by the top-level Makefile.
###################################
###################################
###################################

SYSTEM := Linux64

EXE :=

# overwrites built-in default CFLAGS
BASE_CFLAGS := -Wall -g -fno-common -fno-exceptions 

CFLAGS = $(BASE_CFLAGS)

CFLAGS += -O0 -DDEBUG=1

CFLAGS += -DFIX_64_BITS=1 -DFIX_LINUX_64_BITS=1

TOOLS := tools
AR=ar
RANLIB=ranlib
LDFLAGS := $(BASE_LDFLAGS)
LDCTRL :=

# $(CC) is default compiler program to use.
# RANLIB makes the index for an archive (.a) file.

HOSTCC := $(CC)
HOSTAR := $(AR)
HOSTRANLIB := $(RANLIB)
#HOSTCFLAGS := -Wall -O1 -g -DHOSTBUILD=1 -DDM42=1
HOSTCFLAGS := -w -O1 -g -DHOSTBUILD=1 -DDM42=1

OUTPUTDIR := $(SYSTEM)
UTILITIES := $(SYSTEM)

OBJECTDIR := $(OUTPUTDIR)/obj
DIRS := $(OBJECTDIR) $(OUTPUTDIR)
DNOPTS := -DNEED_D128TOSTRING=1

# Files and libraries

SRCS := keys.c display.c xeq.c prt.c decn.c complex.c stats.c \
		lcd.c int.c date.c alpha.c\
		commands.c string.c storage.c matrix.c

HEADERS := alpha.h complex.h consts.h data.h \
		date.h decn.h display.h features.h int.h keys.h lcd.h \
		stats.h xeq.h xrom.h storage.h matrix.h

# wildcard is a built-in make function. There are others.
XROM := $(wildcard xrom/*.wp34s) $(wildcard xrom/distributions/*.wp34s)

#CNSTS := $(OBJECTDIR)/libconsts.a
#LIBS += -L$(OBJECTDIR) -lconsts

LIBDN := -ldecNum34s

DNSRCS := decNumber.c decContext.c decimal64.c decimal128.c
DNOBJS := $(DNSRCS:%.c=$(OBJECTDIR)/%.o)
DNSRCS := $(DNSRCS:%.c=decNumber/%.c)
DNHDRS := $(DNSRCS:%.c=%.h) 

OPCODES := $(TOOLS)/wp34s.op

# Targets and rules
# PHONY targets don't refer to a file
.PHONY: clean chars cats xrom decNum consts

all: $(DIRS) chars cats xrom decNum consts

clean:
# Note that we don't remove const _object_ files made by ../make
	-rm -fr consts.h consts.c allconsts.c catalogues.h xrom.c
	-rm -f xrom_pre.wp34s user_consts.h wp34s_pp.lst xrom_labels.h
	-rm $(SYSTEM)/consts/const*.c
	-rm $(SYSTEM)/consts/Makefile
	-rm $(UTILITIES)/compile_consts$(EXE)


$(DIRS):
	mkdir -p -v $@

chars:	pretty.h font.c charmap.c translate.c charset7.h revision.h

cats:	catalogues.h $(OPCODES)

xrom:	xrom.c xrom_labels.h

consts: consts.c consts.h $(SYSTEM)/consts/const_10.c

decNum:	$(OBJECTDIR)/libdecNum34s.a

# Make sourcefiles

# chars first

pretty.h font.c charmap.c translate.c: $(UTILITIES)/genfont$(EXE) Makefile
	$(UTILITIES)/genfont$(EXE)

$(UTILITIES)/genfont$(EXE): genfont.c font.inc font_alias.inc licence.h Makefile
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

###

charset7.h: $(UTILITIES)/genchars7$(EXE)
	$(UTILITIES)/genchars7$(EXE) >$@

$(UTILITIES)/genchars7$(EXE): genchars7.c licence.h Makefile lcd.h
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

###

revision.h: $(UTILITIES)/create_revision$(EXE) $(HEADERS) $(SRCS) Makefile
	$(UTILITIES)/create_revision$(EXE) >$@

$(UTILITIES)/create_revision$(EXE): create_revision.c licence.h Makefile
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

##################

# Now consts - note that object files are not created, as we don't need them here.
# The "touch" is needed as compile_consts doesn't overwrite files that are already there

consts.c consts.h $(SYSTEM)/consts/const_10.c : $(UTILITIES)/compile_consts$(EXE) \
		$(DNHDRS) Makefile
	cd $(UTILITIES) && ./compile_consts$(EXE) "../" "../$(OBJECTDIR)/" && touch ./consts/const_10.c

$(UTILITIES)/compile_consts$(EXE): compile_consts.c $(DNSRCS) Makefile features.h \
	licence.h charmap.c pretty.h pretty.c translate.c font_alias.inc
	$(HOSTCC) $(HOSTCFLAGS) -IdecNumber -DNEED_D64FROMSTRING=1 \
	-DNEED_D128FROMSTRING=1 -o $@ $< $(DNSRCS)

#################

# Now cats

catalogues.h $(OPCODES): $(UTILITIES)/compile_cats$(EXE) Makefile pretty.h \
	pretty.c font.c charmap.c translate.c font_alias.inc
	$(UTILITIES)/compile_cats$(EXE) >catalogues.h 2>$(OPCODES)

$(UTILITIES)/compile_cats$(EXE): compile_cats.c consts.h xeq.h charmap.c \
	licence.h commands.c string.c prt.c consts.c pretty.c \
	pretty.h Makefile features.h
	$(HOSTCC) $(HOSTCFLAGS) -IdecNumber -o $@ $<

##################

# Now xrom

xrom.c xrom_labels.h: xrom.wp34s $(XROM) $(OPCODES) Makefile features.h data.h errors.h
	$(HOSTCC) -E -P -x c -Ixrom -DCOMPILE_XROM=1 xrom.wp34s > xrom_pre.wp34s
	$(TOOLS)/wp34s_asm.pl -pp -op $(OPCODES) -c -o xrom.c xrom_pre.wp34s

##################

# Now decNum

# This makes a decNumber archive, by compiling decNumber source files.
# It uses a Makefile in decNumber/.
$(OBJECTDIR)/libdecNum34s.a: $(DNSRCS) $(DNHDRS) features.h decNumber/Makefile Makefile
	+$(MAKE) OBJECTDIR=../$(OBJECTDIR) "CFLAGS=$(CFLAGS) $(DNOPTS)" \
	"LIB=libdecNum34s.a" -C decNumber
	rm $(OBJECTDIR)/dec??*.o

# Shouldn't need this next one?
# Rule to make any object file that is supposed to be in objectdir.
#$(OBJECTDIR)/%.o: %.c
#	$(CC) -c $(CFLAGS) -o $@ $<

