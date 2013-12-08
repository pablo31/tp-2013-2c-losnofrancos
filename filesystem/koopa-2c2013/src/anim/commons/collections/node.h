#ifndef NODE_H_
#define NODE_H_

	struct link_element{
		void *data;
		struct link_element *next;
	};
	typedef struct link_element t_link_element;

	struct double_link_element{
		struct double_link_element *previous;
		void *data;
		struct double_link_element *next;
	};
	typedef struct double_link_element t_double_link_element;

	struct hash_element{
		char *key;
		unsigned int hashcode;
		void *data;
		struct hash_element *next;
	};
	typedef struct hash_element t_hash_element;

#endif /*NODE_H_*/
