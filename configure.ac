#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([LibPix], [0.0.1], [yang@libpix.org])
AC_CONFIG_SRCDIR([src/libpix.cpp])
AC_CONFIG_HEADERS([libpix.config.h])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for header files.
AC_CHECK_HEADERS([iconv.h])
AC_CHECK_HEADERS([jpeglib.h])
AC_CHECK_HEADERS([tiff.h])

# Checks for libraries.
AC_SEARCH_LIBS([iconv_open], [iconv], \
               [echo "------------------------iconv found-------------------"])
AC_SEARCH_LIBS([deflate], [z], \
               [echo "-------------------------zlib found-------------------"])
AC_SEARCH_LIBS([jpeg_destroy_compress], [jpeg], \
               [echo "-------------------------jpeg found-------------------"]) 
AC_SEARCH_LIBS([TIFFOpen], [tiff], \
               [echo "-------------------------tiff found-------------------"])
AC_SEARCH_LIBS([png_access_version_number], [png], \
               [echo "--------------------------png found-------------------"])
AC_SEARCH_LIBS([cmsCreateTransform], [lcms], \
               [echo "-------------------------lcms found-------------------"])
AC_SEARCH_LIBS([FT_Bitmap_New], [freetype], \
               [echo "---------------------freetype found-------------------"])


AC_OUTPUT
