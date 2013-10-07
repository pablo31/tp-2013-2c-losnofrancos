#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/common/config.h"
#include "../libs/logger/logger.h"
#include "../libs/socket/socket.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/common.h"

#include "nivel_ui.h"
#include "nivel_configuracion.h"
#include "nivel.h"

static bool verificar_argumentos(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Error: Debe ingresar los nombres de los archivos log y configuracion.\n");
		return false;
	}
	return true;
}

int main(int argc, char **argv){
	
	if (!verificar_argumentos(argc, argv)) return EXIT_FAILURE;

	char* exe_name = argv[0];
	char* config_file = argv[1];
	char* log_file = argv[2];
	
	//inicializamos el singleton logger
	logger_initialize_for_info(log_file, exe_name);
	
	//Inicializo el nivel
	nivel* nvl = crear_nivel(config_file);

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

	//tp_logger_info(logger, "Fin de proceso Nivel");

	//Liberamos recursos
	nivel_gui_terminar(); 
	destruir_nivel(nvl);

	return EXIT_SUCCESS;
}
