#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>


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

    std::vector<std::string> split(const std::string& string)
	{
	    // get rid of whitespace
	    std::string s = string;
	    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
	    
	    std::vector<std::string> tokens;
	    std::string token;
	    std::string delimiters = "+-*/()^";
	    std::size_t current;
	    std::size_t next = -1;
	    do
	    {
			current = next + 1;
			next = s.find_first_of(delimiters, current);
			token = s.substr(current, next-current);
			
			if (token != "")
			    tokens.push_back(token);
			
			// push operator
			tokens.push_back(std::string(1,s[next]));
	    } while (next != std::string::npos);
	    // delete last entry, which is invalid due to loop end
	    tokens.pop_back();
	    return tokens;
	}
    const std::string input;
    std::vector<std::string> tokens;
    int pos;
    int end;
public:
    Tokenizer(const std::string& input) :input{input}
	{
	    tokens = split(input);
	    pos = 0;
	    end = tokens.size();
	}

    std::string consume()
	{
	    const auto r = tokens[pos];
	    pos++;
	    return r;
	}

    bool expect(const std::string& str)
	{
	    const auto ret = this->consume();
	    if (ret != str)
	    {
		std::cout << "Expected " << str << " but got " << ret << std::endl;
		return false;
	    }
	    return true;
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
    virtual ~ExprAST() {}
    virtual void walk(){}
    virtual double evaluate(double x){ return 0; }
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
    double evaluate(double x) { return value; }
};

class FunctionExprAST : public ExprAST
{
    std::unique_ptr<ExprAST> argument;
    std::string function_name;
public:
    FunctionExprAST(const std::string& fname, std::unique_ptr<ExprAST> arg)
	: function_name{fname}, argument{std::move(arg)}{}
    void walk()
	{
	    std::cout << function_name << "(";
	    argument->walk();
	    std::cout << ")";
	}

    double evaluate(double x)
	{
	    if (function_name == "sin")
	    {
		return std::sin(argument->evaluate(x));
	    } else {
		return 0;
	    }
	}
};

class VariableExprAST : public ExprAST
{
    char c;
public:
    VariableExprAST(char c) : c{c}{}
    void walk()
	{
	    std::cout << c;
	}
    double evaluate(double x) { return x; }
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

    double evaluate(double x)
	{
	    switch (Op)
	    {
	    case '+': return (LHS->evaluate(x) + RHS->evaluate(x));
	    case '-': return (LHS->evaluate(x) - RHS->evaluate(x));
	    case '*': return (LHS->evaluate(x) * RHS->evaluate(x));
	    case '/': return (LHS->evaluate(x) / RHS->evaluate(x));
	    case '^': return (std::pow(LHS->evaluate(x), RHS->evaluate(x)));
	    }
	    return 0;
	}
};

std::unique_ptr<ExprAST> ParseAtom(Tokenizer&);
std::unique_ptr<ExprAST> ParseExpression(Tokenizer&);
std::unique_ptr<ExprAST> ParseFactor(Tokenizer& tokens)
{
	auto ret = ParseAtom(tokens);
    if (tokens.peek() == "^")
    {
		tokens.consume();
		auto exponent = ParseAtom(tokens);
		ret = std::make_unique<BinaryExprAST>('^', std::move(ret), std::move(exponent));
	}
	return ret;
}


std::unique_ptr<ExprAST> ParseExpression(Tokenizer&);
std::unique_ptr<ExprAST> ParseAtom(Tokenizer& tokens)
{
    // check for number
    if (std::isdigit(tokens.peek()[0]))
    {
		const auto number = tokens.consume();
		return std::make_unique<NumberExprAST>(std::stod(number));
    } else if (tokens.peek() == "(") {
		tokens.expect("(");
		auto ret =  ParseExpression(tokens);
		tokens.expect(")");
		return ret;
    } else if (tokens.peek() == "x")
    {
		const char var = tokens.consume()[0];
		return std::make_unique<VariableExprAST>(var);
    } else if (tokens.peek() == "sin")
    {
		tokens.consume();
		tokens.expect("(");
		auto arg = ParseExpression(tokens);
		tokens.expect(")");
		return std::make_unique<FunctionExprAST>("sin", std::move(arg));
   } else {
   		return nullptr;
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
    std::cout << std::endl << res->evaluate(1.0) << std::endl;
    double x = 0;
    for (; x<2.0; x+= 0.01)
	res->evaluate(x);
    return 0;
}
