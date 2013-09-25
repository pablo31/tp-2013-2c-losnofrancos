/*
 * planificador.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_


struct s_planificador{
	int nro_nivel;

};
typedef struct s_planificador tad_planificador;

int planificador_numero_nivel(tad_planificador* planificador);

#endif /* PLANIFICADOR_H_ */
