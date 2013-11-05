#include "verificador_deadlock.h"


void loguear_deadlock_detectado(tad_nivel* nivel,t_list* personajes_bloqueados);

void avisar_deadlock_al_planificador(tad_nivel* nivel,t_list* personajes_bloqueados);


void* verificador_deadlock(void* level) {

	tad_nivel* nivel = (tad_nivel*) level;

	nivel_loguear(nivel->logger, nivel,
			"Verificador de Deadlock creado, tiempo de checkeo: %.2f segundos",
			nivel->tiempo_deadlock / 1000000.0);

	while (true) {
		//TODO cada cierto tiempo tiene que verificar si el nivel tiene deadlock
	}

	return (void*) EXIT_SUCCESS;
}

void loguear_deadlock_detectado(tad_nivel* nivel,
		t_list* personajes_bloqueados) {

	//Todo loquega los personajes
}

void avisar_deadlock_al_planificador(tad_nivel* nivel,
		t_list* personajes_bloqueados) {

	//TODO le avisa al planificador y se carga en el archivo log
}



void liberar_recursos_del_personaje(t_list* personaje,
		t_list* recursos_disponibles) {
	//TODO los personajes que se mueren, sea porque los mato un enemigo
	// o el planificador para librerar el deadlock, se tiene que liberar
	//recursos y cargarlos en el archivo log
}



