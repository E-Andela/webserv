#include "Config/Tokenizer.hpp"
#include <cctype>
#include <sstream>

Tokenizer::Tokenizer(std::istream& input): input_(input), currentChar_(' ') {
    advance();
}

void Tokenizer::advance() {
    currentChar_ = input_.get();
    if (input_.eof())
        currentChar_ = '\0'; // NULL char on eof
}

void Tokenizer::skipWhiteSpace() {
    while (std::isspace(currentChar_))
        advance();
}

Token Tokenizer::readComment() {
    while (currentChar_ != '\n' && !input_.eof())
        advance();
    return next(); // go to the next valid token
}

bool Tokenizer::isSymbol(char c) const {
    return c == '{' || c == '}' || c == ';';
}


Token Tokenizer::readSymbol() {
    char symbol = currentChar_;
    advance();
    return {TokenType::SYMBOL, std::string(1, symbol)}; // c++11+ feature list initialization 
}

Token Tokenizer::readQuotedString() {
    std::string value;
    advance();
    while (currentChar_ != '"' && !input_.eof()) {
        if (currentChar_ == '\\') { // support escaping
            advance();
            if (input_.eof()) break;
        }
        value += currentChar_;
        advance();
    }
    if (currentChar_ == '"') {
        advance(); // skip closing quote
        return {TokenType::STRING, value};
    } else
        return {TokenType::INVALID, "Unterminated string"};
}


Token Tokenizer::readWord() {
    std::string word;
        while (!input_.eof() && !std::isspace(currentChar_) &&
                currentChar_ != '{' && currentChar_ != '}' &&
                currentChar_ != ';') {
            word += currentChar_;
            advance();
        }
        return { TokenType::WORD, word };
}

/* Returns the next valid token from the input config stream */
Token Tokenizer::next() {
    skipWhiteSpace();

    if (input_.eof() || currentChar_ == '\0')
        return {TokenType::END_OF_FILE, ""};
    
    if (currentChar_ == '#')
        return readComment();

    if (isSymbol(currentChar_))
        return readSymbol();
    

    // handle quoted strings
    if (currentChar_ == '"')
        return readQuotedString();


    // a word token constitudes of alphanumeric for listen, server_name, etc - and punctuatuion such as /blah/blah or IP addresses
    // TODO: maybe restrcit the punctutaion for more exact char
    if (std::isalnum(currentChar_) || std::ispunct(currentChar_))
        return readWord();

    char invalid = currentChar_;
    advance();
    return {TokenType::INVALID, std::string(1, invalid)};
}

// does not advance the input stream
Token Tokenizer::peek() {
    std::streampos prevPos = input_.tellg(); // gives currect read position in input stream
    char prevChar = currentChar_;
    Token token = next();
    input_.clear(); // if stream reached eof, clear() resets internal error flags to continue reading
    input_.seekg(prevPos); // rewinf input stream to where we were (peek, not consume)
    currentChar_ = prevChar; // restore currentChar

    return token;
}



// TODO:
// error checking - decide on what is fatal what's a warning
// more punctuatuon checks
// escape support for quotes ?
