/*
 * paquete_tipo_dato.h
 *
 *  Created on: May 21, 2013
 *      Author: pablo
 */

#ifndef PAQUETE_TIPO_DATO_H_
#define PAQUETE_TIPO_DATO_H_

//Aca se definen los valores del primer byte cuando enviamos paquetes (expected package data type)
//Los valores disponibles van de 0 a 255 (dado que tratamos con tipo de dato byte o uchar)
//Recordar que no se puede utilizar el 0. Esta reservado para casos de error
//Cualquier cambio en este enum afecta directamente a todos los procesos, ya que todos comparten este .h

enum DATA_TYPE_PROTOCOL {
	//handshake
	PRESENTACION_PERSONAJE = 1,
	PRESENTACION_ORQUESTADOR = 2,
	PRESENTACION_PLANIFICADOR = 3,
	PRESENTACION_NIVEL = 4,

	//personaje - orquestador
	PERSONAJE_OBJETIVOS_COMPLETADOS = 100,
	PERSONAJE_SOLICITUD_NIVEL = 101,
	PERSONAJE_NOMBRE = 102,
	PERSONAJE_SIMBOLO = 103,

	//personaje - planificador
	PERSONAJE_MOVIMIENTO = 105,
	PERSONAJE_SOLICITUD_RECURSO = 106,

	//nivel - planificador
	NIVEL_NUMERO = 150,

	//planificador - personaje
	PLANIFICADOR_OTORGA_QUANTUM = 200,

	//mensajes compartidos entre planificador, nivel, y personaje
	SOLICITUD_UBICACION_RECURSO = 104,
	OTORGAR_RECURSO = 220,
	MOVIMIENTO_CONFIRMADO = 221
};

#endif /* PAQUETE_TIPO_DATO_H_ */
