CXX = g++
CXXFLAGS += -std=c++0x
AR = ar
ARFLAGS = rvs



all: text_processing \
     scanner

clean:
	rm -f build/* bin/* lib/*



# Build the text processing library

text_processing: lib/text_processing.a

build/char_range.o: include/char_range.hh \
                    include/indexed_text.hh \
                    src/char_range.cc
	$(CXX) $(CXXFLAGS) src/char_range.cc -c -o build/char_range.o

build/indexed_text.o: include/indexed_text.hh \
                      include/itext_iter.hh \
                      src/indexed_text.cc
	$(CXX) $(CXXFLAGS) src/indexed_text.cc -c -o build/indexed_text.o

build/itext_iter.o: include/itext_iter.hh \
                    src/itext_iter.cc
	$(CXX) $(CXXFLAGS) src/itext_iter.cc -c -o build/itext_iter.o

lib/text_processing.a: include/text_processing.hh \
                       build/char_range.o \
                       build/indexed_text.o \
                       build/itext_iter.o
	$(AR) $(ARFLAGS) lib/text_processing.a \
                         build/char_range.o \
                         build/indexed_text.o \
                         build/itext_iter.o \



# Build the tokenizer facilities

build/token_predicates.o: include/token_predicates.hh \
                          include/tokendef.hh \
                          src/token_predicates.cc
	$(CXX) $(CXXFLAGS) src/token_predicates.cc -c -o build/token_predicates.o

lib/tokenizer.a: include/tokenizer.hh \
                 build/token_predicates.o
	$(AR) $(ARFLAGS) lib/tokenizer.a \
                         build/token_predicates.o



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