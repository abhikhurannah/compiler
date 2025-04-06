#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

// Structure to represent a custom instruction
struct Instruction
{
    std::string operation; // e.g., "ADD", "MUL", etc.
    std::string operand1;  // First operand
    std::string operand2;  // Second operand (optional)
    std::string result;    // Result variable
};

class SimpleParser
{
private:
    const char *input;
    int pos;
    int tempVarCount; // Temporary variable counter
    std::vector<Instruction> instructions;

    // Helper function to get the current character
    char currentChar() const
    {
        return input[pos];
    }

    // Helper function to advance to the next character
    void advance()
    {
        pos++;
    }

    // Helper function to consume whitespace
    void skipWhitespace()
    {
        while (currentChar() == ' ')
        {
            advance();
        }
    }

    // Function to generate a new temporary variable
    std::string getTempVar()
    {
        return "temp" + std::to_string(tempVarCount++);
    }

    // Parses a number and returns it as a string
    std::string parseNumber()
    {
        skipWhitespace();
        std::string number;
        while (isdigit(currentChar()) || currentChar() == '.')
        {
            number += currentChar();
            advance();
        }
        return number; // Return the parsed number as a string
    }

    // Parses factors (numbers or expressions in parentheses)
    std::string parseFactor()
    {
        skipWhitespace();
        if (currentChar() == '(')
        {
            advance();                              // Skip '('
            std::string result = parseExpression(); // Parse the inner expression
            if (currentChar() == ')')
            {
                advance(); // Skip ')'
            }
            else
            {
                throw std::runtime_error("Error: Missing closing parenthesis");
            }
            return result;
        }
        else if (isdigit(currentChar()) || currentChar() == '.')
        {
            return parseNumber(); // Parse and return a number
        }
        else
        {
            throw std::runtime_error(std::string("Error: Invalid character '") + currentChar() + "' in input");
        }
    }

    // Parses multiplication and division
    std::string parseTerm()
    {
        std::string left = parseFactor(); // Parse the left factor

        while (currentChar() == '*' || currentChar() == '/')
        {
            char op = currentChar();
            advance();                         // Move to the next character
            std::string right = parseFactor(); // Parse the right factor

            if (op == '/' && right == "0")
            {
                throw std::runtime_error("Error: Division by zero");
            }

            std::string temp = getTempVar(); // Generate a temporary variable
            instructions.push_back({op == '*' ? "MUL" : "DIV", left, right, temp});
            left = temp; // Update left to the result of this operation
        }

        return left; // Return the final result variable
    }

    // Parses addition and subtraction
    std::string parseExpression()
    {
        std::string left = parseTerm(); // Parse the left term

        while (currentChar() == '+' || currentChar() == '-')
        {
            char op = currentChar();
            advance();                       // Move to the next character
            std::string right = parseTerm(); // Parse the right term

            std::string temp = getTempVar(); // Generate a temporary variable
            instructions.push_back({op == '+' ? "ADD" : "SUB", left, right, temp});
            left = temp; // Update left to the result of this operation
        }

        return left; // Return the final result variable
    }

public:
    SimpleParser(const std::string &expr) : input(expr.c_str()), pos(0), tempVarCount(0) {}

    // Parses the full expression and generates instructions
    void parse()
    {
        parseExpression();
        skipWhitespace(); // Skip trailing whitespace
        if (currentChar() != '\0')
        {
            throw std::runtime_error("Error: Unexpected characters at the end of input");
        }
    }

    // Prints the generated instructions
    void printInstructions() const
    {
        std::cout << "Generated Instructions:\n";
        for (const auto &instr : instructions)
        {
            std::cout << instr.operation << " " << instr.operand1;
            if (!instr.operand2.empty())
            {
                std::cout << " " << instr.operand2;
            }
            std::cout << " -> " << instr.result << "\n";
        }
    }
};

// Class to parse and evaluate polynomials
class PolynomialParser {
private:
    struct Term {
        double coefficient;
        int exponent;
    };

    std::vector<Term> terms;

