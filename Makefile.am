
rtems_tests_PROGRAMS = schbility
schbility_SOURCES = system.h \
	init.c \
	tasks.c	\
	samples.h \
	samples.c \
	uunifast.c

dist_rtems_tests_DATA = schbility.scn
dist_rtems_tests_DATA += schbility.doc

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/leaf.am


LINK_OBJS = $(schbility_OBJECTS)
LINK_LIBS = $(schbility_LDLIBS) -lm

schbility$(EXEEXT): $(schbility_OBJECTS) $(schbility_DEPENDENCIES)
	@rm -f schbility$(EXEEXT)
	$(make-exe)

include $(top_srcdir)/../automake/local.am
