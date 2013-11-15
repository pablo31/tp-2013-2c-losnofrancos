
#include "../libs/vector/vector2.h"
#include "nivel_ui.h"
#include "enemigo.h"

void movimiento_permitido_enemigo(tad_nivel* nivel, tad_enemigo* self){


	if(list_size(nivel->personajes) > 0)
		atacar_al_personaje(nivel,self);
	else
		mover_en_L(self);

	//supuestamente aca se dibuja el personaje
	nivel_gui_dibujar();
}

void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self){
	//se carga la posicion del personaje que esta mas cerca.
	//vector2 posicion_personaje = personaje_Mas_Cerca(nivel,self);
	//self->posicion_personaje = posicion_personaje;
	//TODO arreglar el esto

	self->posicion_personaje.x=2;
	self->posicion_personaje.y=2;

	self->pos.x = self->pos.x +1;
	self->pos.y = self->pos.y +1;

}

void mover_en_L(tad_enemigo* self){

	//dependiendo de la posicion que se encuentre en el mapa
	//tiene que mover al enemigo
	int deltax = 1;
	int deltay = 2;

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
	vector2 limite_mapa = vector2_new(cols, rows);

	int random = rand()%9; //TODO random 0,8

	vector2 nueva_pos;

	nueva_pos = vector2_add(pos, movimientos[random]);
	while(vector2_within_map(nueva_pos, limite_mapa)){
		nueva_pos = vector2_add(pos, movimientos[random]);
	}

	self->pos = nueva_pos;

}




