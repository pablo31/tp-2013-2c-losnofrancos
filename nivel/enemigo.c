
#include "../libs/vector/vector2.h"

#include "enemigo.h"

void movimiento_permitido_enemigo(tad_nivel* nivel, tad_enemigo* self){
	if(list_size(nivel->personajes) > 0)
		atacar_al_personaje(self);
	else
		mover_en_L(self);

	nivel_gui_mover_item(self );
	nivel_gui_dibujar();
}

void atacar_al_personaje(tad_enemigo* self){
	nivel_gui_mover_item(self );
	nivel_gui_dibujar();
}

void mover_en_L(tad_enemigo* self){

	//dependiendo de la posicion que se encuentre en el mapa
	//tiene que mover al enemigo
	int deltax = 1;
	int deltay = 3;

	vector2 movimientos[8];
	movimientos[0] = vector2_new(deltax, deltay);
	movimientos[1] = vector2_new(-deltax, deltay);
	movimientos[2] = vector2_new(-deltax, -deltay);
	movimientos[3] = vector2_new(deltax, -deltay);
	movimientos[4] = vector2_new(deltay, deltax);
	movimientos[5] = vector2_new(-deltay, deltax);
	movimientos[6] = vector2_new(-deltay, -deltax);
	movimientos[7] = vector2_new(deltay, -deltax);

	vector2 pos = self->pos;

	int rows;
	int cols;
	nivel_gui_get_area_nivel(out rows, out cols);
	vector2 limites = vector2_new(cols, rows);

	int random = rand()%9; //TODO random 0,8

	vector2 nueva_pos = vector2_add(pos, movimientos[random]);
	while(nueva_pos.x < 0 || nueva_pos.y < 0 || nueva_pos.x > limites.x || nueva_pos.y > limites.y){
		vector2 nueva_pos = vector2_add(pos, movimientos[random]);
	}

	self->pos = nueva_pos;
	//TODO dibujar, etc
}

void mover_horizontal_izquierda(tad_enemigo* self){

}

void mover_horizontal_derecha(tad_enemigo* self){

}

void mover_vertical_izquierda(tad_enemigo* self){

}

void mover_vertidal_derecha(tad_enemigo* self){

}

