#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "nivel.h"
#include "nivel_ui.h"
#include "../libs/common/config.h"
//#include "../libs/logger/logger.h"
#include "../libs/common/string.h"
#include "../libs/common/collections/list.h"

//tp_logger* logger; //extern declarado en nivel.h
t_config* configuracion;//extern declarado en nivel.h


void destruir_nivel(nivel* nivel){
	//tp_logger_info(logger, "Liberando recursos del nivel.");
	
	//tp_logger_debug(logger, "Liberando lista de cajas");
	list_destroy(nivel->cajas);
	//free(nvl->nombre);
	//free(nvl->orquestador);
	free(nivel);
}

void cargar_recursos_nivel(nivel* nivel){	
	ITEM_NIVEL* ListaItems = NULL;

	int rows, cols;
	//int q, p;

	//int x = 1;
	//int y = 1;

    nivel_gui_get_area_nivel(&rows, &cols);

	//q = rows;
	//p = cols;

	//tp_logger_info(logger, "Cargando recursos del nivel en la libreria de Curses.");
	int cantidad_niveles = list_size(nivel->cajas); 
	int i;
	for (i= 0; i < cantidad_niveles ; i++){
		caja* caja = list_get(nivel->cajas,i); 		

		nivel_gui_crear_caja(&ListaItems, caja->simbolo, caja->pos_x ,caja->pos_y , caja->instancias);
	}
	
	//tp_logger_info(logger, "Iniciando el dibujado del nivel.");
	nivel_gui_dibujar(ListaItems);
}

static caja* crear_caja(char* nombre,char simbolo, uint instancias, uint pos_x, uint pos_y){
	caja* caja = malloc(sizeof(caja));
	
	caja->nombre = string_new();
	caja->nombre = strdup(nombre);
	
	caja->simbolo = simbolo;
	caja->instancias = instancias;
	caja->pos_x = pos_x;
	caja->pos_y = pos_y;
	
	return caja;
}

void cargar_enemigos(nivel* nivel, uint cantidad){
	const uint MAX_POSICION = 15;
	int i;
	for (i = 0; i < cantidad; ++i){

		enemigo* enemigo = malloc(sizeof(enemigo));
		enemigo->simbolo = '*';

		srand (time(NULL));
		enemigo->pos_x =  1 + (rand() % MAX_POSICION);
		enemigo->pos_y =  1 + (rand() % MAX_POSICION);
		//le sumo 1 porque no puede ser 0 0 nunca.


		list_add(nivel->enemigos, enemigo);	
	}	
}

nivel* crear_nivel(){
	nivel* nvl = malloc(sizeof(nivel));
	
	nvl->nombre = "";
	nvl->plataforma = string_new();
	nvl->tiempo_deadlock = 0;
	nvl->recovery = false;
	nvl->cajas = malloc(sizeof(caja*));
	nvl->cajas = list_create();
	
	return nvl;
}

//static void nivel_logear_configuracion_cargada(nivel* nvl){
	//tp_logger_info(logger, "Nombre: %s",nvl->nombre);
	//tp_logger_info(logger, "Orquestador: %s",nvl->orquestador);
	//tp_logger_info(logger, "Deadlock: %i",nvl->tiempo_deadlock);
	//tp_logger_info(logger, "recovery: %s", (nvl->recovery) ? "true":"false");
		
	//int i; 	
	//for(i = 0; i < list_size(nvl->cajas); i++){
		//tp_nodo_caja* caja = list_get(nvl->cajas, i);
		
		//tp_logger_info(logger, "Caja numero %i:", i+1);
		//tp_logger_info(logger, "\tNombre: %s", caja->nombre);
		//tp_logger_info(logger, "\tSimbolo: %c", caja->simbolo);
		//tp_logger_info(logger, "\tInstancias: %i", caja->instancias);
		//tp_logger_info(logger, "\tPosicion X: %i", caja->pos_x);
		//tp_logger_info(logger, "\tPosicion Y: %i", caja->pos_y);
	//}
