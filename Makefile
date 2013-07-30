# OneJoker RNG library <http://lcrocker.github.io/onejoker/cardlib>
#
# Linux Makefile -- requires Gmake 3.8, realpath, and probably some
# other things I take for granted.
#

ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRCDIR = $(ROOT)/source
TESTDIR = $(ROOT)/tests
BLDDIR = $(ROOT)/build
MODDIR = ojcardlib
CLASSDIR = com/onejoker/cardlib

CC = gcc
CFLAGS = -g -DDEBUG -Wall -Wextra -std=c99 -pedantic -fpic
# CFLAGS = -O3 -DNDEBUG -Wall -std=c99 -pedantic -fpic
CXX = g++
CXXFLAGS = -g -DDEBUG -Wall -Wextra -std=c++98 -pedantic -fpic
LD = g++
LDFLAGS = -nostartfiles
SYSTEMLIBS = -lm

JAVA_HOME ?= /usr/java
JAVACFLAGS = -g -Werror
# JAVACFLAGS = -g:none
JPACKAGE = $(subst /,.,$(CLASSDIR))

LIBNAME = libojcard.so
CNAMES = init deckinfo text prng cardlist combiner blackjack poker
PYNAMES = __init__ core text cardlist combiner
JNAMES = Card CardList DeckType
TESTNAMES = basic hello cardlist combiner cpphello
# random hello.py Hello.class

LIBOBJECTS = $(patsubst %,$(BLDDIR)/%.o,$(CNAMES))
# LIBOBJECTS += $(BLDDIR)/wrapper.o
# LIBOBJECTS += $(patsubst %,$(BLDDIR)/jni%.o,$(JNAMES))
PYFILES = $(patsubst %,$(BLDDIR)/$(MODDIR)/%.py,$(PYNAMES))
JCLASSES = $(patsubst %,$(BLDDIR)/$(CLASSDIR)/%.class,$(JNAMES))
JHEADERS = $(patsubst %,$(BLDDIR)/com_onejoker_cardlib_%.h,$(JNAMES))
TESTPROGS = $(patsubst %,$(BLDDIR)/t_%,$(TESTNAMES))

.PHONY: all lib test clean python java

all: lib python java test

lib: $(BLDDIR)/$(LIBNAME)

python: $(PYFILES)

java: $(JCLASSES) $(JHEADERS)

test: $(TESTPROGS)
	cd $(BLDDIR) && ./t_basic
	cd $(BLDDIR) && ./t_cpphello
	cd $(BLDDIR) && ./t_cardlist
	# cd $(BLDDIR) && ./t_combiner
	# cd $(BLDDIR) && python3 ./hello.py
	# cd $(BLDDIR) && java -ea -cp "." -Djava.library.path="." Hello

clean:
	rm -rf $(BLDDIR)/*

$(BLDDIR):
	mkdir -p $(BLDDIR)

$(BLDDIR)/$(MODDIR):
	mkdir -p $(BLDDIR)/$(MODDIR)

$(BLDDIR)/$(CLASSDIR):
	mkdir -p $(BLDDIR)/$(CLASSDIR)

$(BLDDIR)/wrapper.o: $(SRCDIR)/library/wrapper.cc $(SRCDIR)/library/ojcardlib.h | $(BLDDIR)
	$(CXX) $(CXXFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/jni%.o: $(SRCDIR)/java/$(CLASSDIR)/jni%.c $(SRCDIR)/java/com_onejoker_cardlib_%.h $(SRCDIR)/library/ojcardlib.h | $(BLDDIR)
	$(CC) $(CFLAGS) -c -I$(JAVA_HOME)/include -I$(SRCDIR)/library -I$(SRCDIR)/java -o $@ $<

$(BLDDIR)/%.o: $(SRCDIR)/library/%.c $(SRCDIR)/library/ojcardlib.h | $(BLDDIR)
	$(CC) $(CFLAGS) -c -I$(SRCDIR)/library -o $@ $<

$(BLDDIR)/$(CLASSDIR)/%.class: $(SRCDIR)/java/$(CLASSDIR)/%.java | $(BLDDIR)/$(CLASSDIR)
	javac $(JAVACFLAGS) -cp $(SRCDIR)/java -d $(BLDDIR) $<

$(BLDDIR)/com_onejoker_cardlib_%.h: $(BLDDIR)/$(CLASSDIR)/%.class
	cd $(BLDDIR) && javah -jni -force $(JPACKAGE).$*

$(BLDDIR)/$(MODDIR)/%.py: $(SRCDIR)/python/$(MODDIR)/%.py | $(BLDDIR)/$(MODDIR)
	cp $< $@

$(BLDDIR)/$(LIBNAME): $(LIBOBJECTS)
	$(LD) $(LDFLAGS) -shared -o $@ $^ $(SYSTEMLIBS)

$(BLDDIR)/t_cpphello: $(TESTDIR)/cpp/hello.cc $(BLDDIR)/$(LIBNAME)
	$(CXX) $(CXXFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lojcard

$(BLDDIR)/t_combiner: $(TESTDIR)/c/combiner.c $(TESTDIR)/c/stats.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(TESTDIR)/c -I$(SRCDIR)/library -o $@ $^ -lojcard -lm

$(BLDDIR)/t_%: $(TESTDIR)/c/%.c $(BLDDIR)/$(LIBNAME)
	$(CC) $(CFLAGS) -L$(BLDDIR) -I$(SRCDIR)/library -o $@ $< -lojcard

$(BLDDIR)/hello.py: $(TESTDIR)/python/hello.py python
	cp $< $@

$(BLDDIR)/Hello.class: $(TESTDIR)/java/Hello.java | $(BLDDIR)
	javac $(JAVACFLAGS) -cp $(BLDDIR) -d $(BLDDIR) $<
