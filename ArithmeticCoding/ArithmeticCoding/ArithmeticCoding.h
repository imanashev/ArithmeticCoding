#pragma once
#include <iostream>
#include <string>

#define DEBUG_ENCODE 0
#define DEBUG_DECODE 0

/* Get alphabet from string */
std::string getAlpabet(std::string input);

/* Get symbols frequency from string */
int* getFrequency(std::string input, std::string alphabet);

/* Get symbols codeIdx in alphabet */
int getSymbolIdx(char symbol, std::string alphabet);

/* Write bits in string */
void addBit(bool bit, int bitsToFollow, std::string& output);

/* Convert bits string to unsigned short int */
unsigned short int getValue(std::string s);

/* Convert char (= 0 or 1) to unsigned short int */
unsigned short int getNewBit(char s);


/* Encode string "input" with arithmetic coding*/
std::string encode(std::string input, std::string alphabet, int * frequency);

/* Decode string "code" from arithmetic coding */
std::string decode(std::string code, std::string alphabet, int * frequency);

