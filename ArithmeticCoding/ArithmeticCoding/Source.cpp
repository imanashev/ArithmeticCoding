#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>

#include <bitset>

#include <stdlib.h>
#include <ctype.h>

using namespace std;


#define SIZE 255
#define DEBUG_ENCODE 0
#define DEBUG_DECODE 0


#define FIRST_QTR h[0] / 4 + 1
#define HALF  2 * FIRST_QTR
#define THIRD_QTR  3 * FIRST_QTR

void gen(char* p, size_t N)
{
	while (N--)
	{
		char ch;
		while ((ch = (char)(rand() % (127 - '0' + 1) + '0')), !isalnum(ch))
			;
		*p++ = ch;
	}
	*p = 0;
}


string getAlpabet(string input)
{
	string alphabet;
	alphabet.push_back('-');
	alphabet.push_back('\0');
	for (int i = 0; i < input.size(); i++)
	{
		int j;
		for (j = 0; j < alphabet.size(); j++)
		{
			if (alphabet[j] == input[i])
			{
				break;
			}
		}
		if (j >= alphabet.size())
		{
			alphabet.push_back(input[i]);
		}

		sort(alphabet.begin() + 1, alphabet.end());
	}
	return alphabet;
}

unsigned short int* getFrequency(string input, string alphabet)
{
	unsigned short int* frequency = new unsigned short int[alphabet.size()];
	frequency[0] = 0;
	for (int i = 1; i < alphabet.size(); ++i)
	{
		frequency[i] = 1;
	}
	for (int i = 0; i < input.size(); i++)
	{
		bool flag = false;
		for (int j = 0; j < alphabet.size(); j++)
		{
			if (alphabet[j] == input[i] || flag)
			{
				frequency[j]++;
				flag = true;
			}
		}
	}
	return frequency;
}

int getSymbolIdx(char symbol, string alphabet)
{
	for (int i = 0; i < alphabet.size(); ++i)
	{
		if (symbol == alphabet.at(i))
		{
			return i;
		}
	}
	return 0;
}


/* Write bits in string */
void write_bits(bool bit, int bits_to_foll, string& output)  
{
	output.push_back(bit + 48);
	while (bits_to_foll > 0)
	{
		output.push_back(!bit + 48);
		bits_to_foll -= 1;
	}
}


/* Write bits in file */
void write_bits(bool bit, int bits_to_foll, fstream& output)
{
	output << bit;
	while (bits_to_foll > 0)
	{
		output << !bit;
		bits_to_foll -= 1;
	}
}


unsigned short int toShort(string s) {
	int i;
	unsigned short int value = 0;
	for (i = 0; i < 16; i++) 
	{
		if (i == s.size()) 
		{
			//value >>= i;
			return value;
		}
		if ('1' == s[i]) 
		{
			value = value | (1 << (15 - i));
		}
		//cout << bitset<16>(value) << endl;
	}
	return value;
}


unsigned short int getNewBit(char s) 
{
	//unsigned short int tmp = 0;
	if ('1' == s) 
	{
		return 1;
		//tmp = 1;
	}
	return 0;
}


string encode(string input, string alphabet, unsigned short int * frequency)
{
	//string alphabet = getAlpabet(input);
	//unsigned short int * frequency = getFrequency(input, alphabet);
	input.push_back('\0');
	int lenght = input.length();
	unsigned short int h[SIZE];
	unsigned short int l[SIZE];
	h[0] = 65535;
	l[0] = 0;
	unsigned int del = frequency[alphabet.size() - 1];

	string output;

	int bits_to_foll = 0;

	if(DEBUG_ENCODE)
	{
		cout << "Input: " << input << endl;
		cout << "Alphabet: " << alphabet << endl;
		cout << "Del: " << del << endl;
		cout << "Frequency:" << endl;
		for (int i = 0; i < alphabet.size(); ++i)
		{
			cout << "	" << alphabet.at(i) << " : " << frequency[i] << endl;
		}		 
		cout << "=============" << endl << endl;
	}

	for (int i = 0; i < lenght;)
	{
		int symbol = getSymbolIdx(input.at(i), alphabet);
		i += 1;

		unsigned int range = h[i - 1] - l[i - 1] + 1;
		l[i] = l[i - 1] + (range * frequency[symbol - 1]) / del;
		h[i] = l[i - 1] + (range * frequency[symbol]) / del - 1;

		if (DEBUG_ENCODE)
		{
			std::cout << "Symbol: " << alphabet.at(symbol) << endl;

			std::cout << "Frequency: " << frequency[symbol] - frequency[symbol - 1] <<
				" [" << frequency[symbol - 1] << "; "<< frequency[symbol]<< "]" << endl;
			std::cout << "[" << l[i] << "; " << h[i] << ")" << endl;
		}

		for (;;)
		{
			if (h[i] < HALF)
			{
				write_bits(0, bits_to_foll, output);
				bits_to_foll = 0;
			}
			else if (l[i] >= HALF)
			{
				write_bits(1, bits_to_foll, output);
				bits_to_foll = 0;
				//Нормализуем интервал, уменьшая значения границ, если они находятся во второй части рабочего интервала
				l[i] -= HALF;
				h[i] -= HALF;
			}
			else if (l[i] >= FIRST_QTR && h[i] < THIRD_QTR)
			{
				// Если текущий интеpвал  содеpжит сеpедину исходного, то вывод еще одного обpатного бита позже, а сейчас убpать общую часть
				bits_to_foll += 1;
				l[i] -= FIRST_QTR;
				h[i] -= FIRST_QTR;
			}
			else
			{
				break;
			}
			//расширяем рабочий интервал
			l[i] = 2 * l[i];
			h[i] = 2 * h[i] + 1;
			
			if (DEBUG_ENCODE) std::cout << "[" << l[i] << "; " << h[i] << ")" << endl;
		}
		if (DEBUG_ENCODE)
		{
			cout << "Output: " << output << endl;
			cout << "-------------" << endl;
		}
	}
	if (DEBUG_ENCODE)
	{
		for (int i = 0; i <= input.size(); ++i){
			std::cout << "[" << l[i] << "; " << h[i] << ")" << endl;
		}
		cout << "Output: " << output << endl;
	}

	return output;
}

