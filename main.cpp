#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <locale>



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


std::unique_ptr<ExprAST> ParseExpression(Tokenizer&);

std::unique_ptr<ExprAST> ParseFactor(Tokenizer& tokens)
{
    // check for number
    if (std::isdigit(tokens.peek()[0]))
    {
	const auto number = tokens.consume();
	return std::make_unique<NumberExprAST>(std::stoi(number));
    } else {
	auto openp = tokens.consume();
	if (openp != "(")
	    std::cout << "error: expected '(' but got " << openp << std::endl;
	auto ret =  ParseExpression(tokens);
	auto closingp = tokens.consume();
	if (closingp != ")")
	    std::cout << "error: expected ')' but got " << closingp << std::endl;
	return ret;
    }
}


std::unique_ptr<ExprAST> ParseTerm(Tokenizer& tokens)
{
    std::unique_ptr<ExprAST> res;
    res = ParseFactor(tokens);
    while (tokens.peek() == "*" || tokens.peek() == "/")
    {
	// get operator token
	auto op_tok = tokens.consume();
	// parse next term
	auto next = ParseFactor(tokens);

	char op = 0;
	if (op_tok == "*")
	    op = '*';
	else
	    op = '/';

	res = std::make_unique<BinaryExprAST>(op, std::move(res), std::move(next));
    }
    return res;
    
}   

std::unique_ptr<ExprAST> ParseExpression(Tokenizer& tokens)
{
    std::unique_ptr<ExprAST> res;

    res = ParseTerm(tokens);
    while (tokens.peek() == "+" || tokens.peek() == "-")
    {
	// get operator token
	auto op_tok = tokens.consume();
	// parse next term
	auto next = ParseTerm(tokens);

	char op = 0;
	if (op_tok == "+")
	    op = '+';
	else
	    op = '-';

	res = std::make_unique<BinaryExprAST>(op, std::move(res), std::move(next));
    }

    return res;
}

int main(int argc, char* argv[])
{
    std::string input;
    std::getline(std::cin, input);
    Tokenizer tokens(input);
    auto res = ParseExpression(tokens);
    res->walk();
    return 0;
}
