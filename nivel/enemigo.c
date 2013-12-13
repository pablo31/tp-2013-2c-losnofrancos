#include "../libs/vector/vector2.h"
#include <unistd.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "enemigo.h"



void enemigo_ia(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* nivel, tad_enemigo* self);
	var(logger, self->logger);

	logger_info(logger, "Posicion inicial: (%d,%d)", self->pos.x, self->pos.y);

	srand(time(NULL)); //seed para random

	int eje_prox_mov = 1;
	int cantidad_personajes;

	while(true){
		mutex_close(nivel->semaforo_personajes);
		mutex_close(nivel->semaforo_enemigos);


		cantidad_personajes = list_size(nivel->personajes);

		//logger_info(logger, "Cantidad de personajes en el nivel: %d.", cantidad_personajes);

		if(cantidad_personajes > 0){
			atacar_al_personaje(nivel, self, &eje_prox_mov);
			self->movimientos_restantes = 0;
		}else{
			if(!self->movimientos_restantes){
				self->movimiento_random = rand()%8; //random 0,7
				self->movimientos_restantes = 3;
			}
			moverse_sin_personajes(nivel, self);
		}


		mutex_open(nivel->semaforo_enemigos);
		mutex_open(nivel->semaforo_personajes);


		nivel_gui_dibujar(nivel);

		usleep(nivel->sleep_enemigos * 1000);
	}
}




private void enemigo_mover_a(tad_nivel* nivel, tad_enemigo* self, vector2 pos){
	//if(posicion_ocupada_por_enemigo(nivel, pos)) return;
	if(vector2_equals(vector2_new(0,0), pos)) return;
	self->pos = pos;
}


void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self, int *eje_prox_mov){
	logger_info(self->logger, "Posicion actual: (%d,%d)", self->pos.x, self->pos.y);

	//se carga la posicion del personaje que esta mas cerca
	tad_personaje* blanco = buscar_personaje_mas_cercano(nivel, self);
	//logger_info(self->logger, "Voy a por %s", blanco->nombre);

    logger_info(self->logger, "Personaje a atrapar %s, posicion del personaje: (%d,%d)", blanco->nombre, blanco->pos.x, blanco->pos.y);

	//controla si el personaje no está en la misma posicion que el enemigo
	if(!vector2_equals(self->pos, blanco->pos)){
		vector2 nueva_posicion = vector2_move_alternately(self->pos, blanco->pos, eje_prox_mov);

		if(posicion_ocupada_por_caja(nivel, nueva_posicion))
			nueva_posicion = esquivar_posicion(self->pos, nueva_posicion, blanco->pos);

		enemigo_mover_a(nivel, self, nueva_posicion);
	}

	//controla si la posicion nueva del enemigo coincide con la del personaje
	if (vector2_equals(self->pos, blanco->pos)){
		muerte_del_personaje(blanco->simbolo, nivel, ENEMIGO);
		logger_info(self->logger, "El personaje %s fue atrapado por un enemigo", blanco->nombre);
	}
}



tad_personaje* buscar_personaje_mas_cercano(tad_nivel* nivel, tad_enemigo* self){
	tad_personaje* blanco;
	int distancia_del_enemigo;
	int distancia_aux;
    bool sin_objetivo = true;

    foreach(personaje, nivel->personajes, tad_personaje*){
    	//logger_info(self->logger, "Entre el foreach! El primero en ser iterado es %s", personaje->nombre);
    	if (sin_objetivo){
    		distancia_del_enemigo = vector2_distance_to(self->pos, personaje->pos);
			blanco = personaje;
			sin_objetivo = false;
	   }else{
		   distancia_aux = vector2_distance_to(self->pos, personaje->pos);
		   if (distancia_aux < distancia_del_enemigo){
			   distancia_del_enemigo = distancia_aux;
			   blanco = personaje;
		   }
	   }
    }
    //logger_info(self->logger, "Sali el foreach! El blanco es %s", blanco->nombre);
    logger_info(self->logger, "Personaje mas cercano: %s", blanco->nombre);

    return blanco;
}


void moverse_sin_personajes(tad_nivel* nivel, tad_enemigo* self){
	int rows;
	int cols;
	nivel_gui_get_area_nivel(out rows, out cols);
	vector2 limite_mapa = vector2_new(cols, rows);
	vector2 nueva_pos;

	nueva_pos = movimiento_random(self->pos, self->movimiento_random, self->movimientos_restantes);

	//si la posicion esta dentro del mapa se grafica
	if(vector2_within_map(nueva_pos, limite_mapa)){
		if (!posicion_ocupada_por_caja(nivel,nueva_pos)){
			self->movimientos_restantes--;
			enemigo_mover_a(nivel, self, nueva_pos);
		}else{
			//si no completo la L por esquivar una caja tiene que empezar de nuevo y buscar otra L al azar
			self->movimientos_restantes = 0;
			logger_info(self->logger, "Se esquiva caja en (%d,%d)", nueva_pos.x, nueva_pos.y);
		}
	}else{
		//sino completo la L porque se iba fuera del mapa tiene que empezar de nuevo y buscar otra L al azar
		self->movimientos_restantes = 0;
		logger_info(self->logger, "Se esquiva borde en (%d,%d)", nueva_pos.x, nueva_pos.y);
	}
}




