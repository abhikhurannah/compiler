#include <iostream>
#include <iomanip>
#include <cctype>
#include <string>
#include <vector>
#include <cmath>
#include <sstream>
#include <map>
#include <algorithm>

// Structure to represent a custom instruction
struct Instruction {
    std::string operation; // e.g., "ADD", "MUL", etc.
    std::string operand1;  // First operand
    std::string operand2;  // Second operand (optional)
    std::string result;    // Result variable
};

class SimpleParser {
private:
    std::string input;
    size_t pos;
    int tempVarCount; // Temporary variable counter
    std::vector<Instruction> instructions;
    std::map<std::string, double> tempValues; // Store actual values for evaluation

    // Helper function to get the current character
    char currentChar() const {
        return pos < input.size() ? input[pos] : '\0';
    }

    // Helper function to advance to the next character
    void advance() {
        pos++;
    }

    // Helper function to consume whitespace
    void skipWhitespace() {
        while (currentChar() == ' ' || currentChar() == '\t')
            advance();
    }

    // Function to generate a new temporary variable
    std::string getTempVar() {
        return "temp" + std::to_string(tempVarCount++);
    }

    // Parses a number and returns it as a string
    std::string parseNumber() {
        skipWhitespace();
        std::string number;
        while (isdigit(currentChar()) || currentChar() == '.') {
            number += currentChar();
            advance();
        }
        
        // Store the parsed number in tempValues
        std::string varName = getTempVar();
        tempValues[varName] = std::stod(number);
        instructions.push_back({"LOAD", number, "", varName});
        
        return varName;
    }

    // Parses factors (numbers or expressions in parentheses)
    std::string parseFactor() {
        skipWhitespace();
        if (currentChar() == '(') {
            advance(); // Skip '('
            std::string result = parseExpression(); // Parse the inner expression
            if (currentChar() == ')') {
                advance(); // Skip ')'
            } else {
                throw std::runtime_error("Error: Missing closing parenthesis");
            }
            return result;
        } else if (isdigit(currentChar()) || currentChar() == '.') {
            return parseNumber(); // Parse and return a number
        } else {
            throw std::runtime_error("Error: Invalid character '" + std::string(1, currentChar()) + "' in input");
        }
    }

    // Parses multiplication and division
    std::string parseTerm() {
        std::string left = parseFactor(); // Parse the left factor

        while (currentChar() == '*' || currentChar() == '/') {
            char op = currentChar();
            advance(); // Move to the next character
            std::string right = parseFactor(); // Parse the right factor

            if (op == '/' && tempValues[right] == 0) {
                throw std::runtime_error("Error: Division by zero");
            }

            std::string temp = getTempVar(); // Generate a temporary variable
            instructions.push_back({op == '*' ? "MUL" : "DIV", left, right, temp});
            
            // Calculate the value
            if (op == '*')
                tempValues[temp] = tempValues[left] * tempValues[right];
            else
                tempValues[temp] = tempValues[left] / tempValues[right];
                
            left = temp; // Update left to the result of this operation
        }

        return left; // Return the final result variable
    }

    // Parses addition and subtraction
    std::string parseExpression() {
        std::string left = parseTerm(); // Parse the left term

        while (currentChar() == '+' || currentChar() == '-') {
            char op = currentChar();
            advance(); // Move to the next character
            std::string right = parseTerm(); // Parse the right term

            std::string temp = getTempVar(); // Generate a temporary variable
            instructions.push_back({op == '+' ? "ADD" : "SUB", left, right, temp});
            
            // Calculate the value
            if (op == '+')
                tempValues[temp] = tempValues[left] + tempValues[right];
            else
                tempValues[temp] = tempValues[left] - tempValues[right];
                
            left = temp; // Update left to the result of this operation
        }

        return left; // Return the final result variable
    }

public:
    SimpleParser(const std::string &expr) : input(expr), pos(0), tempVarCount(0) {}

    // Parses the full expression and generates instructions
    std::string parse() {
        std::string resultVar = parseExpression();
        skipWhitespace(); // Skip trailing whitespace
        if (currentChar() != '\0') {
            throw std::runtime_error("Error: Unexpected characters at the end of input");
        }
        return resultVar;
    }

