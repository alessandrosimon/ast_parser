#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class Tokenizer
{
    std::vector<std::string> split(const std::string& s, char delimiter)
	{
	    std::vector<std::string> tokens;
	    std::string token;
	    std::istringstream tokenStream(s);
	    while (std::getline(tokenStream, token, delimiter))
	    {
		tokens.push_back(token);
	    }
	    return tokens;
	}

    const std::string input;
    std::vector<std::string> tokens;
    int pos;
    int end;
public:
    Tokenizer(const std::string& input) :input{input}
	{
	    tokens = split(input, ' ');
	    pos = 0;
	    end = tokens.size();
	}

    std::string consume()
	{
	    const auto r = tokens[pos];
	    pos++;
	    return r;
	}
    std::string peek()
	{
	    return tokens[pos];
	}

    bool finished()
	{
	    return (pos == end);
	}
};
	    
	

class ExprAST
{
public:
    virtual ~ExprAST() {};
    virtual void walk(){};
};


class NumberExprAST : public ExprAST
{
    double value;
public:
    NumberExprAST(double value) : value{value}{}
    void walk()
	{
	    std::cout << value;
	}
};

class BinaryExprAST : public ExprAST
{
    char Op;
    std::unique_ptr<ExprAST> LHS, RHS;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
		  std::unique_ptr<ExprAST> RHS)
	: Op{op}, LHS{std::move(LHS)}, RHS{std::move(RHS)}{}

    void walk()
	{
	    std::cout << "(" << Op << ",";
	    LHS->walk();
	    std::cout << ",";
	    RHS->walk();
	    std::cout << ")";

	}
};

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

void println(const std::string& str)
{
    std::cout << str << std::endl;
}

const char* text = "5 + 2 - 2";

std::unique_ptr<ExprAST> ParseTerm(const std::string& tok)
{
    return std::make_unique<NumberExprAST>(std::stoi(tok));
}

// std::unique_ptr<ExprAST> ParseFactor(const std::string& tok)
// {

// }

int main()
{
    Tokenizer tokens(text);
    std::unique_ptr<ExprAST> res;
    while (!tokens.finished())
    {

	res = ParseTerm(tokens.consume());

	while (tokens.peek() == "+" || tokens.peek() == "-")
	{
	    auto tok = tokens.consume();
	    auto next = ParseTerm(tokens.consume());

	    char op = 0;
	    if (tok == "+")
		op = '+';
	    else
		op = '-';

	    res = std::make_unique<BinaryExprAST>(op, std::move(res), std::move(next));
	}
    }
    res->walk();
    return 0;
}
