#include "../libs/vector/vector2.h"
#include <unistd.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "enemigo.h"



void movimiento_permitido_enemigo(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* nivel, tad_enemigo* self);

	logger_info(nivel->logger, "ENEMIGO: Se cargo el enemigo %c. Posicion: (%d,%d)", self->simbolo, self->pos.x, self->pos.y);

	srand(time(NULL)); //seed para random
	//teniendo en cuenta que no:
		//salga del mapa
		//pase por arriba de una caja

	mutex_close(nivel->semaforo_personajes);
	int cantidad_personajes = list_size(nivel->personajes);
	mutex_open(nivel->semaforo_personajes);

	if(cantidad_personajes > 0){
		atacar_al_personaje(nivel, self);
	}else{
		logger_info(nivel->logger, "ENEMIGO: Nivel sin personajes.");
		mover_en_L(nivel, self);
	}

}


void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self){
	//se carga la posicion del personaje que esta mas cerca.

	vector2 posicion_actual;

	mutex_close(nivel->semaforo_personajes);
	int cantidad_personajes = list_size(nivel->personajes);
	mutex_open(nivel->semaforo_personajes);

	logger_info(nivel->logger, "ENEMIGO: Al ataque!!!.");
	logger_info(nivel->logger, "ENEMIGO: EL nivel tiene %d personaje/s para atacar.", cantidad_personajes);

	while(cantidad_personajes>0){

		vector2 posicion_personaje;

		mutex_close(nivel->semaforo_personajes);
		mutex_close(nivel->semaforo_enemigos);
		posicion_personaje = buscar_personaje_mas_cercano(nivel, self);
		posicion_actual = self->pos;
		vector2 nueva_posicion = vector2_next_step(posicion_actual, posicion_personaje);
		
		//controlo si en la posicion a moverse se encuentra una caja
		if (posicion_sin_caja(nivel, nueva_posicion)){
			self->pos = nueva_posicion;
		}else{ //si hay una caja busco un movimiento alternativo para esquivarla
			vector2 posicion_alternativa = esquivar_caja(posicion_actual, nueva_posicion, posicion_personaje);
			self->pos = posicion_alternativa;
		}

		//si en la nueva posicion se encuentra el personaje se considera atrapado y se informa su muerte.
		verificar_muerte_por_enemigo(self->blanco, self->pos, nivel);

		mutex_open(nivel->semaforo_enemigos);
		mutex_open(nivel->semaforo_personajes);
		
		sleep(1);
		nivel_gui_dibujar(nivel);

        mutex_close(nivel->semaforo_personajes);
        cantidad_personajes = list_size(nivel->personajes);
        mutex_open(nivel->semaforo_personajes);
	}

	//cuando sale del while, significa que no tiene personajes el nivel, se invoca a mover en L
	mover_en_L(nivel,self);

}


int calcular_distancia (vector2 posicion_a, vector2 posicion_b){
	int distancia;
	vector2 vector_distancia;
	vector_distancia = vector2_subtract(posicion_a, posicion_b);
	distancia = vector_distancia.x + vector_distancia.y;
	return distancia;
}


vector2 buscar_personaje_mas_cercano(tad_nivel* nivel, tad_enemigo* self){

	int distancia_del_enemigo;
	int distancia_aux;
    bool sin_objetivo = true;

    var(pos_enemigo, self->pos);

    foreach(personaje, nivel->personajes, tad_personaje*){

       var(pos_personaje, personaje->pos);

       if (sin_objetivo){
    	   distancia_del_enemigo = calcular_distancia(pos_enemigo, pos_personaje);
           self->blanco = personaje;
           sin_objetivo = false;
       }else{
    	   distancia_aux = calcular_distancia(pos_enemigo, pos_personaje);
           if (distancia_aux < distancia_del_enemigo){
        	   distancia_del_enemigo = distancia_aux;
        	   self->blanco = personaje;
           }
       }
    }

    var(blanco_pos, self->blanco->pos);
    var(blanco_simbolo, self->blanco->simbolo);
    logger_info(nivel->logger, "ENEMIGO: personaje a atrapar: %c. Posicion: (%d,%d)", blanco_simbolo, blanco_pos.x, blanco_pos.y);

    return blanco_pos;
}