    // Prints the generated instructions with computation steps
    void printInstructions() const {
        std::cout << "\nArithmetic Expression: " << input << "\n";
        std::cout << "-----------------\n";
        std::cout << "Computation Steps:\n";
        std::cout << "-----------------\n";
        
        for (const auto &instr : instructions) {
            std::string operationText;
            
            if (instr.operation == "LOAD") {
                operationText = "[LOAD] " + instr.operand1;
            } else if (instr.operation == "ADD") {
                operationText = "[ADD] " + instr.operand1 + " + " + instr.operand2 + " = " + 
                            std::to_string(tempValues.at(instr.result));
            } else if (instr.operation == "SUB") {
                operationText = "[SUB] " + instr.operand1 + " - " + instr.operand2 + " = " + 
                            std::to_string(tempValues.at(instr.result));
            } else if (instr.operation == "MUL") {
                operationText = "[MUL] " + instr.operand1 + " * " + instr.operand2 + " = " + 
                            std::to_string(tempValues.at(instr.result));
            } else if (instr.operation == "DIV") {
                operationText = "[DIV] " + instr.operand1 + " / " + instr.operand2 + " = " + 
                            std::to_string(tempValues.at(instr.result));
            }
            
            std::cout << operationText << " -> " << instr.result << "\n";
        }
        
        std::cout << "-----------------\n";
        if (!instructions.empty()) {
            std::cout << "Final result: " << tempValues.at(instructions.back().result) << "\n\n";
        }
    }
    
    // Get the final result value
    double getResult() const {
        return !instructions.empty() ? tempValues.at(instructions.back().result) : 0.0;
    }
};

// Class to parse and evaluate polynomials
class PolynomialParser {
private:
    struct Term {
        double coefficient;
        int exponent;
        
        // Sort by descending exponent
        bool operator<(const Term& other) const {
            return exponent > other.exponent;
        }
    };

    std::vector<Term> terms;
    std::string originalInput;

    // Parse a single term like "3x^2" or "5x" or "7"
   // Update the parseTerm method in PolynomialParser class
Term parseTerm(const std::string& termStr) {
    Term term;
    term.coefficient = 0;
    term.exponent = 0;
    
    std::string trimmed = termStr;
    // Remove whitespace
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    if (trimmed.empty()) return term;
    
    // Handle special case of just 'x'
    if (trimmed == "x") {
        term.coefficient = 1;
        term.exponent = 1;
        return term;
    }
    
    size_t xPos = trimmed.find('x');
    
    if (xPos == std::string::npos) {
        // Constant term
        try {
            term.coefficient = std::stod(trimmed);
            term.exponent = 0;
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid constant term: " + trimmed);
        }
    } else {
        // Parse coefficient
        std::string coeffStr = trimmed.substr(0, xPos);
        if (coeffStr.empty() || coeffStr == "+") {
            term.coefficient = 1.0;
        } else if (coeffStr == "-") {
            term.coefficient = -1.0;
        } else {
            try {
                term.coefficient = std::stod(coeffStr);
            } catch (const std::exception& e) {
                throw std::runtime_error("Invalid coefficient: " + coeffStr);
            }
        }
        
        // Parse exponent
        size_t caretPos = trimmed.find('^', xPos);
        if (caretPos == std::string::npos) {
            term.exponent = 1;
        } else {
            try {
                std::string expStr = trimmed.substr(caretPos + 1);
                if (expStr.empty()) {
                    throw std::runtime_error("Missing exponent after ^");
                }
                term.exponent = std::stoi(expStr);
            } catch (const std::exception& e) {
                throw std::runtime_error("Invalid exponent: " + trimmed.substr(caretPos + 1));
            }
        }
    }
    return term;
}

// Update the evaluatePolynomial function with input validation

    std::vector<std::string> splitTerms(const std::string& polyStr) {
        std::vector<std::string> result;
        std::string current;
        bool inTerm = false;
        
        for (size_t i = 0; i < polyStr.size(); i++) {
            char c = polyStr[i];
            
            // Handle the first character or signs at the beginning of terms
            if (!inTerm) {
                if (c == '+' || c == '-' || isalnum(c) || c == '.') {
                    current = c;
                    inTerm = true;
                }
                continue;
            }
            
            // Handle signs within the polynomial
            if ((c == '+' || c == '-') && i > 0) {
                // If we find a sign that's not part of an exponent notation, it's a new term
                if (polyStr[i-1] != '^' && polyStr[i-1] != 'e' && polyStr[i-1] != 'E') {
                    if (!current.empty()) {
                        result.push_back(current);
                    }
                    current = c;
                    continue;
                }
            }
            
            current += c;
        }
        
        // Add the last term
        if (!current.empty()) {
            result.push_back(current);
        }
        
        return result;
    }

