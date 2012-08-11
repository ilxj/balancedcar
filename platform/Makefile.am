AM_CPPFLAGS=-I$(top_srcdir)/algorithm

bin_PROGRAMS=blance

blance_SOURCES=hal.h

if platformemu
blance_SOURCES+=view.hpp view.cpp simulator.cpp simulator.hpp
endif

blance_LDADD=$(top_builddir)/algorithm/libbalance.a
blance_DEPENDENCIES=$(blance_LDADD)