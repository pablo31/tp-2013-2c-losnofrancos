#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/common/config.h"
#include "../libs/logger/logger.h"
#include "../libs/socket/socket.h"
#include "../libs/socket/socket_utils.h"
#include "../libs/multiplexor/multiplexor.h"
#include "../libs/notifier/notifier.h"
#include "../libs/common.h"

#include "../libs/protocol/protocol.h"

#include "nivel_ui.h"
#include "nivel_configuracion.h"
#include "nivel.h"


private void config_file_modified(PACKED_ARGS);




private bool verificar_argumentos(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Error: Debe ingresar los nombres de los archivos log y configuracion.\n");
		return false;
	}
	return true;
}

private tad_logger* get_logger(nivel* nvl){
	return nvl->logger;
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
	var(logger, get_logger(nvl));

	//Me conecto con el orquestador
	var(ippuerto, nvl->plataforma);
	var(ip, string_get_ip(ippuerto));
	var(puerto, string_get_port(ippuerto));
	logger_info(logger, "Conectando a %s", ippuerto);

	tad_socket* socket = socket_connect(ip, puerto);
	logger_info(logger, "Conectado con Plataforma");

	//Declaramos un bloque de manejo de errores por si el socket pierde la conexion
	SOCKET_ERROR_MANAGER(socket){
		//TODO switch que muestre un mensaje distinto dependiendo del error que se produjo
		logger_error(logger, "Se cierra la conexion con Plataforma de manera inesperada");
		socket_close(socket);
		nivel_gui_terminar();
		destruir_nivel(nvl);
		return EXIT_FAILURE;
	}

	//Esperamos la presentacion del orquestador
	socket_receive_expected_empty_package(socket, PRESENTACION_ORQUESTADOR);
	logger_info(logger, "El servidor es un Orquestador");

	//Nos presentamos
	socket_send_empty_package(socket, PRESENTACION_NIVEL);

	//Indicamos nuestro nombre
	socket_send_string(socket, NIVEL_NUMERO, nvl->nombre);
	//Le indicamos la cantidad de quantums al planificador
	socket_send_int(socket, QUANTUM, nvl->quantum);
	//Le indicamos la pausa entre turnos al planificador
	socket_send_int(socket, RETARDO, nvl->retardo);
	//Le indicamos el algoritmo al planificador
	socket_send_string(socket, ALGORITMO, nvl->algoritmo);

//	//Inicializo la UI
//	logger_info(logger, "Inicializando interfaz grafica");
//	if (nivel_gui_inicializar() != EXIT_SUCCESS)
//		return EXIT_FAILURE;
//
//	//Cargo los recursos en la pantalla
//	cargar_recursos_nivel(nvl);

	tad_multiplexor* multiplexor = multiplexor_create();
	tad_notifier* notifier = notifier_create(config_file);
	multiplexor_bind_notifier(multiplexor, notifier, config_file_modified, nvl, socket, config_file);
	while(1){
		//TODO multiplexor - logica de nivel
		multiplexor_wait_for_io(multiplexor);
	}

	logger_info(logger, "Fin del proceso Nivel");

	//Liberamos recursos
	nivel_gui_terminar(); 
	destruir_nivel(nvl);

	return EXIT_SUCCESS;
}


private void config_file_modified(PACKED_ARGS){
	UNPACK_ARGS(nivel* self, tad_socket* socket, char* config_file);

	char* algoritmo = self->algoritmo;
	int quantum = self->quantum;
	int retardo = self->retardo;

	char* nuevo_algoritmo;
	int nuevo_quantum;
	int nuevo_retardo;

	t_config* config = config_create(config_file);

	cargar_configuracion_cambiante(self, config,
			out nuevo_algoritmo, out nuevo_quantum, out nuevo_retardo);

	config_destroy(config);

	if(quantum != nuevo_quantum){
		self->quantum = nuevo_quantum;
		socket_send_int(socket, QUANTUM, nuevo_quantum);
	}
	if(retardo != nuevo_retardo){
		self->retardo = nuevo_retardo;
		socket_send_int(socket, RETARDO, nuevo_retardo);
	}
	if(!string_equals(algoritmo, nuevo_algoritmo)){
		self->algoritmo = nuevo_algoritmo;
		free(algoritmo);
		socket_send_string(socket, ALGORITMO, nuevo_algoritmo);
	}else{
		free(nuevo_algoritmo);
	}
}
