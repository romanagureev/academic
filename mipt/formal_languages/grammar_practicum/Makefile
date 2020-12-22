CC=g++
CFLAGS=-std=c++17

CFGrammar = CFGRecognizer.h
TESTS = test.cpp
RUN_TOOL = run.cpp

all: build_tests build_run_tool

build_tests:
	$(CC) $(CFLAGS) -o run_tests $(TESTS)

build_run_tool:
	$(CC) $(CFLAGS) -o run $(RUN_TOOL)
