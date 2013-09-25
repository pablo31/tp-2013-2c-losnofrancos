/*
 * plataforma.h
 *
 *  Created on: Sep 25, 2013
 *      Author: pablo
 */

#ifndef PLATAFORMA_H_
#define PLATAFORMA_H_

struct s_plataforma{
	tad_logger* logger;
	//tad_orquestador* orquestador;
	t_list* planificadores;
};
typedef struct s_plataforma tad_plataforma;

#endif /* PLATAFORMA_H_ */
