/*
 * round.c
 *
 *  Created on: Oct 15, 2013
 *      Author: pablo
 */

#include "../common.h"

#include "round.h"


t_round* round_create(){
	alloc(ret, t_round);
	ret->list = list_create();
	ret->pointer = 0;
	ret->auto_reverse = 0;
	return ret;
}

void round_add(t_round* round, void* element){
	list_add(round->list, element);
}

void* round_get(t_round* round){
	return list_get(round->list, round->pointer);
}

void* round_remove(t_round* round){
	return list_remove(round->list, round->pointer);
}

int round_size(t_round* round){
	return list_size(round->list);
}

void round_dispose(t_round* round){
	list_destroy(round->list);
	dealloc(round);
}

void round_set_autoreverse(t_round* round, int value){
	round->auto_reverse = value;
}

void round_forward(t_round* round){
	int index = round->pointer + 1;
	int size = list_size(round->list);
	if(index >= size){
		if(round->auto_reverse)
			round->pointer = 0;
		else
			round->pointer = size;
	}else
		round->pointer = index;
}

void round_restart(t_round* round){
	round->pointer = 0;
}

int round_has_ended(t_round* round){
	return round->pointer >= list_size(round->list);
}
