#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

enum Token {
  OR_OP, XOR_OP, AND_OP, SHIFT_LEFT_OP, SHIFT_RIGHT_OP,
  NOT_OP, PAREN_OPEN, PAREN_CLOSE, HEX, NEW_LINE, EOF_
};

int HexNumber;        // Holds the most recently parsed hex value
enum Token Operator;  // Holds the most recent operator
FILE *in_file, *out_file;       // Input file pointer

// Function that parses the next token in the character string
enum Token get_next_token() {
  char current_token = fgetc(in_file);
  if (current_token != EOF && current_token != '\n') {  // Print current character to screen
    fprintf(out_file, "%c", current_token);
  }
  switch (current_token) {
    case '|':
      return OR_OP;
    case '^':
      return XOR_OP;
    case '&':
      return AND_OP;
    case '<':
      return SHIFT_LEFT_OP;
    case '>':
      return SHIFT_RIGHT_OP;
    case '~':
      return NOT_OP;
    case '(':
      return PAREN_OPEN;
    case ')':
      return PAREN_CLOSE;
    case '\n':
      return NEW_LINE;
    case EOF:
      return EOF_;
    default:
      HexNumber = (int) strtol(&current_token, NULL, 16);
      return HEX;
  }
  return HEX;
}

/*
 * --------------------------------
 *       RECURSIVE FUNCTIONS
 * --------------------------------
 * Each function follows the pattern 'X' -> 'XX' where 'X' is the original call
 * and 'XX' is the recursive call that provides the function for the language.
 */
// Last step - Unary operations -> single hex character
int C() {
  int hex = HexNumber;

  if (Operator == HEX) {  // DEFAULT CASE: Retrieves the last parsed HexNumber
    hex = HexNumber;
    Operator = get_next_token();
  } else if (Operator == NOT_OP) {
    Operator = get_next_token();
    hex = ~C();
  } else if (Operator == SHIFT_LEFT_OP) {
    Operator = get_next_token();
    hex = C()<<1;
  } else if (Operator == SHIFT_RIGHT_OP) {
    Operator = get_next_token();
    hex = C()>>1;
  } else if (Operator == PAREN_OPEN) {
    Operator = get_next_token();
    hex = E();
  } else if (Operator == PAREN_CLOSE) {
    Operator = get_next_token();
  }

  return hex;
}

// Recursive call from B() for '&' operation
int BB(int value) {
  if (Operator == AND_OP) {
    Operator = get_next_token();
    value = value & C();
    return BB(value);
  }
  return value;
}

// Follows sams strucutre as E(), leads to '&'
int B() {
  int val = C();
  return BB(val);
}

// Recursive call from A()
int AA(int value) {
  if (Operator == XOR_OP) {
    Operator = get_next_token();
    value = value ^ C();
    return AA(value);
  }
  return value;
}

// Follows same structure as E(), leads to '^'
int A() {
  int val = B();
  return AA(val);
}

// Recursive call from E()
int EE(int value) {
  if (Operator == OR_OP) {
    Operator = get_next_token();
    value = value | C();
    return EE(value);
  }
  return value;
}

// First function call
int E() {
  int val = A();
  return EE(val);
}

int main(int argc, char* argv[]) {
  // Input file
  in_file = fopen("input.txt", "r");

  if (argc > 1) {     // Open desired file
    out_file = fopen(argv[1], "w+");
  } else {            // Print to stdout
    out_file = stdout;
  }

  int val;

  Operator = get_next_token();        // First call to get_next_token
  while (Operator != EOF_) {          // Loops through all lines of input.txt
    if (Operator != NEW_LINE) {       // Account for empty line
      val = E();                      // Begin recursive descent
      if (Operator == NEW_LINE) {
        fprintf(out_file, " = %x \n", val&0xf);      // Print result
        Operator = get_next_token();
      } else {
        HexNumber = val;  // Sets hexnumber for future calculation
      }
    }
  }

  // Closes file and exits
  fclose(in_file);
  return 1;
}
