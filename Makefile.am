includepixdir = $(includedir)/pix
CPPFLAGS = -I $(top_builddir) -I $(top_srcdir)/include -I $(top_srcdir)/src

include_HEADERS = \
	$(top_builddir)/libpix.config.h \
	$(top_srcdir)/include/libpix.h

includepix_HEADERS = \
	$(top_srcdir)/include/pix/config.h \
	$(top_srcdir)/include/pix/utils.hpp \
	$(top_srcdir)/include/pix/memory.hpp \
	$(top_srcdir)/include/pix/string.hpp \
	$(top_srcdir)/include/pix/info.hpp \
	$(top_srcdir)/include/pix/stream.hpp \
	$(top_srcdir)/include/pix/image.hpp \
	$(top_srcdir)/include/pix/image_jpg.hpp \
	$(top_srcdir)/include/pix/filter_resize.hpp \
	$(top_srcdir)/include/pix/system.hpp \
	$(top_srcdir)/include/pix/view.hpp


lib_LTLIBRARIES = libpix.la

bin_PROGRAMS = pix

libpix_la_SOURCES = \
	$(top_srcdir)/src/libpix.cpp \
	$(top_srcdir)/src/types.cpp \
	$(top_srcdir)/src/utils.cpp \
	$(top_srcdir)/src/memory.cpp \
	$(top_srcdir)/src/string.cpp \
	$(top_srcdir)/src/info.cpp \
	$(top_srcdir)/src/stream.cpp \
	$(top_srcdir)/src/stream_file.cpp \
	$(top_srcdir)/src/stream_mem.cpp \
	$(top_srcdir)/src/image.cpp \
	$(top_srcdir)/src/image_jpg.cpp \
	$(top_srcdir)/src/filter_resize.cpp \
	\
	$(top_srcdir)/src/system.cpp \
	\
	$(top_srcdir)/src/pix_buf.cpp \
	$(top_srcdir)/src/pix_str.cpp \
	$(top_srcdir)/src/pix_stm.cpp \
	$(top_srcdir)/src/pix_inf.cpp

libpix_la_LDFLAGS = -version-info 0:1:0

pix_SOURCES = \
	$(top_srcdir)/src/pix.cpp

pix_LDADD = \
	$(top_builddir)/libpix.la


