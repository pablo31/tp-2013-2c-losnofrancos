#ifndef BLOCK_H_
#define BLOCK_H_

	#include "../../i_drawable.h"

	typedef struct {
		implements_i_drawable
	} t_block;

//	 __
//	|_||
//	||_|

	t_block* block_create();
	void block_makeLava(t_block* this);
	void block_breakBridge(t_block* this);

#endif
