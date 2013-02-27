PROJECT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BIN         := $(PROJECT_DIR)/bin
BUILD       := $(PROJECT_DIR)/build
INCLUDE     := $(PROJECT_DIR)/include
LIB         := $(PROJECT_DIR)/lib
SRC         := $(PROJECT_DIR)/src
TEST        := $(PROJECT_DIR)/test

CXX       := g++
_CXXFLAGS := $(CXXFLAGS) -std=c++0x -I$(INCLUDE) #-Wall -Wextra
AR        := ar
_ARFLAGS  := $(ARFLAGS)s



all: text_processing \
     tokenizer \
     symbol \
     ast \
     mcc

clean:
	rm -f $(BUILD)/* $(BIN)/* $(LIB)/*

testall: test-scanner



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

$(BUILD)/syntax_error.o: $(LIB)/libtext_processing.a \
                         $(INCLUDE)/tokenizer/syntax_error.hh \
                         $(SRC)/tokenizer/syntax_error.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/syntax_error.cc -c -o $(BUILD)/syntax_error.o

$(BUILD)/tokendef.o: $(LIB)/libtext_processing.a \
                     $(INCLUDE)/tokenizer/tokendef.hh \
                     $(SRC)/tokenizer/tokendef.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/tokendef.cc -c -o $(BUILD)/tokendef.o

$(BUILD)/token_predicates.o: $(LIB)/libtext_processing.a \
                             $(INCLUDE)/tokenizer/token_predicates.hh \
                             $(INCLUDE)/tokenizer/tokendef.hh \
                             $(INCLUDE)/tokenizer/syntax_error.hh \
                             $(SRC)/tokenizer/token_predicates.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/token_predicates.cc -c -o $(BUILD)/token_predicates.o

$(BUILD)/token_extraction.o: $(LIB)/libtext_processing.a \
                             $(INCLUDE)/tokenizer/token_extraction.hh \
                             $(INCLUDE)/tokenizer/tokendef.hh \
                             $(INCLUDE)/tokenizer/token_predicates.hh \
                             $(INCLUDE)/tokenizer/syntax_error.hh \
                             $(SRC)/tokenizer/token_extraction.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/token_extraction.cc -c -o $(BUILD)/token_extraction.o

$(BUILD)/token_range.o: $(INCLUDE)/tokenizer/tokendef.hh \
                        $(INCLUDE)/tokenizer/token_range.hh \
                        $(SRC)/tokenizer/token_range.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/tokenizer/token_range.cc -c -o $(BUILD)/token_range.o

$(LIB)/libtokenizer.a: $(INCLUDE)/tokenizer.hh \
                    $(BUILD)/syntax_error.o \
                    $(BUILD)/tokendef.o \
                    $(BUILD)/token_predicates.o \
                    $(BUILD)/token_extraction.o \
                    $(BUILD)/token_range.o
	$(AR) $(_ARFLAGS) $(LIB)/libtokenizer.a \
                          $(BUILD)/syntax_error.o \
                          $(BUILD)/tokendef.o \
                          $(BUILD)/token_predicates.o \
                          $(BUILD)/token_extraction.o \
                          $(BUILD)/token_range.o

# Build the symbol table facilities

symbol: $(LIB)/libsymbol.a

$(BUILD)/symbol_table.o: $(INCLUDE)/symbol/symbol_table.hh \
                         $(SRC)/symbol/symbol_table.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/symbol/symbol_table.cc -c -o $(BUILD)/symbol_table.o

$(LIB)/libsymbol.a: $(INCLUDE)/symbol.hh \
                 $(BUILD)/symbol_table.o
	$(AR) $(ARFLAGS) $(LIB)/libsymbol.a \
                         $(BUILD)/symbol_table.o

# Build the AST facilities

ast: $(LIB)/libast.a

$(BUILD)/astdef.o: $(INCLUDE)/ast/astdef.hh \
                   $(SRC)/ast/astdef.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/ast/astdef.cc -c -o $(BUILD)/astdef.o

$(BUILD)/ast_predicates.o: $(INCLUDE)/ast/astdef.hh \
                           $(INCLUDE)/ast/ast_predicates.hh \
                           $(SRC)/ast/ast_predicates.cc
	$(CXX) $(_CXXFLAGS) $(SRC)/ast/ast_predicates.cc -c -o $(BUILD)/ast_predicates.o

$(LIB)/libast.a: $(INCLUDE)/ast.hh \
              $(BUILD)/astdef.o \
              $(BUILD)/ast_predicates.o
	$(AR) $(ARFLAGS) $(LIB)/libast.a \
                         $(BUILD)/astdef.o \
                         $(BUILD)/ast_predicates.o



# Build the main executable

mcc: $(BIN)/mcc

$(BUILD)/mcc.o: $(SRC)/mcc.cc \
                $(INCLUDE)/text_processing.hh \
                $(INCLUDE)/tokenizer.hh \
                $(INCLUDE)/ast.hh
	$(CXX) $(_CXXFLAGS) $(SRC)/mcc.cc -c -o $(BUILD)/mcc.o

$(BIN)/mcc: $(BUILD)/mcc.o \
            $(LIB)/libtext_processing.a \
            $(LIB)/libtokenizer.a \
            $(LIB)/libast.a
	$(CXX) $(_CXXFLAGS) $(BUILD)/mcc.o \
                            -L$(LIB) \
                            -ltext_processing \
                            -last \
                            -ltokenizer \
                            -o $(BIN)/mcc



# Run unit tests

test-scanner: $(BIN)/mcc
	$(TEST)/scanner/run_tests.bash
