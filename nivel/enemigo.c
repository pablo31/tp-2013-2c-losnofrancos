 
#include "enemigo.h"

void movimiento_permitido_enemigo(tad_enemigo* self){


	if(self->detecta_personaje){
		atacar_al_personaje(self);
	}else
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
	mover_horizontal_izquierda(self);
	mover_horizontal_derecha(self);
	mover_vertical_izquierda(self);
	mover_vertidal_derecha(self);

}

void mover_horizontal_izquierda(tad_enemigo* self){

}

void mover_horizontal_derecha(tad_enemigo* self){

}

void mover_vertical_izquierda(tad_enemigo* self){

}

void mover_vertidal_derecha(tad_enemigo* self){

}