    // Combine like terms
    void combineTerms() {
        std::map<int, double> exponentMap;
        
        for (const auto& term : terms) {
            exponentMap[term.exponent] += term.coefficient;
        }
        
        terms.clear();
        for (const auto& pair : exponentMap) {
            if (pair.second != 0) {  // Skip terms with coefficient 0
                terms.push_back({pair.second, pair.first});
            }
        }
        
        // Sort terms by descending exponent
        std::sort(terms.begin(), terms.end());
    }

public:
    explicit PolynomialParser(const std::string& polyStr) : originalInput(polyStr) {
        auto termStrings = splitTerms(polyStr);
        
        for (const auto& termStr : termStrings) {
            Term term = parseTerm(termStr);
            if (term.coefficient != 0) {  // Skip terms with coefficient 0
                terms.push_back(term);
            }
        }
        
        combineTerms();
    }

    std::string getPolynomialString() const {
        if (terms.empty()) return "0";
        
        std::stringstream ss;
        bool first = true;
        
        for (const auto& term : terms) {
            // Skip terms with coefficient 0
            if (term.coefficient == 0) continue;
            
            // Handle sign
            if (!first) {
                ss << (term.coefficient > 0 ? " + " : " - ");
            } else if (term.coefficient < 0) {
                ss << "-";
                first = false;
            }
            
            // Handle coefficient
            double absCoeff = std::abs(term.coefficient);
            if (first) first = false;
            
            if (term.exponent == 0) {
                // Constant term
                ss << absCoeff;
            } else {
                // Term with x
                if (absCoeff != 1) {
                    ss << absCoeff;
                }
                
                ss << "x";
                if (term.exponent > 1) {
                    ss << "^" << term.exponent;
                }
            }
        }
        
        return ss.str();
    }

    double evaluate(double x) const {
        double result = 0.0;
        int tempCount = 0;
        std::vector<std::pair<std::string, double>> computationSteps;
        
        std::cout << "\nPolynomial Expression: " << originalInput << "\n";
        std::cout << "Simplified Form: " << getPolynomialString() << "\n";
        std::cout << "Where x = " << x << "\n\n";
        std::cout << "Computation Steps:\n";
        std::cout << "-----------------\n";
        
        // Process each term
        for (const auto& term : terms) {
            std::string tempVar = "temp" + std::to_string(tempCount++);
            std::stringstream stepDesc;
            
            if (term.exponent == 0) {
                // Constant term
                stepDesc << "[LOAD] " << term.coefficient << " -> " << tempVar;
                computationSteps.push_back({stepDesc.str(), term.coefficient});
                result += term.coefficient; // Add constant term directly to result
            } else {
                // x^n term
                std::string powTempVar = "temp" + std::to_string(tempCount++);
                double powVal = std::pow(x, term.exponent);
                
                stepDesc << "[POW] x^" << term.exponent << " = " << powVal << " -> " << powTempVar;
                computationSteps.push_back({stepDesc.str(), powVal});
                
                stepDesc.str("");
                double mulVal = term.coefficient * powVal;
                stepDesc << "[MUL] " << term.coefficient << " * " << powTempVar << " = " << mulVal 
                      << " -> " << tempVar;
                computationSteps.push_back({stepDesc.str(), mulVal});
                
                result += mulVal; // Add the result of the term to the total result
            }
        }
        
        // Print all computation steps
        for (const auto& step : computationSteps) {
            std::cout << step.first << "\n";
        }
        
        std::cout << "-----------------\n";
        std::cout << "Final result: " << result << "\n\n";
        return result;
    }
};

void evaluatePolynomial() {
    std::string polyStr;
    double x;
    
    std::cout << "\n==== USER POLYNOMIAL EVALUATION ====\n";
    std::cout << "Enter polynomial (e.g. 3x^2 + 2x + 1): ";
    std::getline(std::cin, polyStr);
    
    if (polyStr.empty()) {
        throw std::runtime_error("Empty polynomial input");
    }
    
    std::cout << "Enter value of x: ";
    std::string xStr;
    std::getline(std::cin, xStr);
    
    try {
        x = std::stod(xStr);
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid x value: " + xStr);
    }
    
    PolynomialParser parser(polyStr);
    double result = parser.evaluate(x);
    std::cout << "Result: " << result << "\n";
}
    // Split the polynomial st
void evaluateArithmetic() {
    std::string input;
    std::cout << "\n==== USER ARITHMETIC EXPRESSION ====\n";
    std::cout << "Enter an arithmetic expression: ";
    std::getline(std::cin, input);

    SimpleParser parser(input);
    std::string resultVar = parser.parse();
    parser.printInstructions();
}

int main() {
    try {
        std::string choice;
        std::cout << "Enter '1' for polynomial evaluation or '2' for arithmetic expressions: ";
        std::getline(std::cin, choice);

        if (choice == "1") {
            evaluatePolynomial();
        }
        else if (choice == "2") {
            evaluateArithmetic();
        }
        else {
            std::cout << "Invalid choice" << std::endl;
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

