#
# Makefile for rogue
# @(#)Makefile	4.21 (Berkeley) 02/04/99
#
# Rogue: Exploring the Dungeons of Doom
# Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
# All rights reserved.
#
# See the file LICENSE.TXT for full copyright and licensing information.
#

DISTNAME=rogue5.4.2
PROGRAM=rogue54

O=o

HDRS=	rogue.h extern.h score.h

OBJS1 =	vers.$(O) extern.$(O) armor.$(O) chase.$(O) command.$(O) daemon.$(O) \
	daemons.$(O) fight.$(O) init.$(O) io.$(O) list.$(O) mach_dep.$(O) \
	main.$(O) mdport.$(O) misc.$(O) monsters.$(O) move.$(O) new_level.$(O)
OBJS2 = options.$(O) pack.$(O) passages.$(O) potions.$(O) rings.$(O) rip.$(O) \
        rooms.$(O) save.$(O) scrolls.$(O) state.$(O) sticks.$(O) things.$(O) \
        weapons.$(O) wizard.$(O) xcrypt.$(O)
OBJS  =	$(OBJS1) $(OBJS2)

CFILES=	vers.c extern.c armor.c chase.c command.c daemon.c \
	daemons.c fight.c init.c io.c list.c mach_dep.c \
	main.c  mdport.c misc.c monsters.c move.c new_level.c \
	options.c pack.c passages.c potions.c rings.c rip.c \
	rooms.c save.c scrolls.c state.c sticks.c things.c \
	weapons.c wizard.c xcrypt.c
MISC_C=	findpw.c scedit.c scmisc.c
DOCSRC= rogue.me rogue.6
DOCS  = $(PROGRAM).doc $(PROGRAM).html $(PROGRAM).cat readme54.html
MISC  =	Makefile $(MISC_C) LICENSE.TXT $(PROGRAM).sln $(PROGRAM).vcproj $(DOCS)\
        $(DOCSRC)

CC    = gcc
ROPTS = -DALLSCORES -DSCOREFILE
COPTS = -O3
CFLAGS= $(COPTS) $(ROPTS) 
LIBS =	-lcurses
RM    = rm -f

.SUFFIXES: .obj

.c.obj:
	$(CC) $(CFLAGS) /c $*.c
    
$(PROGRAM): $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
    
clean:
	$(RM) $(OBJS1)
	$(RM) $(OBJS2)
	$(RM) core a.exe a.out a.exe.stackdump $(PROGRAM) $(PROGRAM).exe $(PROGRAM).lck
	$(RM) $(PROGRAM).tar $(PROGRAM).tar.gz $(PROGRAM).zip 
    
dist.src:
	make clean
	tar cf $(DISTNAME)-src.tar $(CFILES) $(HDRS) $(MISC)
	gzip -f $(DISTNAME)-src.tar

findpw: findpw.c xcrypt.o mdport.o xcrypt.o
	$(CC) -s -o findpw findpw.c xcrypt.o mdport.o -lcurses

scedit: scedit.o scmisc.o vers.o mdport.o xcrypt.o
	$(CC) -s -o scedit vers.o scedit.o scmisc.o mdport.o xcrypt.o -lcurses

scmisc.o scedit.o:
	$(CC) -O -c $(SF) $*.c

doc.nroff:
	tbl rogue.me | nroff -me | colcrt - > rogue.doc
	nroff -man rogue.6 | colcrt - > rogue.cat

doc.groff:
	groff -P-c -t -me -Tascii rogue.me | sed -e 's/.\x08//g' > rogue.doc
	groff -man rogue.6 | sed -e 's/.\x08//g' > rogue.cat

dist.irix:
	make clean
	make CC=cc COPTS="-woff 1116 -O3" $(PROGRAM)
	tar cf $(DISTNAME)-irix.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-irix.tar

dist.aix:
	make clean
	make CC=xlc COPTS="-qmaxmem=16768 -O3 -qstrict" $(PROGRAM)
	tar cf $(DISTNAME)-aix.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-aix.tar

dist.linux:
	make clean
	make $(PROGRAM)
	tar cf $(DISTNAME)-linux.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-linux.tar
	
dist.interix:
	@$(MAKE) clean
	@$(MAKE) COPTS="-ansi" $(PROGRAM)
	tar cf $(DISTNAME)-interix.tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-interix.tar
	
dist.cygwin:
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory $(PROGRAM)
	tar cf $(DISTNAME)-cygwin.tar $(PROGRAM).exe LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-cygwin.tar

#
# Use MINGW32-MAKE to build this target
#
dist.mingw32:
	@$(MAKE) --no-print-directory RM="cmd /c del" clean
	@$(MAKE) --no-print-directory LIBS="-lpdcurses" $(PROGRAM)
	cmd /c del $(DISTNAME)-mingw32.zip
	zip $(DISTNAME)-mingw32.zip $(PROGRAM).exe LICENSE.TXT $(DOCS)
	
dist.msys:
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory LIBS="-lcurses" $(PROGRAM)
	tar cf $(DISTNAME)-msys.tar $(PROGRAM).exe LICENSE.TXT $(DOCS)
	gzip -f $(DISTNAME)-msys.tar
	
dist.djgpp:
	@$(MAKE) --no-print-directory clean
	@$(MAKE) --no-print-directory LDFLAGS="-L$(DJDIR)/LIB" \
	LIBS="-lpdcurses" $(PROGRAM)
	rm -f $(DISTNAME)-djgpp.zip
	zip $(DISTNAME)-djgpp.zip $(PROGRAM) LICENSE.TXT $(DOCS)

#
# Use NMAKE to build this targer
#
dist.win32:
	@$(MAKE) /NOLOGO O="obj" RM="-del" clean
	@$(MAKE) /NOLOGO O="obj" CC="CL" \
	    LIBS="..\pdcurses\pdcurses.lib shell32.lib user32.lib Advapi32.lib" \
	    COPTS="-nologo -I..\pdcurses \
	    -Ox -wd4033 -wd4716" $(PROGRAM)
	-del $(DISTNAME)-win32.zip
	zip $(DISTNAME)-win32.zip $(PROGRAM).exe LICENSE.TXT $(DOCS)
