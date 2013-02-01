PROJECT_DIR = $(CURDIR)
BIN         = $(PROJECT_DIR)/bin
BUILD       = $(PROJECT_DIR)/build
INCLUDE     = $(PROJECT_DIR)/include
LIB         = $(PROJECT_DIR)/lib
SRC         = $(PROJECT_DIR)/src
TEST        = $(PROJECT_DIR)/test

CXX      = g++
CXXFLAGS += -std=c++0x #-Wall -Wextra
AR       = ar
ARFLAGS  = rvs



all: text_processing \
     tokenizer \
     scanner

clean:
	rm -f $(BUILD)/* $(BIN)/* $(LIB)/*



# Build the text processing library

text_processing: $(LIB)/text_processing.a

$(BUILD)/char_range.o: $(INCLUDE)/char_range.hh \
                       $(SRC)/char_range.cc
	$(CXX) $(CXXFLAGS) $(SRC)/char_range.cc -c -o $(BUILD)/char_range.o

$(BUILD)/file_position.o: $(INCLUDE)/file_position.hh \
                          $(INCLUDE)/char_range.hh \
                          $(SRC)/file_position.cc
	$(CXX) $(CXXFLAGS) $(SRC)/file_position.cc -c -o $(BUILD)/file_position.o

$(LIB)/text_processing.a: $(INCLUDE)/text_processing.hh \
                          $(BUILD)/char_range.o \
                          $(BUILD)/file_position.o
	$(AR) $(ARFLAGS) $(LIB)/text_processing.a \
                         $(BUILD)/char_range.o \
                         $(BUILD)/file_position.o



# Build the tokenizer facilities

tokenizer: $(LIB)/tokenizer.a

$(BUILD)/syntax_error.o: $(LIB)/text_processing.a \
                         $(INCLUDE)/syntax_error.hh \
                         $(SRC)/syntax_error.cc
	$(CXX) $(CXXFLAGS) $(SRC)/syntax_error.cc -c -o $(BUILD)/syntax_error.o

$(BUILD)/tokendef.o: $(INCLUDE)/tokendef.hh \
                     $(SRC)/tokendef.cc
	$(CXX) $(CXXFLAGS) $(SRC)/tokendef.cc -c -o $(BUILD)/tokendef.o

$(BUILD)/token_predicates.o: $(LIB)/text_processing.a \
                             $(INCLUDE)/token_predicates.hh \
                             $(INCLUDE)/tokendef.hh \
                             $(INCLUDE)/syntax_error.hh \
                             $(SRC)/token_predicates.cc
	$(CXX) $(CXXFLAGS) $(SRC)/token_predicates.cc -c -o $(BUILD)/token_predicates.o

$(BUILD)/token_extraction.o: $(LIB)/text_processing.a \
                             $(INCLUDE)/token_extraction.hh \
                             $(INCLUDE)/tokendef.hh \
                             $(INCLUDE)/token_predicates.hh \
                             $(INCLUDE)/syntax_error.hh \
                             $(SRC)/token_extraction.cc
	$(CXX) $(CXXFLAGS) $(SRC)/token_extraction.cc -c -o $(BUILD)/token_extraction.o

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

$(BUILD)/main.o: $(PROJECT_DIR)/main.cc \
                 $(INCLUDE)/text_processing.hh \
                 $(INCLUDE)/tokenizer.hh
	$(CXX) $(CXXFLAGS) main.cc -c -o $(BUILD)/main.o

$(BIN)/scanner: $(BUILD)/main.o \
                $(LIB)/text_processing.a \
                $(LIB)/tokenizer.a
	$(CXX) $(CXXFLAGS) $(BUILD)/main.o \
                           $(LIB)/text_processing.a \
                           $(LIB)/tokenizer.a \
                           -o $(BIN)/scanner



# Run tests

test: scanner
	$(TEST)/run_tests.bash