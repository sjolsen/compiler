PROJECT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BIN         := $(PROJECT_DIR)/bin
BUILD       := $(PROJECT_DIR)/build
INCLUDE     := $(PROJECT_DIR)/include
LIB         := $(PROJECT_DIR)/lib
SRC         := $(PROJECT_DIR)/src
TEST        := $(PROJECT_DIR)/test

CXX       := g++
_CXXFLAGS := -std=c++0x -O0 -g -I$(INCLUDE) $(CXXFLAGS) #-Wall -Wextra
AR        := ar
_ARFLAGS  := $(ARFLAGS)s



all: text_processing \
     tokenizer \
     ast \
     semantic \
     mcc

clean:
	rm -f $(BUILD)/* $(BIN)/* $(LIB)/*

testall: test-scanner \
         test-parser \
         test-semantic



# Build the text processing library

text_processing: $(LIB)/libtext_processing.a

$(BUILD)/char_range.o: $(INCLUDE)/text_processing/char_range.hh \
                       $(SRC)/text_processing/char_range.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/text_processing/char_range.cc -c -o $(BUILD)/char_range.o

$(BUILD)/file_position.o: $(INCLUDE)/text_processing/file_position.hh \
                          $(INCLUDE)/text_processing/char_range.hh \
                          $(SRC)/text_processing/file_position.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/text_processing/file_position.cc -c -o $(BUILD)/file_position.o

$(LIB)/libtext_processing.a: $(INCLUDE)/text_processing.hh \
                             $(BUILD)/char_range.o \
                             $(BUILD)/file_position.o
	$(AR) $(_ARFLAGS) $(LIB)/libtext_processing.a \
                          $(BUILD)/char_range.o \
                          $(BUILD)/file_position.o



# Build the tokenizer facilities

tokenizer: $(LIB)/libtokenizer.a

$(BUILD)/error.o: $(LIB)/libtext_processing.a \
                         $(INCLUDE)/tokenizer/error.hh \
                         $(SRC)/tokenizer/error.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/error.cc -c -o $(BUILD)/error.o

$(BUILD)/tokendef.o: $(LIB)/libtext_processing.a \
                     $(INCLUDE)/tokenizer/tokendef.hh \
                     $(SRC)/tokenizer/tokendef.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/tokendef.cc -c -o $(BUILD)/tokendef.o

$(BUILD)/token_predicates.o: $(LIB)/libtext_processing.a \
                             $(INCLUDE)/tokenizer/token_predicates.hh \
                             $(INCLUDE)/tokenizer/tokendef.hh \
                             $(INCLUDE)/tokenizer/error.hh \
                             $(SRC)/tokenizer/token_predicates.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/token_predicates.cc -c -o $(BUILD)/token_predicates.o

$(BUILD)/token_extraction.o: $(LIB)/libtext_processing.a \
                             $(INCLUDE)/tokenizer/token_extraction.hh \
                             $(INCLUDE)/tokenizer/tokendef.hh \
                             $(INCLUDE)/tokenizer/token_predicates.hh \
                             $(INCLUDE)/tokenizer/error.hh \
                             $(SRC)/tokenizer/token_extraction.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/token_extraction.cc -c -o $(BUILD)/token_extraction.o

$(BUILD)/token_range.o: $(INCLUDE)/tokenizer/tokendef.hh \
                        $(INCLUDE)/tokenizer/token_range.hh \
                        $(SRC)/tokenizer/token_range.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/token_range.cc -c -o $(BUILD)/token_range.o

$(LIB)/libtokenizer.a: $(INCLUDE)/tokenizer.hh \
                       $(BUILD)/error.o \
                       $(BUILD)/tokendef.o \
                       $(BUILD)/token_predicates.o \
                       $(BUILD)/token_extraction.o \
                       $(BUILD)/token_range.o
	$(AR) $(_ARFLAGS) $(LIB)/libtokenizer.a \
                          $(BUILD)/error.o \
                          $(BUILD)/tokendef.o \
                          $(BUILD)/token_predicates.o \
                          $(BUILD)/token_extraction.o \
                          $(BUILD)/token_range.o



# Build the AST facilities

ast: $(LIB)/libast.a

$(BUILD)/astprint.o: $(INCLUDE)/ast/astdef.hh \
                     $(INCLUDE)/ast/symbol_table.hh \
                     $(INCLUDE)/ast/astprint.hh \
                     $(SRC)/ast/astprint.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/ast/astprint.cc -c -o $(BUILD)/astprint.o

$(BUILD)/astdef.o: $(INCLUDE)/ast/astdef.hh \
                   $(INCLUDE)/ast/astprint.hh \
                   $(SRC)/ast/astdef.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/ast/astdef.cc -c -o $(BUILD)/astdef.o

$(BUILD)/ast_nodes.o: $(INCLUDE)/ast/astdef.hh \
                      $(INCLUDE)/ast/symbol_table.hh \
                      $(INCLUDE)/ast/ast_nodes.hh \
                      $(INCLUDE)/ast/auto.astdecls.hh \
                      $(INCLUDE)/ast/astprint.hh \
                      $(SRC)/ast/ast_nodes.cc \
                      $(SRC)/ast/auto.astdefs.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/ast/ast_nodes.cc -c -o $(BUILD)/ast_nodes.o

$(BUILD)/ast_predicates.o: $(INCLUDE)/ast/astdef.hh \
                           $(INCLUDE)/ast/symbol_table.hh \
                           $(INCLUDE)/ast/ast_nodes.hh \
                           $(INCLUDE)/ast/ast_predicates.hh \
                           $(SRC)/ast/ast_predicates.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/ast/ast_predicates.cc -c -o $(BUILD)/ast_predicates.o

$(BUILD)/ast_pos.o: $(INCLUDE)/ast/astdef.hh \
                    $(INCLUDE)/ast/ast_nodes.hh \
                    $(SRC)/ast/ast_pos.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/ast/ast_pos.cc -c -o $(BUILD)/ast_pos.o

$(LIB)/libast.a: $(INCLUDE)/ast.hh \
                 $(BUILD)/astprint.o \
                 $(BUILD)/astdef.o \
                 $(BUILD)/ast_nodes.o \
                 $(BUILD)/ast_predicates.o \
                 $(BUILD)/ast_pos.o
	$(AR) $(ARFLAGS) $(LIB)/libast.a \
                         $(BUILD)/astprint.o \
                         $(BUILD)/astdef.o \
                         $(BUILD)/ast_nodes.o \
                         $(BUILD)/ast_predicates.o \
                         $(BUILD)/ast_pos.o



# Build the static analysis library

semantic: $(LIB)/libsemantic.a

$(BUILD)/semantic.o: $(LIB)/libast.a \
                     $(INCLUDE)/tokenizer.hh \
                     $(INCLUDE)/semantic.hh \
                     $(SRC)/semantic.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/semantic.cc -c -o $(BUILD)/semantic.o

$(LIB)/libsemantic.a: $(INCLUDE)/semantic.hh \
                      $(BUILD)/semantic.o
	$(AR) $(ARFLAGS) $(LIB)/libsemantic.a \
                         $(BUILD)/semantic.o



# Build the main executable

mcc: $(BIN)/mcc

$(BUILD)/mcc.o: $(SRC)/mcc.cc \
                $(INCLUDE)/text_processing.hh \
                $(INCLUDE)/tokenizer.hh \
                $(INCLUDE)/ast.hh \
                $(INCLUDE)/semantic.hh
	$(CXX) $(_CXXFLAGS) $(SRC)/mcc.cc -c -o $(BUILD)/mcc.o

$(BIN)/mcc: $(BUILD)/mcc.o \
            $(LIB)/libtext_processing.a \
            $(LIB)/libtokenizer.a \
            $(LIB)/libast.a \
            $(LIB)/libsemantic.a
	$(CXX) $(_CXXFLAGS) $(BUILD)/mcc.o \
                            -L$(LIB) \
                            -last \
                            -ltokenizer \
                            -ltext_processing \
                            -lsemantic \
                            -o $(BIN)/mcc



# Run unit tests

test-scanner: $(BIN)/mcc
	$(TEST)/scanner/run_tests.bash

test-parser: $(BIN)/mcc
	$(TEST)/parser/run_tests.bash

test-semantic: $(BIN)/mcc
	$(TEST)/semantic/run_tests.bash
