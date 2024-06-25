#include "Parser.hpp"
#include "parser/RootNode.hpp"

AstNode *Parser::parse(Lexer &lexer) { return RootNode::parse(lexer); }
