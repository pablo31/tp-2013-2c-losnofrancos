#include <unistd.h>
#include "../libs/common/collections/list.h"
#include "../libs/common/string.h"
#include "verificador_deadlock.h"

class(t_personaje){
	char simbolo;
    _Bool blocked;
    char recurso_esperado;
    t_list* recursos_asignados;
    vector2 posicion;
};

class(t_recurso){
	char simbolo;
	int cantidad;
};

void loguear_deadlock_detectado(tad_nivel* nivel,t_list* personajes_bloqueados);

void avisar_deadlock_al_planificador(tad_nivel* nivel,t_list* personajes_bloqueados);

char* personajes_as_string_ids(t_list* personajes);

void* verificador_deadlock(void* level) {

	tad_nivel* nivel = (tad_nivel*) level;

	nivel_loguear(nivel->logger, nivel,
			"Verificador de Deadlock creado, tiempo de checkeo: %.2f segundos",
			nivel->tiempo_deadlock / 1000000.0);

	while (true) {
		//cada cierto tiempo tiene que verificar si el nivel tiene deadlock

		nivel_loguear(nivel->logger, nivel,
			"Verificador de Deadlock esperando %d segundos para realizar el checkeo.",
			nivel->tiempo_deadlock);

		usleep(nivel->tiempo_deadlock);

		t_list* recursos_disponibles = clonar_recursos(nivel);
		t_list* personajes_bloqueados = clonar_personajes(nivel);

		bool hay_deadlock = false;

		bool personaje_puede_ejecutar(t_personaje* personaje) {
				if (personaje->recurso_esperado == NULL ) {
					liberar_recursos_del_personaje(personaje, recursos_disponibles);
					hay_deadlock = false;
					return true;
				}

				bool es_el_recurso(t_recurso* recurso) {
					return recurso->simbolo
							== personaje->recurso_esperado[0];
				}

				t_recurso* recurso = list_find(recursos_disponibles,
						(void*) es_el_recurso);

				if (recurso->cantidad > 0) {
						liberar_recursos_del_personaje(personaje, recursos_disponibles);
						hay_deadlock = false;
						return true;
				}

				return false;
			}

			while (!hay_deadlock && !list_is_empty(personajes_bloqueados)) {
					hay_deadlock = true;
					list_remove_and_destroy_by_condition(personajes_bloqueados,
							(void*) personaje_puede_ejecutar,
							(void*) nivel_destroy_personaje);
			}

			if (hay_deadlock) {
					loguear_deadlock_detectado(nivel, personajes_bloqueados);

					if (nivel->recovery) {
						avisar_deadlock_al_planificador(nivel, personajes_bloqueados);
					}

			} else {
					nivel_loguear(nivel->logger, nivel,
							"Verificador deadlock: NO HAY DEADLOCK");
			}

			//list_remove_and_destroy_element(recursos_disponibles,
			//			(void*) recurso_destroy);

			//list_remove_and_destroy_element(personajes_bloqueados,
			//			(void*) nivel_destroy_personaje);

		}

	return (void*) EXIT_SUCCESS;
}

void loguear_deadlock_detectado(tad_nivel* nivel,
		t_list* personajes_bloqueados) {

	char* ids_personajes_en_deadlock = personajes_as_string_ids(
			personajes_bloqueados);

	nivel_loguear(nivel->logger, nivel,
			"Verificador deadlock: DEADLOCK DETECTADO, personajes involucrados: %s",
			ids_personajes_en_deadlock);

	free(ids_personajes_en_deadlock);

}

void avisar_deadlock_al_planificador(tad_nivel* nivel,
		t_list* personajes_bloqueados) {


	char* ids_personajes = personajes_as_string_ids(personajes_bloqueados);

	/*
	Avisar al planificador y cargar en el archivo log
	*/

	free(ids_personajes);
}


char* personajes_as_string_ids(t_list* personajes) {
	char* ids_personajes = string_new();

	void appendear_id_personaje(tad_personaje* personaje) {
		char* id_personaje = string_from_format("%c ", personaje->simbolo);
		string_append(&ids_personajes, string_duplicate(id_personaje));
		free(id_personaje);
	}

	list_iterate(personajes, (void*) appendear_id_personaje);

	string_trim(&ids_personajes);
	return ids_personajes;
}


void liberar_recursos_del_personaje(t_personaje* personaje,
		t_list* recursos_disponibles) {

	void liberar_recurso(t_recurso* recurso) {
			bool es_el_recurso(t_recurso* elem) {
				return recurso_equals(recurso, elem);
			}

			t_recurso* mi_recurso = list_find(recursos_disponibles,
					(void*) es_el_recurso);
			mi_recurso->cantidad += recurso->cantidad;

	}

	list_iterate(personaje->recursos_asignados,(void *) liberar_recurso);

}

t_list* clonar_personajes(tad_nivel* nivel) {
//	t_list* personajes_bloqueados = list_create();

	t_personaje* clonar_personaje(t_personaje* personaje) {
		t_personaje* new = malloc(sizeof(t_personaje));
		new->simbolo = personaje->simbolo;

		if (personaje->recurso_esperado == NULL ) {
			new->recurso_esperado = NULL;
		} else {
			new->recurso_esperado = string_duplicate(
				personaje->recurso_esperado);
		}

		//new->recursos_asignados = list_clone_and_clone_elements(
		//		personaje->recursos_asignados, (void*) recurso_clone);
		//new->posicion = NULL;

		return new;
	}

	list_iterate(nivel->personajes, (void*) clonar_personaje_bloqueado);
	return personajes_bloqueados;

	return list_clone_and_clone_elements(nivel-> personajes,
			(void*) clonar_personaje);
}

//t_list* clonar_recursos(tad_nivel* nivel) {
//	return list_clone_and_clone_elements(nivel-> cajas,
//			(void*) recurso_clone);
//}



