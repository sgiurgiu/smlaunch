## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(SMLAUNCH_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = smlaunch

smlaunch_SOURCES = \
	main.c \
	utils.h \
	utils.c \
	client.c \
	client.h \
	log.c \
	log.h \
	player.c \
	player.h

smlaunch_LDFLAGS = 

smlaunch_LDADD = $(SMLAUNCH_LIBS)

