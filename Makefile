
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

# Les targets
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
	$(CC) $(CCFLAGS) -o ${BINDIR}/runtests.exe $(BUILDDIR)/*.o $(LIBDIR)/*.a -lcppunit 

run-test: build-test
	${BINDIR}/runtests.exe
	
.PHONY: test
test: clean build clean-test build-test run-test

all: clean build

generate-docs:
	doxygen doxygen.conf
