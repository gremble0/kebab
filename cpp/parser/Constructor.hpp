#ifndef KEBAB_CONSTRUCTOR_HPP
#define KEBAB_CONSTRUCTOR_HPP

#include <memory>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "parser/Statement.hpp"
#include "parser/Type.hpp"

namespace Kebab {
namespace Parser {

class Constructor : public AstNode {
protected:
  Constructor() = default;

  virtual void parse_type(Lexer &lexer) = 0;
  virtual void parse_body(Lexer &lexer) = 0;

public:
  virtual ~Constructor() = default;

  // This pointer can be shared with the return type of a function constructor (the return type of a
  // function constructor is the same as the type of the constructor in its body)
  std::shared_ptr<Type> type;

  static std::unique_ptr<Constructor> parse(Lexer &lexer);
};

class ListConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer);
  void parse_body(Lexer &lexer);

public:
  std::unique_ptr<ListType> type;
  std::vector<std::unique_ptr<Statement>> body;

  static std::unique_ptr<ListConstructor> parse(Lexer &lexer);
};

class FunctionParameter : public AstNode {
public:
  std::string name;
  // This type is shared with the type of the function the parameter belongs to
  std::shared_ptr<Type> type;

  static std::unique_ptr<FunctionParameter> parse(Lexer &lexer);
};

class FunctionConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer);
  void parse_body(Lexer &lexer);

public:
  std::vector<std::unique_ptr<FunctionParameter>> parameters;
  std::unique_ptr<FunctionType> type;
  std::unique_ptr<Constructor> body;

  static std::unique_ptr<FunctionConstructor> parse(Lexer &lexer);
};

class PrimitiveConstructor : public Constructor {
private:
  void parse_type(Lexer &lexer);
  void parse_body(Lexer &lexer);

public:
  std::unique_ptr<PrimitiveType> type;
  std::vector<std::unique_ptr<Statement>> body;

  static std::unique_ptr<PrimitiveConstructor> parse(Lexer &lexer);
};

} // namespace Parser
} // namespace Kebab

#endif
