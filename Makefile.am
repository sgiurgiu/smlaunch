## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

smlaunchdocdir = ${prefix}/doc/smlaunch
smlaunchdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(smlaunchdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(smlaunchdocdir)