//}

bool cargar_configuracion_nivel(nivel* nvl){	
	bool carga_correcta = true;


	printf("Nivel\n");

	if (config_has_property(configuracion,"Nombre")){
		nvl->nombre = config_get_string_value(configuracion, "Nombre");
		printf("nombre:%s\n",nvl->nombre);
	}else{		
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del nombre");
		carga_correcta = false;
	}

	if (config_has_property(configuracion,"TiempoChequeoDeadlock")){
		nvl->tiempo_deadlock = config_get_int_value(configuracion,"TiempoChequeoDeadlock");
		printf("deadlock:%s\n",(nvl->tiempo_deadlock) ? "si" : "no" );
	}else{
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del tiempo de chequeo del deadlock");
		carga_correcta = false;
	}

	if (config_has_property(configuracion,"Recovery")){
		nvl->recovery = (bool) config_get_int_value(configuracion,"Recovery");
		printf("recovery:%i\n",nvl->tiempo_deadlock);
	}else{
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del Recovery");
		carga_correcta = false;
	}

	if (config_has_property(configuracion,"Enemigos")){
		uint enemigos = config_get_int_value(configuracion,"Enemigos");
		printf("Enemigos:%i\n",enemigos);

		//cargar_enemigos(nvl,enemigos);

	}else{
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del Recovery");
		carga_correcta = false;
	}

	if (config_has_property(configuracion,"Sleep_Enemigos")){
		nvl->sleep_enemigos = config_get_int_value(configuracion,"Sleep_Enemigos");
		printf("Sleep_Enemigos:%i\n",nvl->sleep_enemigos);
	}else{
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del Recovery");
		carga_correcta = false;
	}

	if (config_has_property(configuracion,"algoritmo")){
		nvl->algoritmo = config_get_string_value(configuracion,"algoritmo");
		printf("algoritmo:%s\n",nvl->algoritmo);
	}else{
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del Recovery");
		carga_correcta = false;
	}

	if (config_has_property(configuracion,"retardo")){
		nvl->retardo = config_get_int_value(configuracion,"retardo");
		printf("retardo:%i\n",nvl->retardo);
	}else{
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del Recovery");
		carga_correcta = false;
	}

	if (config_has_property(configuracion,"Plataforma")){
		nvl->plataforma = config_get_string_value(configuracion,"Plataforma");
		printf("Plataforma:%s\n",nvl->plataforma);
	}else{
		//tp_logger_error(logger,"El archivo de configuracion no tiene el valor del Recovery");
		carga_correcta = false;
	}
	
		
	uint  numero_caja = 1;
	char* nombre_caja = string_from_format("Caja%i",numero_caja);
	char* nombre =  "";
	char  simbolo ;
	uint  instancias;
	uint  pos_x;
	uint  pos_y;


	printf("\nCajas\n");
	char *p;
	int base = 10;
	while(config_has_property(configuracion, nombre_caja)){

		printf("\t%s\n",nombre_caja);

		char** valores = config_get_array_value(configuracion,nombre_caja );
		
		nombre = valores[0];
		printf("\tnombre:%s\n",nombre);
		simbolo = valores[1][0];
		printf("\tsimbolo:%c\n",simbolo);
		instancias = atoi(valores[2]);
		printf("\tinstancias:%i\n",instancias);

		pos_x = strtol(valores[3], &p, base);
		printf("\tpos_x:%i\n",pos_x);
		
		//pos_y = strtol(valores[4], &p, base);
		printf("\tpos_y:%i\n",pos_y);
		
		pos_y = 3;
		
		printf("\n");
		caja* caja = crear_caja(nombre,simbolo,instancias,pos_x,pos_y);		
		list_add(nvl->cajas, caja);	
		
		numero_caja++;
		nombre_caja = string_from_format("Caja%i",numero_caja);
	}

	//nivel_logear_configuracion_cargada(nvl);

	return carga_correcta;	
}
