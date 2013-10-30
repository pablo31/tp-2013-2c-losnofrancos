#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/socket/socket_utils.h"
#include "../libs/multiplexor/multiplexor.h"
#include "../libs/notifier/notifier.h"

#include "../libs/protocol/protocol.h"

#include "nivel_ui.h"
#include "nivel_configuracion.h"
#include "nivel.h"


private void nivel_finalizar(tad_nivel* self);
private void manejar_error_planificador(tad_nivel* self, tad_socket* socket);
private void manejar_paquete_planificador(PACKED_ARGS);
private void config_file_modified(PACKED_ARGS);




private void verificar_argumentos(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Error: Debe ingresar los nombres de los archivos log y configuracion.\n");
		exit(EXIT_FAILURE);
	}
}

tad_logger* get_logger(tad_nivel* self){
	return self->logger;
}

int main(int argc, char **argv){
	
	verificar_argumentos(argc, argv);

	char* exe_name = argv[0];
	char* config_file = argv[1];
	char* log_file = argv[2];
	
	//inicializamos el singleton logger
	logger_initialize_for_info(log_file, exe_name);
	
	//Inicializo el nivel
	char* ippuerto_plataforma;
	tad_nivel* self = crear_nivel(config_file, out ippuerto_plataforma);
	var(logger, get_logger(self));

	//Me conecto con el orquestador
	var(ip, string_get_ip(ippuerto_plataforma));
	var(puerto, string_get_port(ippuerto_plataforma));
	logger_info(logger, "Conectando a %s", ippuerto_plataforma);

	tad_socket* socket = socket_connect(ip, puerto);
	logger_info(logger, "Conectado con Plataforma");
	free(ippuerto_plataforma);

	//Declaramos un bloque de manejo de errores por si el socket pierde la conexion
	SOCKET_ON_ERROR_WRET(socket, manejar_error_planificador(self, socket), EXIT_FAILURE);

	//Esperamos la presentacion del orquestador
	socket_receive_expected_empty_package(socket, PRESENTACION_ORQUESTADOR);
	logger_info(logger, "El servidor es un Orquestador");

	//Nos presentamos
	socket_send_empty_package(socket, PRESENTACION_NIVEL);

	//Indicamos nuestro nombre
	socket_send_string(socket, NIVEL_NUMERO, self->nombre);
	//Le indicamos la cantidad de quantums al planificador
	socket_send_int(socket, QUANTUM, self->quantum);
	//Le indicamos la pausa entre turnos al planificador
	socket_send_int(socket, RETARDO, self->retardo);
	//Le indicamos el algoritmo al planificador
	socket_send_string(socket, ALGORITMO, self->algoritmo);

	//Inicializo la UI
	logger_info(logger, "Inicializando interfaz grafica");
	if (nivel_gui_inicializar() != EXIT_SUCCESS)
		return EXIT_FAILURE;

	//Cargo los recursos en la pantalla
	cargar_recursos_nivel(self);

	//Creamos un notificador de cambios sobre el archivo de configuracion
	tad_notifier* notifier = notifier_create(config_file);

	//Creamos un multiplexor y le asociamos el notificador y el socket del planificador
	tad_multiplexor* multiplexor = multiplexor_create();
	multiplexor_bind_notifier(multiplexor, notifier, config_file_modified, self, socket, config_file);
	multiplexor_bind_socket(multiplexor, socket, manejar_paquete_planificador, self, socket);

	//Esperamos por mensajes entrantes
	while(1)
		multiplexor_wait_for_io(multiplexor);

	logger_info(logger, "Fin del proceso Nivel");

	//Liberamos recursos
	nivel_gui_terminar(); 
	destruir_nivel(self);

	return EXIT_SUCCESS;
}


private void config_file_modified(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* self, tad_socket* socket, char* config_file);

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


private void nivel_finalizar(tad_nivel* self){
	logger_info(get_logger(self), "Fin del proceso Nivel");

	//Liberamos recursos
	nivel_gui_terminar();
	destruir_nivel(self);

	logger_dispose();
}

private void manejar_error_planificador(tad_nivel* self, tad_socket* socket){
	logger_error(get_logger(self), "Se cierra la conexion con Plataforma de manera inesperada");
	socket_close(socket);
	nivel_finalizar(self);
}

private void manejar_paquete_planificador(PACKED_ARGS){
	UNPACK_ARGS(tad_nivel* self, tad_socket* socket);

}
