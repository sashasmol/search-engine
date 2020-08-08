#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashmap.h"
#include "search.h"

// creating the hashmap array with associate linked lists docids and words
// "training process" done before searching for query
struct searcheng* training(int num_buckets, char *files[], int files_count) {
	struct searcheng* srch_eng = (struct searcheng*) malloc(sizeof(struct searcheng));
	// create a hashmap using the number of buckets inputted
	struct hashmap* hm = hm_create(num_buckets);

	FILE* f;
	char word[1024];

	// parsing through the files
	for (int i = 0; i < files_count; i++) {
		// open files individually; D1-D3
		f = fopen(files[i], "r");
		// if the file is empty, move on
		if (f == NULL) {
			continue;
		}

		// otherwise, put word in designated bucket word by word
		while (fscanf(f, "%1023s", word) == 1) {
			// put word in specified bucket using put function within hashmap.c
			hm_put(hm, word, files[i]);
		}
		// close file when file end is reached
		fclose(f);
	}
	srch_eng->files_count = files_count;
	srch_eng->map = hm;
	srch_eng->files = files;

	// call stop word function to check for stop words
	stop_word(srch_eng);
	// return the created array of buckets with linked lists of docids and words
	return srch_eng;
}

// calculating the number of documents that have the word
int get_df(struct hashmap* hm, char* word) {
	int bucket = hash(hm, word);
	// create lwordnode pointer to iterate through list
	struct lwordnode* current = hm->buckets[bucket];
	// until we reach the end, compare the words
	// incrementing document frequency when word occurs in documents
	while (current != NULL) {
		if (strcmp(current->word, word) == 0) {
			return current->df;
		}
		// iterate through nodes
		current = current->next;
	}
	return 0;
}

// function used to calculate inverse document frequency
// using the hashmap, the word, and total number of documents
double get_idf(struct hashmap* hm, char* word, int tot_docs) {
	// calculate document frequency using get document frequency function
	int df = get_df(hm, word);
	// if document frequency is 0, then inverse document frequency is also 0
	if (df == 0) {
		return 0;
	}
	// idf = log (number of documents/ (1 + document frequency))
	// since we started at 0, +1 is removed
	return (double) log((double)tot_docs / df);
}

// rank function used to calculate relevancy of documents
int rank(struct searcheng* srch_eng, char* query) {
	// malloc space for the rank
	double* rank = malloc(sizeof(double)*srch_eng->files_count);
	// clear rank array
	for (int i = 0; i < srch_eng->files_count; i++) {
		rank[i] = 0;
	}
	char* word;

	// checks for spaces throughout query to separate words
	word = strtok(query, " ");

	// until all words are tested, continue loops
	while (word != NULL) {
		// call get_idf function to calculate inverse document frequency for specific word
		double idf_w = get_idf(srch_eng->map, word, srch_eng->files_count);
		// if the idf is not 0
		if (idf_w != 0) {
			// iterate through files
			for (int i = 0; i < srch_eng->files_count; i++) {
				// calculate word occurrence using (term frequency) using get function
				int occurs = hm_get(srch_eng->map, word, srch_eng->files[i]);
				// if the term occurs more than once, calculate the rank
				// rank is calculated by multiplying the term frequency (occurrences) by idf
				if (occurs > 0) {
					rank[i] += occurs * idf_w;
				}
			}
		}
		word = strtok(NULL, " ");
	}

	// create pointer array for rank index
	int* rank_index = malloc(sizeof(int) * srch_eng->files_count);
	int non_zero_rank_cnt = 0;
	// iterate through files
	for (int i = 0; i < srch_eng->files_count; i++) {
		// if the document rank is greater than 1
		// the document is relevant
		if (rank[i] > 0) {
			rank_index[non_zero_rank_cnt++] = i;
		}
	}

	// sort document'indexes by rank (descending)
	// most relevant to least relevant
	for (int i = 0; i < non_zero_rank_cnt; i++) {
		for (int j = i + 1; j < non_zero_rank_cnt; j++) {
			if (rank[rank_index[i]] < rank[rank_index[j]]) {
				// if index is less than, swap elements
				// current becomes prev
				// and next becomes current
				int index = rank_index[i];
				rank_index[i] = rank_index[j];
				rank_index[j] = index;
			}
		}
	}

	// if there is no rank, there are no matches
	if (non_zero_rank_cnt == 0) {
		printf("No files match!\n");
	} else {
		printf("File Matches (Descending Order): \n");
		// print documents in descending order
		for (int i = 0; i < non_zero_rank_cnt; i++) {
			int document_index = rank_index[i];
			printf("%s's rank is %f\n", srch_eng->files[document_index], rank[document_index]);
		}
	}
	// free malloced memory
	free(rank_index);
	free(rank);
	return non_zero_rank_cnt;
}

// eliminating repeating words (words common to all documents)
void stop_word(struct searcheng* srch_eng) {
	// iterate through buckets
	for (int i = 0; i < srch_eng->map->num_buckets; i++) {
		// create two pointers for current and next wordnode
		// set current lwordnode to bucket (0-i)
		struct lwordnode* current = srch_eng->map->buckets[i];
		struct lwordnode* next = NULL;
		// while loop that runs until current node is null
		while (current != NULL) {
			next = current->next;
			// if document frequency of the word is equal to the number of files
			// the word exists in all documents, and thus, a stop word
			if (current->df == srch_eng->files_count) {
				// delete current wordnode
				hm_remove(srch_eng->map, current->word);
			}
			// iterate through nodes within buckets
			current = next;
		}
	}
}

// used to free hashmap memory
void se_destroy(struct searcheng* srch_eng) {
	// destroy hashmap using destroy function
	hm_destroy(srch_eng->map);
	// free search engine's memory
	free(srch_eng);
}

// used for user to input their desired number of buckets
int read_number_of_buckets(void) {
	int result;
	printf("Please enter the number of buckets\n");
	// use scanf to retrieve integer input from user
	scanf("%d", &result);
	return result;
}

// used for user to input their desired search query
char* read_query(void) {
	#define MAX 200
	char buf[MAX];
	// allow user to input their desired search query
	printf("Please enter your search query:\n");
	fgets(buf, MAX - 1, stdin);
	// remove '\n'
	int i = -1;
	// while the string character is not at the end of the string
	while (buf[++i] != '\0') {
		// when enter is hit, string input has ended
		if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		}
	}
	// returns copied user inputted search query
	return strdup(buf);
}
