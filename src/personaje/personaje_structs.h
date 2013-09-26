#include "../libs/socket/socket.h"

#ifndef POSICION_H_
#define POSICION_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

//Tipos de Mensajes
//Handshakes (1-10)
#define M_HANDSHAKE_PERSONAJE 1

//Contenidos de Mensajes
#define PERSONAJE_HANDSHAKE "Aquí un personaje"
#define HANDSHAKE_SUCCESS "OK"

//esto se tiene que sacar a una clase extrarna, pablo no se si en algun lado tenes a
//alguna estrutura como esta...
typedef struct t_mensaje {
	uint8_t type;
	uint16_t length;
	void* payload;
}__attribute__ ((packed)) t_mensaje;

typedef struct t_connection_info {
	char* ip;
	//uint32_t puerto;
	int puerto;
}__attribute__ ((packed)) t_connection_info;


typedef enum {
	SOCKETSTATE_CONNECTED, SOCKETSTATE_DISCONNECTED
} e_socket_state;


typedef struct {
	int desc;
	struct sockaddr *my_addr;
	e_socket_mode mode;
} t_socket;


typedef struct {
	t_socket* socket;
	t_socket* serv_socket;
	e_socket_state state;
} t_socket_client;

///---------------------pablo tenemos que ver donde ponemos esto... XD


#define DISTANCIA_MOVIMIENTO_PERMITIDA 1

typedef struct t_posicion {
	uint32_t x;
	uint32_t y;
}__attribute__ ((packed)) t_posicion;
