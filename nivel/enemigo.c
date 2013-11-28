
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

	mutex_close(nivel->semaforo_personajes);
	int cantidad_personajes = list_size(nivel->personajes);
	mutex_open(nivel->semaforo_personajes);

	if(cantidad_personajes > 0){
		logger_info(nivel->logger, "ENEMIGO: Al ataque!!!.");
		atacar_al_personaje(nivel,self);
	}else{
		logger_info(nivel->logger, "ENEMIGO: Nivel sin personajes.");
		mover_en_L(nivel,self);
	}

}

void atacar_al_personaje(tad_nivel* nivel, tad_enemigo* self){
	//se carga la posicion del personaje que esta mas cerca.

	vector2 posicion_actual;

	mutex_close(nivel->semaforo_personajes);
	int cantidad_personajes = list_size(nivel->personajes);
	mutex_open(nivel->semaforo_personajes);

	while(cantidad_personajes>0){
		logger_info(nivel->logger, "ENEMIGO: Nivel tiene %d personajes para atacar.", cantidad_personajes);
		vector2 posicion_personaje;

		mutex_close(nivel->semaforo_personajes);
		mutex_close(nivel->semaforo_enemigos);
		posicion_personaje= buscar_Personaje_Mas_Cercano(nivel,self);
		posicion_actual= self->pos;
		vector2 nueva_posicion = vector2_next_step(posicion_actual,posicion_personaje);

		//controlo si en la posicion a moverse se encuentra una caja
		if (posicion_sin_caja(nivel,nueva_posicion)){
			self->pos = nueva_posicion;
			sleep(1);
		}else{ //si hay una caja busco un movimiento alternativo para esquivarla
			vector2 posicion_alternativa = esquivar_caja(posicion_actual,nueva_posicion,nivel);
			sleep(1);
			self->pos = posicion_alternativa;
		}
		mutex_open(nivel->semaforo_enemigos);
		mutex_open(nivel->semaforo_personajes);

		nivel_gui_dibujar(nivel);

		//si en la nueva posicion se encuentra el personaje se considera atrapado y se informa su muerte.

		mutex_close(nivel->semaforo_enemigos);
		if (vector2_equals(posicion_personaje,self->pos)){
			logger_info(nivel->logger, "ENEMIGO: un personaje fue alcanzado.");

			bool personaje_buscado(void* ptr){
				return vector2_equals(((tad_personaje*)ptr)->pos, posicion_personaje);
			}

			//Se busca personaje
			mutex_close(nivel->semaforo_personajes);
			tad_personaje* personaje_muerto = list_find(nivel->personajes, personaje_buscado);
			mutex_open(nivel->semaforo_personajes);

			logger_info(nivel->logger, "ENEMIGO: murio el personaje %s al ser atrapado por un enemigo.", personaje_muerto->nombre);

			//se avisa la muerte del personaje por enemigo al planificador
			socket_send_char(nivel->socket, MUERTE_POR_ENEMIGO, personaje_muerto->simbolo);
		}
		mutex_open(nivel->semaforo_enemigos);
	}

	//cuando sale del while, significa que no tiene personajes el nivel, se invoca a mover en L
	mover_en_L(nivel,self);

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

	int random = rand()%8; //random 0,7

	int movimientos_faltantes=3;

	int cantidad_personajes = list_size(nivel->personajes);
	mutex_open(nivel->semaforo_personajes);

	while(cantidad_personajes==0){

		while(movimientos_faltantes>0){

			//esto es para que cuando ingrese un personaje, salga de los 2 while
			mutex_close(nivel->semaforo_personajes);
			cantidad_personajes = list_size(nivel->personajes);
			mutex_open(nivel->semaforo_personajes);

			if(cantidad_personajes > 0){
				logger_info(nivel->logger, "ENEMIGO: Al ataque Rom rom rooommmmmm.....!!!.");
				atacar_al_personaje(nivel,self);
				movimientos_faltantes=0;
			}
			mutex_close(nivel->semaforo_enemigos);
			vector2 nueva_pos = vector2_move_in_L(self->pos,random,movimientos_faltantes);
			mutex_open(nivel->semaforo_enemigos);

			//si la posicion esta dentro del mapa se grafica
			if(vector2_within_map(nueva_pos, limite_mapa)){

			//controlo si en la posicion a moverse se encuentra una caja
				if (posicion_sin_caja(nivel,nueva_pos)){

					movimientos_faltantes --;
					mutex_close(nivel->semaforo_enemigos);
					self->pos = nueva_pos;
					logger_info(nivel->logger, "ENEMIGO: moviendose en L a (%d,%d)", self->pos.x,self->pos.y);
					sleep(1);
					mutex_open(nivel->semaforo_enemigos);
					//usleep(nivel->sleep_enemigos * 600); //esto es lo que va

					nivel_gui_dibujar(nivel);

				}
				else{
					//sino completo la L por esquivar una caja tiene que empezar de nuevo y buscar otra L al azar
					movimientos_faltantes=0;
				}

			}else{
				//sino completo la L porque se iba fuera del mapa tiene que empezar de nuevo y buscar otra L al azar
				movimientos_faltantes=0;
			}
		}
		sleep(1); // lo agrego nada mas para visualizar mejor los movimientos cuando pruebo
		movimientos_faltantes=3;
		random = rand()%8;
	}

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
		logger_info(nivel->logger, "ENEMIGO: Esquivo caja: %s.", caja_a_esquivar->nombre);
		return false;

}

vector2 esquivar_caja (vector2 posicion_actual,vector2 nueva_posicion, tad_nivel* nivel){

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

