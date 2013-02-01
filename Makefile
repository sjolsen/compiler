CXX = g++
CXXFLAGS += -std=c++0x #-Wall -Wextra
AR = ar
ARFLAGS = rvs



all: text_processing \
     tokenizer \
     scanner

clean:
	rm -f build/* bin/* lib/*



# Build the text processing library

text_processing: lib/text_processing.a

build/char_range.o: include/char_range.hh \
                    src/char_range.cc
	$(CXX) $(CXXFLAGS) src/char_range.cc -c -o build/char_range.o

build/file_position.o: include/file_position.hh \
                       include/char_range.hh \
                       src/file_position.cc
	$(CXX) $(CXXFLAGS) src/file_position.cc -c -o build/file_position.o

lib/text_processing.a: include/text_processing.hh \
                       build/char_range.o \
                       build/file_position.o
	$(AR) $(ARFLAGS) lib/text_processing.a \
                         build/char_range.o \
                         build/file_position.o



# Build the tokenizer facilities

tokenizer: lib/tokenizer.a

build/syntax_error.o: lib/text_processing.a \
                      include/syntax_error.hh \
                      src/syntax_error.cc
	$(CXX) $(CXXFLAGS) src/syntax_error.cc -c -o build/syntax_error.o

build/tokendef.o: include/tokendef.hh \
                  src/tokendef.cc
	$(CXX) $(CXXFLAGS) src/tokendef.cc -c -o build/tokendef.o

build/token_predicates.o: lib/text_processing.a \
                          include/token_predicates.hh \
                          include/tokendef.hh \
                          include/syntax_error.hh \
                          src/token_predicates.cc
	$(CXX) $(CXXFLAGS) src/token_predicates.cc -c -o build/token_predicates.o

build/token_extraction.o: lib/text_processing.a \
                          include/token_extraction.hh \
                          include/tokendef.hh \
                          include/token_predicates.hh \
                          include/syntax_error.hh \
                          src/token_extraction.cc
	$(CXX) $(CXXFLAGS) src/token_extraction.cc -c -o build/token_extraction.o

lib/tokenizer.a: include/tokenizer.hh \
                 build/syntax_error.o \
                 build/tokendef.o \
                 build/token_predicates.o \
                 build/token_extraction.o
	$(AR) $(ARFLAGS) lib/tokenizer.a \
                         build/syntax_error.o \
                         build/tokendef.o \
                         build/token_predicates.o \
                         build/token_extraction.o



# Build the scanner-driver

scanner: bin/scanner

build/main.o: main.cc \
              include/text_processing.hh \
              include/tokenizer.hh
	$(CXX) $(CXXFLAGS) main.cc -c -o build/main.o

bin/scanner: build/main.o \
             lib/text_processing.a \
             lib/tokenizer.a
	$(CXX) $(CXXFLAGS) build/main.o \
                           lib/text_processing.a \
                           lib/tokenizer.a \
                           -o bin/scanner
