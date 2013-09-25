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
	PRESENTACION_PERSONAJE = 1,
	PRESENTACION_ORQUESTADOR = 2,
	PRESENTACION_PLANIFICADOR = 3,
	PRESENTACION_NIVEL = 4,

	PERSONAJE_OBJETIVOS_COMPLETOS = 10,
	PERSONAJE_OBJETIVOS_INCOMPLETOS = 11,

	NIVEL_NUMERO = 50,

	PERSONAJE_OBJETIVOS_COMPLETADOS = 100,
	PERSONAJE_SOLICITUD_NIVEL = 101,

	PUERTO_PLANIFICADOR = 150,
	IPPUERTO_NIVEL = 151,

	PERSONAJE_CONECTADO_A_NIVEL = 201
};

#endif /* PAQUETE_TIPO_DATO_H_ */
