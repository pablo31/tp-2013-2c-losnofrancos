#include "../libs/vector/vector2.h"
#include <unistd.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "enemigo.h"



void movimiento_permitido_enemigo(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* nivel, tad_enemigo* self);
	var(logger, self->logger);

	logger_info(logger, "Posicion inicial: (%d,%d)", self->pos.x, self->pos.y);

	srand(time(NULL)); //seed para random

	int eje_prox_mov = 1;
	int cantidad_personajes;
	while(true){
		mutex_close(nivel->semaforo_personajes);
		cantidad_personajes = list_size(nivel->personajes);
		mutex_open(nivel->semaforo_personajes);

		logger_info(logger, "Cantidad de personajes en el nivel: %d.", cantidad_personajes);

		if(cantidad_personajes > 0){
			atacar_al_personaje(nivel, self, &eje_prox_mov);
		}else{
			moverse_sin_personajes(nivel, self);
		}
	}
}


void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self, int *eje_prox_mov){
	vector2 posicion_actual;
	vector2 posicion_personaje;

	mutex_close(nivel->semaforo_enemigos);
	posicion_actual = self->pos;
	mutex_open(nivel->semaforo_enemigos);
	logger_info(self->logger, "Posicion actual: (%d,%d)", posicion_actual.x, posicion_actual.y);

	//se carga la posicion del personaje que esta mas cerca.
	posicion_personaje = buscar_personaje_mas_cercano(nivel, self, posicion_actual);

	//controla si el personaje no está en la misma posicion que el enemigo
	int atrapado_por_enemigo = (vector2_equals(posicion_actual, posicion_personaje));

	if(!atrapado_por_enemigo){
		//se calcula la proxima posicion intentando moverse alternadamente por los ejes
		vector2 nueva_posicion = vector2_move_alternately(posicion_actual, posicion_personaje, eje_prox_mov);

		//si hay una caja se busca un movimiento alternativo para esquivarla
		if(!posicion_valida(nivel, nueva_posicion))
			nueva_posicion = esquivar_posicion(posicion_actual, nueva_posicion, posicion_personaje);
		//solo movemos al enemigo si la posicion es valida
		if(posicion_valida(nivel, nueva_posicion))
			self->pos = nueva_posicion;

		//controla si la posicion nueva del enemigo coincide con la del personaje
		if (vector2_equals(self->pos, posicion_personaje))
			muerte_del_personaje(self->blanco->simbolo, nivel, ENEMIGO);
	}
	else
		muerte_del_personaje(self->blanco->simbolo, nivel, ENEMIGO);

	usleep(nivel->sleep_enemigos * 1000);
	nivel_gui_dibujar(nivel);
}


int calcular_distancia (vector2 posicion_a, vector2 posicion_b){
	int distancia;
	vector2 vector_distancia;
	vector_distancia = vector2_subtract(posicion_a, posicion_b);
	distancia = vector_distancia.x + vector_distancia.y;
	return distancia;
}


vector2 buscar_personaje_mas_cercano(tad_nivel* nivel, tad_enemigo* self, vector2 pos_enemigo){
	int distancia_del_enemigo;
	int distancia_aux;
    bool sin_objetivo = true;

    mutex_close(nivel->semaforo_personajes);
    foreach(personaje, nivel->personajes, tad_personaje*){
    	var(pos_personaje, personaje->pos);
    	if (sin_objetivo){
    		distancia_del_enemigo = calcular_distancia(pos_enemigo, pos_personaje);
			mutex_close(nivel->semaforo_enemigos);
			self->blanco = personaje;
			mutex_open(nivel->semaforo_enemigos);
			sin_objetivo = false;
	   }else{
		   distancia_aux = calcular_distancia(pos_enemigo, pos_personaje);
		   if (distancia_aux < distancia_del_enemigo){
			   distancia_del_enemigo = distancia_aux;
			   mutex_close(nivel->semaforo_enemigos);
			   self->blanco = personaje;
			   mutex_open(nivel->semaforo_enemigos);
		   }
	   }
    }
    mutex_open(nivel->semaforo_personajes);

    mutex_close(nivel->semaforo_enemigos);
    var(blanco_pos, self->blanco->pos);
    var(blanco_simbolo, self->blanco->simbolo);
    mutex_open(nivel->semaforo_enemigos);
    logger_info(self->logger, "Personaje a atrapar: %c. Posicion: (%d,%d)", blanco_simbolo, blanco_pos.x, blanco_pos.y);

    return blanco_pos;
}


void moverse_sin_personajes(tad_nivel* nivel, tad_enemigo* self){
	int rows;
	int cols;
	nivel_gui_get_area_nivel(out rows, out cols);
	vector2 limite_mapa = vector2_new(cols, rows);
	vector2 nueva_pos;

	int random = rand()%8; //random 0,7

	int movimientos_faltantes = 3;
	
	mutex_close(nivel->semaforo_personajes);
	int cantidad_personajes_local = list_size(nivel->personajes);
	mutex_open(nivel->semaforo_personajes);

	while(movimientos_faltantes > 0){

		mutex_close(nivel->semaforo_enemigos);
		nueva_pos = movimiento_random(self->pos, random, movimientos_faltantes);
		mutex_open(nivel->semaforo_enemigos);

		//si la posicion esta dentro del mapa se grafica
		if(vector2_within_map(nueva_pos, limite_mapa)){
			mutex_close(nivel->semaforo_enemigos);

			//se controla si en la posicion a moverse se encuentra una caja
			if (posicion_valida(nivel,nueva_pos)){
				movimientos_faltantes --;
				self->pos = nueva_pos;
				logger_info(self->logger, "Moviendose en L a (%d,%d)", self->pos.x,self->pos.y);
				mutex_open(nivel->semaforo_enemigos);
				usleep(nivel->sleep_enemigos * 1000);
				nivel_gui_dibujar(nivel);
			}
			else{
				mutex_open(nivel->semaforo_enemigos);
				//si no completo la L por esquivar una caja tiene que empezar de nuevo y buscar otra L al azar
				movimientos_faltantes = 0;
			}

		}else{
			//sino completo la L porque se iba fuera del mapa tiene que empezar de nuevo y buscar otra L al azar
			movimientos_faltantes = 0;
			logger_info(self->logger, "Esquiva borde en (%d,%d)", nueva_pos.x, nueva_pos.y);
		}
		mutex_close(nivel->semaforo_personajes);
		cantidad_personajes_local = list_size(nivel->personajes);
		mutex_open(nivel->semaforo_personajes);

		if(cantidad_personajes_local > 0)
			movimientos_faltantes=0;
	}
}






bool posicion_valida(tad_nivel* nivel, vector2 pos){
	//buscamos enemigos en la misma posicion
	foreach(enemigo, nivel->enemigos, tad_enemigo*)
		if(vector2_equals(pos, enemigo->pos))
			return false;

	//buscamos cajas en la misma posicion
	foreach(caja, nivel->cajas, tad_caja*)
		if(vector2_equals(pos, caja->pos))
			return false;

	//si no hay enemigos ni cajas, es una posicion valida
	return true;
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