//logger_info(nivel->logger, "posicon enemigo (%d:%d)", self->pos.x,self->pos.y);
void mover_en_L(tad_nivel* nivel, tad_enemigo* self){

	int rows;
	int cols;
	nivel_gui_get_area_nivel(out rows, out cols);
	vector2 limite_mapa = vector2_new(cols, rows);
	vector2 nueva_pos;

	int random = rand()%8; //random 0,7

	int movimientos_faltantes=3;
	
	mutex_close(nivel->semaforo_personajes);
	int cantidad_personajes = list_size(nivel->personajes);
	mutex_open(nivel->semaforo_personajes);

	logger_info(nivel->logger, "ENEMIGO: empieza a moverse en L");

	while(cantidad_personajes==0){

		while(movimientos_faltantes>0){

			mutex_close(nivel->semaforo_enemigos);
			nueva_pos = vector2_move_in_L(self->pos, random, movimientos_faltantes);
			mutex_open(nivel->semaforo_enemigos);

			if(vector2_within_map(nueva_pos, limite_mapa)){

			//controlo si en la posicion a moverse se encuentra una caja
				if (posicion_sin_caja(nivel,nueva_pos)){

					movimientos_faltantes --;
					mutex_close(nivel->semaforo_enemigos);
					self->pos = nueva_pos;
					//logger_info(nivel->logger, "ENEMIGO: moviendose en L a (%d,%d)", self->pos.x,self->pos.y);
					mutex_open(nivel->semaforo_enemigos);
					//usleep(nivel->sleep_enemigos * 600); //esto es lo que va
					sleep(1);
					nivel_gui_dibujar(nivel);
				}
				else{
					//si no completo la L por esquivar una caja tiene que empezar de nuevo y buscar otra L al azar
					movimientos_faltantes=0; 
				}
				

			}else{
				//sino completo la L porque se iba fuera del mapa tiene que empezar de nuevo y buscar otra L al azar
				movimientos_faltantes=0;
				logger_info(nivel->logger, "ENEMIGO: esquiva borde en (%d,%d)", nueva_pos.x, nueva_pos.y);

			}
			mutex_close(nivel->semaforo_personajes);
			cantidad_personajes = list_size(nivel->personajes);
			mutex_open(nivel->semaforo_personajes);

			if(cantidad_personajes > 0){
				movimientos_faltantes=0;
			}
			
		}
		movimientos_faltantes=3;
		random = rand()%8;
	}
	
	atacar_al_personaje(nivel,self);
}

bool posicion_sin_caja(tad_nivel* nivel, vector2 nueva_pos){

	mutex_close(nivel->semaforo_cajas);

	bool caja_buscada(void* ptr){
		return vector2_equals(((tad_caja*)ptr)->pos, nueva_pos);
	}
	tad_caja* caja_a_esquivar = list_find(nivel->cajas, caja_buscada);

	mutex_open(nivel->semaforo_cajas);

	if (caja_a_esquivar == NULL)
		return true;
	else
		logger_info(nivel->logger, "ENEMIGO: Esquiva caja %c en (%d,%d).", caja_a_esquivar->simbolo, caja_a_esquivar->pos.x, caja_a_esquivar->pos.y);
		return false;

}

/*
vector2 esquivar_caja(vector2 posicion_actual,vector2 nueva_posicion, tad_nivel* nivel){

	bool posicion_no_disponible = true;
	vector2 posicion_alternativa;
	int direccion_movimiento;
	int movimiento_incorrecto;

	movimiento_incorrecto = calcular_direccion_movimiento(posicion_actual, nueva_posicion);

	while (posicion_no_disponible){
		switch (movimiento_incorrecto){
			case ARRIBA:
				//intenta moverse hacia la izquierda
				posicion_alternativa = vector2_add_x(posicion_actual, -1);
				direccion_movimiento = IZQUIERDA;
				break;
			case IZQUIERDA:
				//intenta moverse hacia abajo
				posicion_alternativa = vector2_add_y(posicion_actual, 1);
				direccion_movimiento = ABAJO;
				break;
			case ABAJO:
				//intenta moverse hacia la derecha
				posicion_alternativa = vector2_add_x(posicion_actual, 1);
				direccion_movimiento = DERECHA;
				break;
			case DERECHA:
				//intenta moverse hacia arriba
				posicion_alternativa = vector2_add_y(posicion_actual, -1);
				direccion_movimiento = ARRIBA;
				break;
		}

		if (posicion_sin_caja(nivel, posicion_alternativa)){
			posicion_no_disponible = false;
		}
		else{
			posicion_no_disponible = true;
			movimiento_incorrecto = direccion_movimiento;

		}

	}
	return posicion_alternativa;
}
*/

vector2 esquivar_caja(vector2 posicion_actual, vector2 nueva_posicion, vector2 posicion_personaje){

    vector2 posicion_alternativa;
    int movimiento_incorrecto;

    	//para esquivar la caja veo en que direccion me iba a mover y calculo una posicion alternativa 
    	//moviendome en el eje contrario, elijo el sentido segun la posicion del personaje

	movimiento_incorrecto = calcular_direccion_movimiento(posicion_actual, nueva_posicion);

	
    switch (movimiento_incorrecto){

           case ARRIBA:
           case ABAJO:

        	   if (posicion_actual.x > posicion_personaje.x) {
        		   //se mueve hacia la izquierda
        		   posicion_alternativa = vector2_add_x(posicion_actual, -1);
        	   }else{
        		  //se mueve hacia la derecha
        		   posicion_alternativa = vector2_add_x(posicion_actual, 1);
        	   }
        	   break;

           case IZQUIERDA:
           case DERECHA:

        	   if (posicion_actual.y > posicion_personaje.y) {
        		   //se mueve  hacia arriba
        		   posicion_alternativa = vector2_add_y(posicion_actual, -1);
        	   }else{
        		   //se mueve hacia abajo
        		   posicion_alternativa = vector2_add_y(posicion_actual, 1);
        	   }
        	   break;
     	}

	return posicion_alternativa;

}

int calcular_direccion_movimiento(vector2 pos1, vector2 pos2) {

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


