#pragma once

#include <string>
#include <istream>


enum class TokenType {
    WORD,
    STRING,         // quoted strings
    SYMBOL,         // { } ;
    END_OF_FILE,    // end of stream
    INVALID
};

struct Token {
    TokenType   type;
    std::string value;
};


class Tokenizer {

private:
    std::istream&   input_;
    char            currentChar_;

    void            advance();
    void            skipWhiteSpace();
    bool            isSymbol(char c) const;
    Token           readSymbol();
    Token           readComment();
    Token           readQuotedString();
    Token           readWord();

public:
    explicit        Tokenizer(std::istream& input);

    // prevent copy and assignment
    Tokenizer(const Tokenizer&) = delete;
    Tokenizer& operator=(const Tokenizer&) = delete;


    Token           next();
    Token           peek();
};

// don't need OCF class bc we don't have ownership
