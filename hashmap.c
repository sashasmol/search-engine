#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashmap.h"

// creates hashmap
struct hashmap* hm_create(int num_buckets) {
	// allocate space for a new hashmap
	struct hashmap* new = (struct hashmap*) malloc(sizeof(struct hashmap));
	// allocate space for hashmap buckets
	new->buckets = (struct lwordnode**) malloc(sizeof(struct llnode*) * num_buckets);
	// point new map to buckets
	new->num_buckets = num_buckets;
	// clear the newly created map
	for (int i = 0; i < num_buckets; i++) {
		new->buckets[i] = NULL;
	}
	// the function returns the newly allocated hashmap
	return new;
}

// returns value associated with word input
int hm_get(struct hashmap* hm, char* word, char* document_id) {
	// create hash bucket
	int bucket = hash(hm, word);
	// creat lwordnode pointer
	struct lwordnode* head = hm->buckets[bucket];

	// if the map is empty, return 0
	// check if whole map is empty
	if (hm == NULL) {
		return 0;
	// check if head is empty
	} else if (head == NULL) {
		return 0;
	} else {
		/* while the head is not NULL, compare the word and the file name
		 if they are equal, return the number of occurrences
		 count occurrences
		 create lwordnode pointer and assign to head */
		struct lwordnode* current = head;
		// unti list is empty
		while (current != NULL) {
			// if the words are equal, check docid of the word
			if (strcmp(current->word, word) == 0) {
				struct ldocnode* doc_node_ptr = current->docnode;
				// until list is empty
				while (doc_node_ptr != NULL) {
					// if the docids are equal, term frequency is increased
					if (strcmp(doc_node_ptr->document_id, document_id) == 0) {
						return doc_node_ptr->tf;
					}
					doc_node_ptr = doc_node_ptr->next;
				}
			}
			// assign head to next element.
			current = current->next;
		}
		return 0;
	}
}

// created method to initialize a node for a word
// using word parameter
struct lwordnode* initWordNode(char* word) {
	struct lwordnode* node;
	//allocate space for new node
	node = (struct lwordnode*) malloc(sizeof(struct lwordnode));
	//copy word's string, must free after using
	node->word = strdup(word);
	node->df = 1;
	node->docnode = NULL;
	node->next = NULL;
	return node;
}

// created method initialize a node for a document
// using doc_id parameter
struct ldocnode* initDocumentNode(char* document_id) {
	struct ldocnode* node;
	//allocate space for new node
	node = (struct ldocnode*) malloc(sizeof(struct ldocnode));
	//copy doc_id's string, don't forget to free
	node->document_id = strdup(document_id);
	node->tf = 1;
	node->next = NULL;
	return node;
}

// put key value pair into the hashmap
void hm_put(struct hashmap* hm, char* word, char* document_id) {
	// create variable to hold assigned bucket from hash function
	int bucket = hash(hm, word);
	// head points to specific bucket
	struct lwordnode* head = hm->buckets[bucket];

	// create two pointers for word
	// current & previous
	// set them equal to the head of the bucket
	struct lwordnode* c_wordnode = head;
	struct lwordnode* p_wordnode = head;

	// create while loop that will parse until current word node is NOT null
	while (c_wordnode != NULL) {
		// compare the curred word to the word inside the bucket
		if (strcmp(c_wordnode->word, word) == 0) {
			// create two pointers for current doc and previous doc
			// if the words are equal, set current docnode to current wordnode's document id
			struct ldocnode* c_docnode = c_wordnode->docnode;
			struct ldocnode* p_docnode = c_docnode;
			// while loop that will parse until current docnode is null
			while (c_docnode != NULL) {
				// check if the current docid matches the docid inputted
				if (strcmp(c_docnode->document_id, document_id) == 0) {
					// if so, increase term frequency
					c_docnode->tf++;
					return;
				}
				// set current docnode to previous, and point current docnode to next
				p_docnode = c_docnode;
				c_docnode = c_docnode->next;
			}
			// if the document id is not found, create new document node
			c_docnode = initDocumentNode(document_id);
			// add new docnode to the doc's list by setting previous to next = to current
			p_docnode->next = c_docnode;
			// increase document frequency
			c_wordnode->df++;
			return;
		}
		// set current wordnode to next
		p_wordnode = c_wordnode;
		c_wordnode = c_wordnode->next;
	}
	// add wordnode to the end of list
	c_wordnode = initWordNode(word);
	// add new doc's node by creating new docnode
	c_wordnode->docnode = initDocumentNode(document_id);
	// if prev word node is not null,
	// point previous to next (current)
	if (p_wordnode != NULL) {
		p_wordnode->next = c_wordnode;
	} else {
		// otherwise, assign the bucket to the current node
		hm->buckets[bucket] = c_wordnode;
	}
}

