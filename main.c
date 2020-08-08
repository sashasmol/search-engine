#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include "search.h"

int main(void)
{
	// creating an array for text files
	char* file[3];
	file[0] = "D1.txt";
	file[1] = "D2.txt";
	file[2] = "D3.txt";

	// opening statement
	printf("-SEARCH ENGINE PROGRAM-\n");

	// user is prompted to input number of buckets
	int bucketNumber = read_number_of_buckets();

	// creating hash map consisting of linked lists
	struct searcheng* srch_eng = training(bucketNumber, file, 3);

	// user is prompted with a message to input s or x
	char sx;
	printf("Enter 'S' for SEARCH or 'X' for EXIT: ");
	// while loop used to continuously ask for s or x
	while (1)
	{
		// scanf used to obtain input from user
		scanf("%c", &sx);
		// if x is inputted, program ends
		if (sx == 'X' || sx == 'x') break;
		// if s is inputted, the user should be called to input a search query
		else if (sx == 'S' || sx == 's')
		{
			// continuation of while loop prompting search or exit
			scanf("%c", &sx);
			// call query function to read query in from user
			char* query = read_query();
			// rank calculated for inputted query using the hashmap
			rank(srch_eng, query);
			// free the memory used for the inputted search query
			free(query);
			// repeat cycle
			printf("Enter 'S' for SEARCH or 'X' for EXIT: ");
		}
	}
	// clear hashmap memory once x is pressed
	se_destroy(srch_eng);
	return -1;
}