string decode(string input, string alphabet, unsigned short int * frequency)
{
	if (DEBUG_DECODE)
	{
		cout << "This is decode: ";
	}
	unsigned short int h[SIZE];
	unsigned short int l[SIZE];
	l[0] = 0;
	h[0] = 65535;
	unsigned int del = frequency[alphabet.size() - 1];
	
	unsigned short int value;
	value = toShort(input);

	string output;
	bool FLAG = 0;
	int index = 16;
	for (int i = 1;; ++i)
	{
		unsigned int range = h[i - 1] - l[i - 1] + 1;
		unsigned int cum = ((value - l[i - 1] + 1) * del - 1) / range;

		int symbol;
		for (symbol = 1; frequency[symbol] <= cum; ++symbol) ;  //в цикле нашли индекс

		l[i] = l[i - 1] + (range * frequency[symbol - 1]) / del;
		h[i] = l[i - 1] + (range * frequency[symbol]) / del - 1;

		if (DEBUG_DECODE)
		{
			cout << "Symbol: " << alphabet.at(symbol) << endl;
			cout << "Symbol idx: " << symbol << endl;
			cout << "Value: " << value << endl;
			cout << "----------------" << endl;
		}
		if (alphabet.at(symbol) == '\0')
		{
			return output;
		}
		output.push_back(alphabet.at(symbol));
		//cout << alphabet.at(symbol);

		for (;;)
		{
			if (h[i] >= HALF)
			{
				if (l[i] >= HALF)
				{
					value -= HALF;
					l[i] -= HALF;
					h[i] -= HALF;
				}
				else if (l[i] >= FIRST_QTR && h[i] < THIRD_QTR)
				{
					value -= FIRST_QTR;
					l[i] -= FIRST_QTR;
					h[i] -= FIRST_QTR;
				}
				else
				{
					break;
				}
			}
			l[i] = 2 * l[i];
			h[i] = 2 * h[i] + 1;


			//TODO: what to do when input ends
			if (index < input.length())
			{
				value = 2 * value + getNewBit(input.at(index));
			}
			else
			{
				if (FLAG)
				{
					value = 2 * value + getNewBit('0');
				}
				else
				{
					value = 2 * value + getNewBit('1');
					FLAG = 1;
				}

			}

			//if (DEBUG_DECODE) cout << "Value before: " << bitset<16>(value) << endl;
			//value = 2 * value + getNewBit(input.at(index]));  //Добавляем еще 1 бит из файла
			//if (DEBUG_DECODE) cout << "Value after: " << bitset<16>(value) << endl;


			if (DEBUG_DECODE) std::cout << "[" << l[i] << "; " << h[i] << ")" << endl;
			index += 1;
			
		}
		//if (index >= input.length() + 16) //TODO: when stop?
		//{
		//	return output;
		//}


	}
}



void main()
{
	for (int i = 0; i < 10000; ++i)
	{
		char s[50] = { 0 };
		gen(s, 50);
		string input(s);

		string alphabet = getAlpabet(input);
		unsigned short int* frequency = getFrequency(input, alphabet);

		string encoded = encode(input, alphabet, frequency);
		string decoded = decode(encoded, alphabet, frequency);
		if (input != decoded)
		{
			cout << "step #" << i << endl;
			cout << input << endl;
			cout << decoded << endl;
			cout << "--------------" << endl;
		}
		else
		{
			cout << "step #" << i << ": GOOD!" << endl;
		}
	}



	//string input = "compressor";
	//string input = "baaa";
	//string input = "hello";
	//string input = "That method is better than Huffman";

	//string alphabet = getAlpabet(input);
	//unsigned short int* frequency = getFrequency(input, alphabet);

	//string encoded = encode(input);
	//cout << "Encoded: " << encoded << endl;

	//decode(encoded, alphabet, frequency);

	//std::cout << std::endl;
}
