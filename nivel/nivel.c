#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/common/config.h"
#include "../libs/logger/logger.h"
#include "../libs/socket/socket.h"
#include "../libs/socket/socket_utils.h"

#include "nivel_ui.h"
#include "nivel_configuracion.h"
#include "nivel.h"

//tp_logger* logger; //extern declarado en nivel.h
t_config* configuracion;//extern declarado en nivel.h

int main(int argc, char **argv){
	
	if (argc < 2){//Valido que tenga los argumentos necesarios
		printf("\tDebe especificar el archivo de configuración y el archivo de log.\n");
		printf("\tej: nivel.sh nivel1.conf nivel1.log \n");
		return EXIT_FAILURE;		
	}	
	
	//char* nombre_ejecutable = argv[0];
	char* archivo_configuracion = argv[1];
	//char* archivo_log = argv[2];
	
	//Creo log y config
	//tp_logger_inicializar(archivo_log, nombre_ejecutable);
	//logger = //tp_logger_instancia("");
	
	//Inicializo el nivel
	nivel* nvl = crear_nivel();

	//Cargo su configuracion
	configuracion = config_create(archivo_configuracion);
	if (!cargar_configuracion_nivel(nvl)){
		//tp_logger_error(logger, "Error al cargar la configuración de %s, valide el archivo de configuracion y vuelva a ejecutar", nombre_ejecutable);
		return EXIT_FAILURE;
	}

	//Me conecto con el orquestador
	//tp_socket* socket = tp_socket_conectar(tp_ip(nvl->orquestador), tp_puerto(nvl->orquestador));
	//tp_logger_info(logger, "Conectado con orquestador");

	//Nos presentamos, informamos nuestro numero de nivel
	//tp_socket_enviar_paquete_vacio(socket, PRESENTACION_NIVEL);
	//tp_socket_enviar(socket, NIVEL_NUMERO, strlen(nvl->nombre), nvl->nombre);
	//TODO
	//tp_socket_cerrar(socket); //TODO esta linea va al final, al hacer ctrl+c

	//Inicializo la UI
	//tp_logger_info(logger, "Iniciando interfaz grafica");
	if (nivel_gui_inicializar() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//Cargo los recursos en la pantalla
	cargar_recursos_nivel(nvl);

	while(true){
		//TODO multiplexor - logica de nivel
	}

	//Informamos el fin del proceso
	//tp_logger_info(logger, "Fin de proceso Nivel");

	//Liberamos recursos
	nivel_gui_terminar(); 
	destruir_nivel(nvl);
	config_destroy(configuracion);
	//tp_logger_liberar(logger);
	//tp_logger_finalizar();

	return EXIT_SUCCESS;
}
