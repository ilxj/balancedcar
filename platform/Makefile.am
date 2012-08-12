AM_CPPFLAGS=-I$(top_srcdir)/algorithm

bin_PROGRAMS=balance

balance_SOURCES=hal.h

if platformemu
balance_SOURCES+=view.hpp view.cpp simulator.cpp simulator.hpp
endif

balance_LDADD=$(top_builddir)/algorithm/libbalance.a
balance_DEPENDENCIES=$(blance_LDADD)