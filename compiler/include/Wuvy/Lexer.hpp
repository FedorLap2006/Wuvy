#pragma once

#include <Wuvy/Common/Types.hpp>
#include <Wuvy/Utils/Functions.hpp>
#include <string>
#include <Wuvy/Common/Patterns/Visitor.hpp>

namespace Lexer {
  using namespace Common;
  using namespace Utils;

  class Location {
  public:
    Location(std::string filepath, int line, int col) : m_Filepath(filepath), m_Line(line), m_Col(col) {}
    Location() : m_Filepath(""), m_Line(0), m_Col(0) {}

    std::string getFilepath() { return m_Filepath; }
    void setFilepath(std::string filepath) { m_Filepath = filepath; }

    int getLine() { return m_Line + 1; }
    void setLine(int line) { m_Line = line + 1; }

    int getCol() { return m_Col + 1; }
    void setCol(int col) { m_Col = col + 1; }

  private:
    std::string m_Filepath;
    int m_Line = 0, m_Col = 0;
  };

  using VisitorTraits = Patterns::VisitorTraits<void>;

  class BaseToken : public Patterns::BaseVisitable<VisitorTraits> {
  public:
    BaseToken(Location location) : m_Location(location) {}
    virtual ~BaseToken() = default;
    MAKE_VISITABLE();


    Location getLocation() const { return m_Location; }
    void setLocation(Location location) { m_Location = location; }

  protected:
    Location m_Location;
  };
  using TokenContainer = Types::PShared<BaseToken>;
  using TokenList = Types::List<TokenContainer>;

  class IdentToken : public BaseToken {
  public:
    IdentToken(std::string ident, Location location) : BaseToken(location), m_Ident(ident) {}
    ~IdentToken() = default;

    MAKE_VISITABLE();


    std::string getIdent() const { return m_Ident; }
    void setIdent(std::string ident) { m_Ident = ident; }

  private:
    std::string m_Ident;
  };

  using KeywordId = int; // Language::Keywords;

  class KeywordToken : public BaseToken {
  public:
    KeywordToken(KeywordId ident, Location location) : BaseToken(location), m_Ident(ident) {}
    ~KeywordToken() = default;

    MAKE_VISITABLE();


    
    KeywordId getIdent() const { return m_Ident; }
    void setIdent(KeywordId ident) { m_Ident = ident; }

  private:
    KeywordId m_Ident = 0;
  };

  using OperatorId = int; 

  class OperatorToken : public BaseToken {
  public:
    OperatorToken(OperatorId ident, Location location) : BaseToken(location), m_Ident(ident) {}
    ~OperatorToken() = default;

    MAKE_VISITABLE();

    OperatorId getIdent() const { return m_Ident; }
    void setIdent(OperatorId ident) { m_Ident = ident; }
  private:
    OperatorId m_Ident = 0;
  };

  class NumberToken : public BaseToken {
  public:
    NumberToken(int ident, Location location) : BaseToken(location), m_Ident(ident) {}
    ~NumberToken() = default;

    MAKE_VISITABLE();

    int getIdent() const { return m_Ident; }
    void setIdent(int ident) { m_Ident = ident; }
  private:
    int m_Ident = 0;
  };

  class EndToken : public BaseToken {
  public:
    EndToken() : BaseToken(Location()) {}

    MAKE_VISITABLE();
  };

  class Instance {
  public:
    Instance(Types::StringDataset<KeywordId> keywords, Types::StringDataset<OperatorId> operators) : m_Keywords(keywords), m_Operators(operators) {}



    void restoreToken(TokenContainer token) { m_CurrentTokenList.push_front(token); }
    TokenContainer getToken() { 
      if(m_CurrentTokenList.empty()) return Functions::pMakeShared<EndToken>();
      //auto ret = m_CurrentTokenList.back(); m_CurrentTokenList.pop_back(); return ret;
      auto v = m_CurrentTokenList.front();
      m_CurrentTokenList.pop_front();
      return v;
    }
    TokenContainer peekToken() {
      auto res = getToken();
      restoreToken(res);
      return res;
    }


    void run(std::string filepath);

  private:

    TokenList processString(std::string line, Location initialLoc);

    TokenList m_CurrentTokenList;
    Types::StringDataset<KeywordId> m_Keywords;
    Types::StringDataset<OperatorId> m_Operators;

    // int m_CurrentTokenIndex;
  };  

}