#ifndef HASHMAP_H
#define HASHMAP_H

// node for storing word reference
struct lwordnode {
	char* word;
	int df;
	struct lwordnode* next;
	struct ldocnode* docnode;
};

// node for storing document reference
struct ldocnode {
	char* document_id;
	int tf;
	struct ldocnode* next;
};

struct hashmap {
	struct lwordnode** buckets;
  int num_buckets;
};

struct hashmap* hm_create(int num_buckets);
int hm_get(struct hashmap* hm, char* word, char* document_id);
struct lwordnode* initWordNode(char* word);
struct ldocnode* initDocumentNode(char* document_id);
void hm_put(struct hashmap* hm, char* word, char* document_id);
void hm_remove(struct hashmap* hm, char* word);
void hm_destroy(struct hashmap* hm);
int hash(struct hashmap* hm, char* word);

#endif
