
#include "../libs/vector/vector2.h"
#include <unistd.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "enemigo.h"



vector2 buscar_Personaje_Mas_Cercano(tad_nivel* nivel, tad_enemigo* self);

void movimiento_permitido_enemigo(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* nivel, tad_enemigo* self);

	logger_info(get_logger(nivel), "ENEMIGO: Se cargo el enemigo %c:",self->simbolo);

	srand(time(NULL)); //seed para random
	//teniendo en cuenta que no:
		//salga del mapa
		//pase por arriba de una caja

	if(list_size(nivel->personajes) > 0)
			atacar_al_personaje(nivel,self);
		else{
			logger_info(nivel->logger, "ENEMIGO: Nivel sin enemigos.");
			mover_en_L(nivel,self);
		}

}

void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self){
	//se carga la posicion del personaje que esta mas cerca.
	vector2 posicion_personaje;
		posicion_personaje= buscar_Personaje_Mas_Cercano(nivel,self);

		vector2 nuevaPosicion = vector2_next_step(self->pos,posicion_personaje);

		self->pos= nuevaPosicion;


	nivel_gui_dibujar(nivel);
}

vector2 buscar_Personaje_Mas_Cercano(tad_nivel* nivel,tad_enemigo* self){

	vector2 masCerca;
	vector2 masCercaBase;
	vector2 maslejos = vector2_new(20,20);
	int x =1;


	foreach(personaje, nivel->personajes, tad_personaje*){
		// hacer un burbujeo de enemigos
		//caso base, se compara el enemigo primero de la lista con un vector maximo (20,20) => levanta enemigo
		//si hay mas de uno, compara el primero con el siguiente y se queda con el mejor


		//con el primer enemigo
		if (x==1){
			masCercaBase = vector2_minimize(maslejos,personaje->pos);
			masCerca.x = masCercaBase.x;
			masCerca.y = masCercaBase.y;
			x = x+1;
		}else	masCerca = vector2_minimize(masCercaBase,personaje->pos);

	}

	return masCerca;
}

//logger_info(nivel->logger, "posicon enemigo (%d:%d)", self->pos.x,self->pos.y);
void mover_en_L(tad_nivel* nivel, tad_enemigo* self){

	int rows;
	int cols;
	nivel_gui_get_area_nivel(out rows, out cols);
	vector2 limite_mapa = vector2_new(cols, rows);

	int random = rand()%9; //random 0,8

	int completo_movimiento_L=1;
	int cantidad_movimiento=3;

	while(1){

		while(cantidad_movimiento>0){

					if (completo_movimiento_L){
						vector2 nueva_pos = vector2_move_in_L(self->pos,random,cantidad_movimiento);

						//si la posicion esta dentro del mapa se grafica, sino no, contando el movimiento en L
						if(vector2_within_map(nueva_pos, limite_mapa)){
							cantidad_movimiento --;
							self->pos = nueva_pos;
							sleep(1);
							//usleep(nivel->sleep_enemigos * 1000);
							nivel_gui_dibujar(nivel);
						}else{
							//sino completo la L, tiene que buscar otra L azar
							cantidad_movimiento=0;
							completo_movimiento_L =0;
						}

					}
			}
		completo_movimiento_L=1;
		cantidad_movimiento=3;
	}


}

	//dependiendo de la posicion que se encuentre en el mapa
	//tiene que mover al enemigo
    /*
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

	int random = rand()%9; //random 0,8

	vector2 nueva_pos;

	nueva_pos = vector2_add(pos, movimientos[random]);
	while(vector2_within_map(nueva_pos, limite_mapa)){
		nueva_pos = vector2_add(pos, movimientos[random]);
		nivel_gui_dibujar(nivel);
		usleep(nivel->sleep_enemigos * 1000);
	}

	self->pos = nueva_pos;
*/

