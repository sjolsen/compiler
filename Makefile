PROJECT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BIN         := $(PROJECT_DIR)/bin
BUILD       := $(PROJECT_DIR)/build
INCLUDE     := $(PROJECT_DIR)/include
LIB         := $(PROJECT_DIR)/lib
SRC         := $(PROJECT_DIR)/src
TEST        := $(PROJECT_DIR)/test

CXX      := g++
CXXFLAGS += -std=c++0x #-Wall -Wextra
AR       := ar
ARFLAGS  := $(ARFLAGS)s



all: text_processing \
     tokenizer \
     scanner

clean:
	rm -f $(BUILD)/* $(BIN)/* $(LIB)/*

testall: test-scanner



# Build the text processing library

text_processing: $(LIB)/text_processing.a

$(BUILD)/char_range.o: $(INCLUDE)/text_processing/char_range.hh \
                       $(SRC)/text_processing/char_range.cc
	$(CXX) $(CXXFLAGS) $(SRC)/text_processing/char_range.cc -c -o $(BUILD)/char_range.o

$(BUILD)/file_position.o: $(INCLUDE)/text_processing/file_position.hh \
                          $(INCLUDE)/text_processing/char_range.hh \
                          $(SRC)/text_processing/file_position.cc
	$(CXX) $(CXXFLAGS) $(SRC)/text_processing/file_position.cc -c -o $(BUILD)/file_position.o

$(LIB)/text_processing.a: $(INCLUDE)/text_processing.hh \
                          $(BUILD)/char_range.o \
                          $(BUILD)/file_position.o
	$(AR) $(ARFLAGS) $(LIB)/text_processing.a \
                         $(BUILD)/char_range.o \
                         $(BUILD)/file_position.o



# Build the tokenizer facilities

tokenizer: $(LIB)/tokenizer.a

$(BUILD)/syntax_error.o: $(LIB)/text_processing.a \
                         $(INCLUDE)/tokenizer/syntax_error.hh \
                         $(SRC)/tokenizer/syntax_error.cc
	$(CXX) $(CXXFLAGS) $(SRC)/tokenizer/syntax_error.cc -c -o $(BUILD)/syntax_error.o

$(BUILD)/tokendef.o: $(INCLUDE)/tokenizer/tokendef.hh \
                     $(SRC)/tokenizer/tokendef.cc
	$(CXX) $(CXXFLAGS) $(SRC)/tokenizer/tokendef.cc -c -o $(BUILD)/tokendef.o

$(BUILD)/token_predicates.o: $(LIB)/text_processing.a \
                             $(INCLUDE)/tokenizer/token_predicates.hh \
                             $(INCLUDE)/tokenizer/tokendef.hh \
                             $(INCLUDE)/tokenizer/syntax_error.hh \
                             $(SRC)/tokenizer/token_predicates.cc
	$(CXX) $(CXXFLAGS) $(SRC)/tokenizer/token_predicates.cc -c -o $(BUILD)/token_predicates.o

$(BUILD)/token_extraction.o: $(LIB)/text_processing.a \
                             $(INCLUDE)/tokenizer/token_extraction.hh \
                             $(INCLUDE)/tokenizer/tokendef.hh \
                             $(INCLUDE)/tokenizer/token_predicates.hh \
                             $(INCLUDE)/tokenizer/syntax_error.hh \
                             $(SRC)/tokenizer/token_extraction.cc
	$(CXX) $(CXXFLAGS) $(SRC)/tokenizer/token_extraction.cc -c -o $(BUILD)/token_extraction.o

$(LIB)/tokenizer.a: $(INCLUDE)/tokenizer.hh \
                    $(BUILD)/syntax_error.o \
                    $(BUILD)/tokendef.o \
                    $(BUILD)/token_predicates.o \
                    $(BUILD)/token_extraction.o
	$(AR) $(ARFLAGS) $(LIB)/tokenizer.a \
                         $(BUILD)/syntax_error.o \
                         $(BUILD)/tokendef.o \
                         $(BUILD)/token_predicates.o \
                         $(BUILD)/token_extraction.o



# Build the scanner-driver

scanner: $(BIN)/scanner

$(BUILD)/scanner.o: $(SRC)/test-drivers/scanner.cc \
                    $(INCLUDE)/text_processing.hh \
                    $(INCLUDE)/tokenizer.hh
	$(CXX) $(CXXFLAGS) $(SRC)/test-drivers/scanner.cc -c -o $(BUILD)/scanner.o

$(BIN)/scanner: $(BUILD)/scanner.o \
                $(LIB)/text_processing.a \
                $(LIB)/tokenizer.a
	$(CXX) $(CXXFLAGS) $(BUILD)/scanner.o \
                           $(LIB)/text_processing.a \
                           $(LIB)/tokenizer.a \
                           -o $(BIN)/scanner



# Run unit tests

test-scanner: $(BIN)/scanner
	$(TEST)/scanner/run_tests.bash
