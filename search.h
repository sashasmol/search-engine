#ifndef SEARCH_H
#define SEARCH_H

struct searcheng {
	struct hashmap* map;
	int files_count;
	char** files;
};

struct searcheng* training(int num_buckets, char *files[], int files_count);
int rank(struct searcheng* srch_eng, char* query);
void stop_word(struct searcheng* srch_eng);
void se_destroy(struct searcheng* srch_eng);
char* read_query(void);
int read_number_of_buckets(void);
int get_df(struct hashmap* hm, char* word);
double get_idf(struct hashmap* hm, char* word, int tot_docs);

#endif
