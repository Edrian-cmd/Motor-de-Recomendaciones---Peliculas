#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "token.h"

class Lexer {
    public: 
        explicit Lexer(const std::string& linea); 
        std::vector<Token> tokenizar();
    
    private:
        std::string linea; 
}; 

#endif
