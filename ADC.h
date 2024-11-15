/* 
 * File:   ADC.h
 * Author: Ariadna Osorio Uribe
 *
 * Created on November 15, 2024, 10:09 AM
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

void adc_init(void);
int adc_read(int channel);
int read_temperatura(void);
int read_fotoresistor(void);




#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

