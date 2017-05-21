#include "ArithmeticCoding.h"

using namespace std;

#define LENGTH 50 // lenght of generated stirng
#define N 10000 // count of tests

/* Generate random char[n] */
void gen(char* p, size_t n)
{
	while (n--)
	{
		char ch;
		while ((ch = (char)(rand() % (127 - '0' + 1) + '0')), !isalnum(ch));
		*p++ = ch;
	}
	*p = 0;
}


void main()
{
	/* Tests */
	for (int i = 0; i <= N; ++i)
	{
		/* Generate string */
		char s[LENGTH + 1] = { 0 };
		gen(s, LENGTH);
		string input(s);

		/* Get alphabet and frequency*/
		string alphabet = getAlpabet(input);
		int* frequency = getFrequency(input, alphabet);

		/* Encode and decode generated string */
		string encoded = encode(input, alphabet, frequency);
		string decoded = decode(encoded, alphabet, frequency);

		/* Compare result */
		if (input == decoded)
		{
			cout << "step #" << i << ": GOOD!" << endl;
		}
		else
		{
			cout << "step #" << i << ": BAD!" << endl;
			cout << "	" << input << endl;
			cout << "	" << decoded << endl;
		}
	}
}
