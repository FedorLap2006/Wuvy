#include <iostream>
#include <Wuvy/Lexer.hpp>
#include <cctype>
//#include <filesystem>

// struct SerealizeVisitor : public Patterns::Visitor<Lexer::IdentToken, Lexer::KeywordToken, Lexer::OperatorToken, Lexer::NumberToken> {
//     SerealizeVisitor() {}
//   void visit(Lexer::IdentToken const& tok) override {
//     std::cout << "Ident token(" << tok.getLocation().getFilepath() << ", " << tok.getLocation().getLine() << ", " 
//       << tok.getLocation().getCol() << "): " << tok.getIdent() << std::endl;
//   }
//   void visit(Lexer::KeywordToken const& tok) override {
//     std::cout << "Keyword token(" << tok.getLocation().getFilepath() << ", " << tok.getLocation().getLine() << ", " 
//       << tok.getLocation().getCol() << "): " << static_cast<int>(tok.getIdent())  << std::endl;
//   }
//   void visit(Lexer::OperatorToken const& tok) override {
//     std::cout << "Operator token(" << tok.getLocation().getFilepath() << ", " << tok.getLocation().getLine() << ", " 
//       << tok.getLocation().getCol() << "): " << static_cast<int>(tok.getIdent())  << std::endl;
//   }
//   void visit(Lexer::NumberToken const& tok) override {
//     std::cout << "Number token(" << tok.getLocation().getFilepath() << ", " << tok.getLocation().getLine() << ", " 
//       << tok.getLocation().getCol() << "): " << tok.getIdent()  << std::endl;
//   }
// }


struct SerealizeVisitor : public Patterns::Visitor<void, Lexer::IdentToken, Lexer::KeywordToken, Lexer::OperatorToken, Lexer::NumberToken, Lexer::EndToken> {
  void visit(Lexer::IdentToken& token) override {
    std::cout << "Ident token(" << token.getLocation().getFilepath() << ", " << token.getLocation().getLine() << ", " 
       << token.getLocation().getCol() << "): " << token.getIdent() << std::endl;
  } 
  void visit(Lexer::KeywordToken& token) override {
    std::cout << "Keyword token(" << token.getLocation().getFilepath() << ", " << token.getLocation().getLine() << ", " 
       << token.getLocation().getCol() << "): " << static_cast<int>(token.getIdent()) << std::endl;
  } 
  void visit(Lexer::OperatorToken& token) override {
    std::cout << "Operator token(" << token.getLocation().getFilepath() << ", " << token.getLocation().getLine() << ", " 
       << token.getLocation().getCol() << "): " << static_cast<int>(token.getIdent()) << std::endl;
  } 
  void visit(Lexer::NumberToken& token) override {
    std::cout << "Operator token(" << token.getLocation().getFilepath() << ", " << token.getLocation().getLine() << ", " 
       << token.getLocation().getCol() << "): " << token.getIdent() << std::endl;
  }   
  void visit(Lexer::EndToken& token) override {
    std::cout << "Terminating...";
    exit(0);
  } 

  void visit(Patterns::Visitable<void>& visitable) override {}
//  void visit(Patterns::Visitable const& token) const override {} // this used for production, not debug version
};

int main(int argc, char **argv) {
  std::cout << "Starting..." << std::endl;
  // Lexer::Location loc("test", 0, 32);
  // Utils::Types::PShared<Lexer::BaseToken> ptr = Utils::Functions::pMakeShared<Lexer::NumberToken>(100, loc);
  // SerealizeVisitor v;

  // ptr->accept(v);
  Utils::Types::StringDataset<Lexer::KeywordId> keywords = { {1, "if"}, {2, "goto"} };
  Utils::Types::StringDataset<Lexer::OperatorId> operators = { {3, "+"}, {4, "-"}, {5, "@"}, {6, "="} };
  Lexer::Instance instance(keywords, operators);
  SerealizeVisitor v;

  instance.run("TestSource.txt");

  while(true) instance.getToken()->accept(v);
 
  //Utils::Functions::isAllMatch("a", std::isdigit);
  // Utils::Functions::isAllMatch("a", [](int c) -> int { 
  //   return std::isdigit(c);
  // });
  
  //throw "ya";
  // Lexer::StringDataset<Keyword> keywords = {
  //   {1, "goto"},
  //   {2, "if"},
  // };
  // Lexer::StringDataset<Operator> operators = {
  //   {1, "="},
  //   {2, "+"},
  // };
  // Lexer::Instance inst(keywords, operators);

  // Lexer::TokenList list = inst.Run("test.txt");
  // SerealizeVisitor visitor;
  // for(auto element : list) {
  //   element.accept(visitor);
  // }
}
