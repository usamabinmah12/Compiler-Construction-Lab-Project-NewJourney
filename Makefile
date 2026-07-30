CXX      = g++
CXXFLAGS = -std=c++17 -I . -I src -I src/ast -I src/parser -I src/lexer \
           -I src/symbol -I src/semantic -I src/codegen

BISON = bison
FLEX  = flex

PARSER_SRC = src/parser/parser.y
LEXER_SRC  = src/lexer/lexer.l

PARSER_CPP = src/parser/parser.tab.cpp
PARSER_HDR = src/parser/parser.tab.h
LEXER_CPP  = src/lexer/lexer.yy.cpp

TARGET = compiler

all: $(TARGET)

$(PARSER_CPP) $(PARSER_HDR): $(PARSER_SRC)
	$(BISON) -d --defines=$(PARSER_HDR) -o $(PARSER_CPP) $(PARSER_SRC)

$(LEXER_CPP): $(LEXER_SRC) $(PARSER_HDR)
	$(FLEX) -o $(LEXER_CPP) $(LEXER_SRC)

$(TARGET): src/main.cpp $(PARSER_CPP) $(LEXER_CPP)
	$(CXX) $(CXXFLAGS) -o $(TARGET) src/main.cpp $(PARSER_CPP) $(LEXER_CPP)

clean:
	rm -f $(PARSER_CPP) $(PARSER_HDR) $(LEXER_CPP) $(TARGET) $(TARGET).exe

.PHONY: all clean