    // Parse a single term like "3x^2" or "5x" or "7"
    Term parseTerm(const std::string& termStr) {
        Term term;
        size_t xPos = termStr.find('x');
        
        if (xPos == std::string::npos) {
            // Constant term (no x)
            term.coefficient = std::stod(termStr);
            term.exponent = 0;
        } else {
            // Parse coefficient
            std::string coeffStr = termStr.substr(0, xPos);
            term.coefficient = coeffStr.empty() ? 1.0 : 
                             (coeffStr == "-" ? -1.0 : std::stod(coeffStr));
            
            // Parse exponent
            size_t caretPos = termStr.find('^', xPos);
            if (caretPos == std::string::npos) {
                // Handle implicit exponent (like 3x or x2)
                if (xPos + 1 < termStr.size() && isdigit(termStr[xPos+1])) {
                    term.exponent = termStr[xPos+1] - '0';
                } else {
                    term.exponent = 1;
                }
            } else {
                term.exponent = std::stoi(termStr.substr(caretPos + 1));
            }
        }
        return term;
    }

public:
    PolynomialParser(const std::string& polyStr) {
        std::string processed;
        // Add spaces around operators for easier parsing
        for (char c : polyStr) {
            if (c == '+' || c == '-') {
                processed += ' ';
                processed += c;
                processed += ' ';
            } else {
                processed += c;
            }
        }

        std::istringstream iss(processed);
        std::string termStr;
        char op = '+';
        
        while (iss >> termStr) {
            if (termStr == "+" || termStr == "-") {
                op = termStr[0];
                continue;
            }
            
            Term term = parseTerm(termStr);
            if (op == '-') {
                term.coefficient = -term.coefficient;
            }
            terms.push_back(term);
            
            // Reset op to '+' for next term unless specified otherwise
            op = '+';
        }
    }

    double evaluate(double x) const {
        double result = 0.0;
        int tempCount = 0;
        std::vector<double> tempValues;
        
        std::cout << "\nComputation Steps:\n";
        std::cout << "-----------------\n";
        for (const auto& term : terms) {
            if (term.exponent == 0) {
                // Constant term
                std::cout << "[LOAD] " << term.coefficient << " -> temp" << tempCount << "\n";
                tempValues.push_back(term.coefficient);
            }
            else {
                // x^n term
                double powVal = std::pow(x, term.exponent);
                std::cout << "[POW] x^" << term.exponent << " = " << powVal << " -> temp" << tempCount << "\n";
                double mulVal = term.coefficient * powVal;
                std::cout << "[MUL] " << term.coefficient << " * temp" << tempCount << " = " << mulVal 
                          << " -> temp" << (tempCount+1) << "\n";
                tempValues.push_back(mulVal);
                tempCount++;
            }
            tempCount++;
        }

        // Sum all terms
        result = tempValues[0];
        for (size_t i = 1; i < tempValues.size(); i++) {
            std::cout << "[ADD] temp" << (tempCount-1) << " + temp" << i << " = " << (result + tempValues[i])
                      << " -> temp" << tempCount << "\n";
            result += tempValues[i];
            tempCount++;
        }
        
        std::cout << "-----------------\n";
        std::cout << "Final result: " << result << "\n\n";
        return result;
    }
};

int main()
{
    try
    {
        std::string choice;
        std::cout << "Enter '1' for polynomial evaluation or '2' for arithmetic expressions: ";
        std::getline(std::cin, choice);

        if (choice == "1") {
            std::string polyStr;
            double x;
            
            std::cout << "Enter polynomial (e.g. 3x^2 + 2x + 1): ";
            std::getline(std::cin, polyStr);
            
            std::cout << "Enter value of x: ";
            std::cin >> x;
            std::cin.ignore(); // Clear newline
            
            PolynomialParser parser(polyStr);
            double result = parser.evaluate(x);
            std::cout << "Result: " << result << std::endl;
        }
        else if (choice == "2") {
            std::string input;
            std::cout << "Enter an arithmetic expression: ";
            std::getline(std::cin, input);

            SimpleParser parser(input);
            parser.parse();
            parser.printInstructions();
        }
        else {
            std::cout << "Invalid choice" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
