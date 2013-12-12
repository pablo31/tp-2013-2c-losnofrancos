#ifndef NIVEL
#define NIVEL


#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "../libs/common/collections/list.h"
#include "../libs/common/config.h"

#include "../libs/socket/socket.h"
#include "../libs/logger/logger.h"
#include "../libs/vector/vector2.h"
#include "../libs/thread/mutex.h"
#include "../libs/common.h"

//motivo de muerte
#define FIN 0
#define ENEMIGO 1
#define DEADLOCK 2


class(tad_caja){
	char* nombre;
	char simbolo;
	int instancias;
	vector2 pos;
};


class(tad_recurso){
	char simbolo;
	int cantidad;
};


class(tad_personaje){
	char* nombre;
	char simbolo;
	vector2 pos;
	t_list* recursos_asignados;
	tad_recurso* recurso_pedido;
};


class(tad_enemigo){
	char simbolo;
	vector2 pos;
	tad_personaje* blanco;
	tad_logger* logger; //instancia del logger propia de cada enemigo
};


class (tad_bloqueado){
	char simbolo;
	char recurso;
};

class(tad_nivel){
	char* nombre;
	tad_socket* socket;
	char* config_path;

	tad_mutex* semaforo_personajes;
	t_list* personajes;

	tad_mutex* semaforo_cajas;
	t_list* cajas;

	tad_mutex* semaforo_enemigos;
	t_list* enemigos;

	tad_mutex* semaforo_bloqueados;
	t_list* bloqueados;

	char* algoritmo;
	int retardo;
	int quantum;

	uint tiempo_deadlock;
	bool recovery;
	uint sleep_enemigos;
	tad_logger* logger;
};


tad_logger* get_logger(tad_nivel* self);
char* get_config_path(tad_nivel* self);
void evaluar_solicitud_recurso(tad_nivel* self, char simbolo_personaje, char simbolo_recurso);
void otorgar_recurso(tad_nivel* self, char simbolo_personaje, char simbolo_recurso);
void liberar_y_reasignar_recursos(tad_nivel* self, tad_personaje* personaje_muerto);
void muerte_del_personaje(char personaje_simbolo, tad_nivel* self, int motivo);

#endif
