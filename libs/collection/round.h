/*
 * round.h
 *
 *  Created on: Oct 15, 2013
 *      Author: pablo
 */

#ifndef ROUND_H_
#define ROUND_H_

#include "../common/collections/list.h"

struct s_round{
	t_list* list;
	int pointer;
	int auto_reverse;
};
typedef struct s_round t_round;


t_round* round_create();
void round_add(t_round* round, void* element);
void* round_get(t_round* round);
void* round_remove(t_round* round);
int round_size(t_round* round);
int round_is_empty(t_round* round);
void round_dispose(t_round* round);

void round_set_autoreverse(t_round* round, int value);
void round_forward(t_round* round);
void round_restart(t_round* round);
int round_has_ended(t_round* round);


#endif /* ROUND_H_ */
