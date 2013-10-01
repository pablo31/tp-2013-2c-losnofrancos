//#include "mensaje_personaje.h"  // ver si conviene o no crearla
#include <stdio.h>
#include <libs/common/string.h>


//Contenidos de Mensajes
#define PERSONAJE_HANDSHAKE "Aquí un personaje"
#define HANDSHAKE_SUCCESS "OK"

//Tipos de Mensajes
//Handshakes (1-10)
#define M_HANDSHAKE_PERSONAJE 1


//Request(21-40)
#define M_GET_SYMBOL_PERSONAJE_REQUEST 23

//Response(41-60)
#define M_GET_SYMBOL_PERSONAJE_RESPONSE 44

typedef struct t_mensaje {
	uint8_t type;
	uint16_t length;
	void* payload;
}__attribute__ ((packed)) t_mensaje;

typedef struct t_connection_info {
	char* ip;
	uint32_t puerto;
}__attribute__ ((packed)) t_connection_info;


typedef struct {
	char data[DEFAULT_BUFFER_SIZE];
	int size;
} t_socket_buffer;  // por el momento esta aca, pero corresponde a socket...
					// pablo tenemos que hacer delegacion XD XD XD


//pablo en algun lado hay que poner esto, es algo generico,
// el viernes nos juntamos a definir el protocolo


t_mensaje* mensaje_create(uint8_t type);
void mensaje_create_and_send(uint8_t type, void* data, uint16_t length,
		t_socket_client* client);
t_socket_buffer* mensaje_serializer(t_mensaje* mensaje);
t_mensaje* mensaje_deserializer(t_socket_buffer* buffer, uint32_t dataStart);
t_mensaje* mensaje_recibir(t_socket_client* client);
void mensaje_destroy(t_mensaje* mensaje);
void mensaje_setdata(t_mensaje* mensaje, void* data, uint16_t length);
void* mensaje_getdata(t_mensaje* mensaje);
void mensaje_send(t_mensaje* mensaje, t_socket_client *client);

t_mensaje* mensaje_deserializer(t_socket_buffer* buffer,
		uint32_t offsetinBuffer) {
	int tmpsize, offset;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	memcpy(&mensaje->type, buffer->data, offset = sizeof(uint8_t));
	memcpy(&mensaje->length, buffer->data + offset, tmpsize = sizeof(uint16_t));
	offset += tmpsize;

	mensaje->payload = malloc(mensaje->length);
	memcpy(mensaje->payload, buffer->data + offset, mensaje->length);

	return mensaje;
}

t_socket_buffer* mensaje_serializer(t_mensaje* mensaje) {
	int tmpsize, offset = 0;
	t_socket_buffer* buffer = malloc(sizeof(t_socket_buffer));

	memcpy(buffer->data, &mensaje->type, tmpsize = sizeof(uint8_t));
	offset += tmpsize;

	memcpy(buffer->data + offset, &mensaje->length, tmpsize = sizeof(uint16_t));
	offset += tmpsize;

	memcpy(buffer->data + offset, mensaje->payload, mensaje->length);
	buffer->size = offset + mensaje->length;

	return buffer;
}

t_mensaje* mensaje_recibir(t_socket_client* client) {
	t_socket_buffer* buffer = sockets_recv(client);

	if (buffer == NULL ) {
		return NULL ;
	}

	t_mensaje* mensaje = mensaje_deserializer(buffer, 0);
	sockets_bufferDestroy(buffer);

	return mensaje;
}



void mensaje_destroy(t_mensaje* mensaje) {
	free(mensaje->payload);
	free(mensaje);
}

void mensaje_setdata(t_mensaje* mensaje, void* data, uint16_t length) {
	mensaje->length = length;
	mensaje->payload = data;
}

void* mensaje_getdata(t_mensaje* mensaje) {
	return mensaje->payload;
}

void mensaje_send(t_mensaje* mensaje, t_socket_client* client) {
	t_socket_buffer* buffer = mensaje_serializer(mensaje);
	//sockets_sendBuffer(client, buffer);// esto no existe o si pablo?
	//sockets_bufferDestroy(buffer);
	printf("ok se envio el mensaje =)");
}




t_mensaje* mensaje_create(uint8_t type) {
	t_mensaje* mensaje = (t_mensaje*) malloc(sizeof(t_mensaje));
	mensaje->type = type;
	mensaje->length = 0;
	mensaje->payload = NULL;

	return mensaje;
}

void mensaje_create_and_send(uint8_t type, void* data, uint16_t length,
		t_socket_client* client) {
	t_mensaje* mensaje = mensaje_create(type);
	mensaje_setdata(mensaje, data, length);
	mensaje_send(mensaje, client);
	mensaje_destroy(mensaje);
}
