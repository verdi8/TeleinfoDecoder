
# Répertoires
SOURCEDIR = src
TESTDIR = test
BUILDDIR = build
LIBDIR = lib
BINDIR = bin
DOCDIR = docs

# Compilation
CC = g++
CCFLAGS = -g -L $(LIBDIR) -I $(SOURCEDIR)

# Archivage
AR = ar
ARFLAGS = crf

# Construction -----------------------------------------------------------------------------------------

all: clean build

clean:
	$(RM) -r $(BUILDDIR)/*
	$(RM) -r $(DOCDIR)/*
	$(RM) -r $(LIBDIR)/*

.PHONY: build
build: clean
	$(CC) $(CCFLAGS) -c -o ${BUILDDIR}/TeleinfoDecoder.o $(SOURCEDIR)/TeleinfoDecoder.cpp
	$(AR) $(ARFLAGS) ${LIBDIR}/libteleinfodecoder.a ${BUILDDIR}/TeleinfoDecoder.o 

# Tests ------------------------------------------------------------------------------------------------

clean-test:
	$(RM) -r $(BUILDDIR)/*
	$(RM) -r $(BINDIR)/*

build-test: clean build
	$(CC) $(CCFLAGS) -c -o ${BUILDDIR}/TeleinfoDecoderTest.o $(TESTDIR)/TeleinfoDecoderTest.cpp
	$(CC) $(CCFLAGS) -c -o ${BUILDDIR}/runtests.o $(TESTDIR)/runtests.cpp
	$(CC) $(CCFLAGS) -o ${BINDIR}/runtests $(BUILDDIR)/*.o $(LIBDIR)/*.a -lcppunit 

run-test: build-test
	${BINDIR}/runtests
	
.PHONY: test
test-all: all clean-test build-test run-test
