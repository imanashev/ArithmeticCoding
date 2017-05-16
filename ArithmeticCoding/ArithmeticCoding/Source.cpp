#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;

#define EOF_SYMBOL '-'
#define SIZE 255
#define DEBUG 1

#define FIRST_QTR 65535 / 4 + 1
#define HALF  2 * FIRST_QTR
#define THIRD_QTR  3 * FIRST_QTR

string getAlpabet(string input)
{
	string alphabet;
	alphabet.push_back(EOF_SYMBOL);
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

int* getFrequency(string input, string alphabet)
{
	int* frequency = new int[alphabet.size()];
	for (int i = 0; i < alphabet.size(); ++i)
	{
		frequency[i] = 0;
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
void write_bits1(bool bit, int bits_to_foll, string& output)  
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


void encode(string input)
{
	string alphabet = getAlpabet(input);
	int * frequency = getFrequency(input, alphabet);
	int lenght = input.length();
	int h[SIZE];
	int l[SIZE];
	h[0] = 65535;
	l[0] = 0;
	int del = frequency[alphabet.size() - 1];

	fstream output("output.bin", ios_base::out | ios_base::binary);
	//string output;

	int bits_to_foll = 0;

	/* Debug part */
	if(DEBUG)
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

		int range = h[i - 1] - l[i - 1] + 1;
		l[i] = l[i - 1] + (range * frequency[symbol - 1]) / del;
		h[i] = l[i - 1] + (range * frequency[symbol]) / del - 1;

		/* Debug part */
		{
			std::cout << "Symbol: " << alphabet.at(symbol) << input.at(i - 1) << endl;

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
			
			if (DEBUG) std::cout << "[" << l[i] << "; " << h[i] << ")" << endl;
		}
		if (DEBUG) cout << "-------------" << endl;
	}
	/* Debug part */
	if (DEBUG)
	{
		/*for (int i = 0; i < input.size(); ++i){
			std::cout << "[" << l[i] << "; " << h[i] << ")" << endl;
		}
		cout << "Output: " << output << endl;*/
		
	}
}


//	value - в этой переменной считываем 16 бит из сжатого файла
void decode(string filename, string alphabet, int * frequency)
{
	int h[SIZE];
	int l[SIZE];
	l[0] = 0;
	h[0] = 65535;
	int del = frequency[alphabet.size() - 1];


	/* Debug part */
	if (DEBUG)
	{
		cout << "This is decode" << endl;
	}
	
	fstream input(filename, ios_base::in | ios_base::binary);
	unsigned short int value;
	input.read((char *)&value, sizeof(unsigned short int));

	for (int i = 1; i < 10; ++i) // ??? WHEN STOP
	{
		int range = h[i - 1] - l[i - 1] + 1;
		int cum = (((value - l[i - 1]) + 1) * del - 1) / range;

		int symbol;
		for (symbol = 1; frequency[symbol] > cum; ++symbol) {};  //в цикле нашли индекс

		l[i] = l[i - 1] + (range * frequency[symbol - 1]) / del - 1;
		h[i] = l[i - 1] + (range * frequency[symbol]) / del;

		/* Debug part */
		if (DEBUG)
		{
			cout << "Symbol: "  << alphabet.at(symbol) << endl;
			cout << "Value: " << value << endl;
		}
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
				else if(l[i] >= FIRST_QTR && h[i] < THIRD_QTR)
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
			
			//TODO
			unsigned short int newBit;
			input.read((char *)&newBit, sizeof(bool));
			value = 2 * value + newBit;  //Добавляем еще 1 бит из файла
		}
	}
}



void main()
{
	string input = "compressor";
	//string input = "That method is better than Huffman";

	encode(input);

	string alphabet = getAlpabet(input);
	int* frequency = getFrequency(input, alphabet);

	decode("output.bin", alphabet, frequency);

	std::cout << std::endl;
}
