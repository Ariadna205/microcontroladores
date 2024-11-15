/* 
 * File:   monitoreo_sensores.h
 * Author: Ariadna Osorio Uribe
 *
 * Created on November 15, 2024, 10:21 AM
 */

#ifndef MONITOREO_SENSORES_H
#define	MONITOREO_SENSORES_H

#ifdef	__cplusplus
extern "C" {
#endif

void sensores_init(void);
int read_hall(void);
int read_movimiento(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MONITOREO_SENSORES_H */