// remove key value pair
void hm_remove(struct hashmap* hm, char* word) {
	int bucket = hash(hm, word);
	// create three pointers: head, current, and previous
	// assign all to the head
	struct lwordnode* head = hm->buckets[bucket];
	struct lwordnode* current = head;
	struct lwordnode* previous = head;

	// while loops which loops until the current wordnode is empty (null)
	while (current != NULL) {
		// if the words/text files are equal; break
		if (strcmp(current->word, word) == 0) {
			break;
		}
		// iterate nodes
		previous = current;
		current = current->next;
	}

	// if the current node is null, then there is nothing to remove
	if (current == NULL) {
		return;
	}

	// create docnode which will be used to delete doc nodes
	struct ldocnode* doc_node_ptr = current->docnode;
	// loops until reached end of docnodes
	while (doc_node_ptr != NULL) {
		// create temporary docnode pointer and set it equal to doc node pointer
		struct ldocnode* tmp_node_ptr = doc_node_ptr;
		// temp becomes current and doc node ptr becomes next
		doc_node_ptr = doc_node_ptr->next;
		// free memory (clear) of current node and free the pointer
		free(tmp_node_ptr->document_id);
		free(tmp_node_ptr);
	}

	// delete word node
	if (current == hm->buckets[bucket]) {
		// next element becomes the head since original head was removed
		hm->buckets[bucket] = current->next;
	} else {
		previous->next = current->next;
	}
	// free each wordnode one by one through iteration
	free(current->word);
	// free pointer
	free(current);
}

// destroy the hashmap and free all memory
void hm_destroy(struct hashmap* hm) {
	int i;
	// iterate through all the buckets
	for (i = 0; i < hm->num_buckets; i++) {
		// create pointer used for iterationg through wordnodes
		struct lwordnode* word_ptr = hm->buckets[i];
		// destroy each element individually until list is empty
		while (word_ptr != NULL) {
			// delete doc nodes
			// create pointer used for iterating through docnodes
			struct ldocnode* doc_node_ptr = word_ptr->docnode;
			while (doc_node_ptr != NULL) {
				// create temp doc node used as current
				struct ldocnode* tmp_node_ptr = doc_node_ptr;
				doc_node_ptr = doc_node_ptr->next;
				// free string
				free(tmp_node_ptr->document_id);
				// free node struct memory
				free(tmp_node_ptr);
			}
			// create temp word node used as current
			struct lwordnode*  word_temp_ptr = word_ptr;
			word_ptr = word_ptr->next;
			// free string
			free(word_temp_ptr->word);
			// free node struct memory
			free(word_temp_ptr);
		}
	}
	// after all the nodes are freed, free the buckets
	// free map using map pointer
	free(hm->buckets);
	// after everything else is freed, free the entire hashmap
	free(hm);
}

// sum ascii values & mod by number of buckets
// determines specific bucket word belongs in
int hash(struct hashmap* hm, char* word) {
	char* ptr;
	int sum = 0;
	// summing ASCII values of inputted string characters
	for (ptr = word; *ptr != '\0'; ptr++) {
		int value = (int)*ptr;
		sum = sum + value;
	}
	// sum mod by number of buckets in order to assure the
	// returnBucket is in bounds
	int returnBucket = sum % (hm->num_buckets);
	return returnBucket;
}