bool posicion_ocupada_por_caja(tad_nivel* nivel, vector2 pos){
	//buscamos cajas en la misma posicion
	foreach(caja, nivel->cajas, tad_caja*)
		if(vector2_equals(pos, caja->pos))
			return true;
	return false;
}
bool posicion_ocupada_por_enemigo(tad_nivel* nivel, vector2 pos){
	//buscamos enemigos en la misma posicion
	foreach(enemigo, nivel->enemigos, tad_enemigo*)
		if(vector2_equals(pos, enemigo->pos))
			return true;
	return false;
}





vector2 esquivar_posicion(vector2 posicion_actual, vector2 nueva_posicion, vector2 posicion_personaje){
    vector2 posicion_alternativa;
    int movimiento_incorrecto;

    //para esquivar la caja analiza en que direccion se iba a mover y se calcula una posicion alternativa
    //moviendose en el eje contrario, se elije el sentido segun la posicion del personaje
	movimiento_incorrecto = calcular_direccion_movimiento(posicion_actual, nueva_posicion);

    switch(movimiento_incorrecto){

           case ARRIBA:
           case ABAJO:
        	   if (posicion_actual.x > posicion_personaje.x)
        		   //se mueve hacia la izquierda
        		   posicion_alternativa = vector2_add_x(posicion_actual, -1);
        	   else
        		  //se mueve hacia la derecha
        		   posicion_alternativa = vector2_add_x(posicion_actual, 1);

        	   break;

           case IZQUIERDA:
           case DERECHA:
        	   if (posicion_actual.y > posicion_personaje.y)
        		   //se mueve  hacia arriba
        		   posicion_alternativa = vector2_add_y(posicion_actual, -1);
        	   else
        		   //se mueve hacia abajo
        		   posicion_alternativa = vector2_add_y(posicion_actual, 1);

        	   break;
     	}
	return posicion_alternativa;
}


int calcular_direccion_movimiento(vector2 pos1, vector2 pos2){
	if (pos1.x != pos2.x){
		if (pos1.x < pos2.x)
			return DERECHA;
		else
			return IZQUIERDA;
		}
	else {
		if (pos1.y < pos2.y)
			return ARRIBA;
		else
			return ABAJO;
	}
}


vector2 movimiento_random(vector2 enemigo_pos, int random, int cantidad){
	vector2 posicion_final;

	switch(random){
		case 0:
		case 1:
			//movimiento en L: me muevo dos posiciones en el eje Y hacia Abajo y una en el eje X (Izq. o Der.)
			posicion_final = movimiento_en_L(enemigo_pos, EJE_Y, 1, random, cantidad);
			break;
		case 2:
		case 3:
			//movimiento en L: me muevo dos posiciones en el eje Y hacia Arriba y una en el eje X (Izq. o Der.)
			posicion_final = movimiento_en_L(enemigo_pos, EJE_Y, -1, random, cantidad);
			break;
		case 4:
		case 5:
			//movimiento en L: me muevo dos posiciones en el eje X hacia la Derecha y una en el eje Y (Arriba o Abajo)
			posicion_final = movimiento_en_L(enemigo_pos, EJE_X, 1, random, cantidad);
			break;
		case 6:
		case 7:
			// movimiento en L: me muevo dos posiciones en el eje X hacia la Izquierda y una en el eje Y (Arriba o Abajo)
			posicion_final = movimiento_en_L(enemigo_pos, EJE_X, -1, random, cantidad);
    		break;
	}
	return posicion_final;
}


vector2 movimiento_en_L(vector2 enemigo_pos, int eje, int sentido, int random, int cantidad){
	int eje_alterno;
	vector2 posicion_final;

	if (cantidad > 1){
		posicion_final = vector2_move_pos(enemigo_pos, eje, sentido);
    }else{
    	eje_alterno = get_eje_alterno (eje);

    	if (random%2==0)
    		posicion_final = vector2_move_pos(enemigo_pos, eje_alterno, 1);
    	else
    		posicion_final = vector2_move_pos(enemigo_pos, eje_alterno, -1);
    }
	return posicion_final;
}


int get_eje_alterno(int eje){
	if (eje == EJE_X)
		return EJE_Y;
	else
		return EJE_X;
}

