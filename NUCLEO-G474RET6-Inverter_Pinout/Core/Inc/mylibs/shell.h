/*
 * shell.h
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */

#ifndef INC_MYLIBS_SHELL_H_
#define INC_MYLIBS_SHELL_H_

#define UART_RX_BUFFER_SIZE 1
#define UART_TX_BUFFER_SIZE 64
#define CMD_BUFFER_SIZE 64
#define MAX_ARGS 9
#define ASCII_LF 0x0A			// LF = line feed, saut de ligne
#define ASCII_CR 0x0D			// CR = carriage return, retour chariot
#define ASCII_BACK 0x08			// BACK = Backspace

#define NULL_SPEED 50
#define PRECISION 0.05f // Résistance de shunt en ohms
#define V_REF 3.3f   // Tension de référence de l'ADC
#define ADC_RESOLUTION 4096.0f // Résolution ADC 12 bits
#define OFFSET 1.65f

#define ADC_NOMINAL_SENSITIVITY 0.080f
#define ADC_VREF 3.3f
#define ADC_V_OFFSET 1.65f


void Shell_Init(void);
void Shell_Loop(void);
void setPWM(int dutycycle);

#endif /* INC_MYLIBS_SHELL_H_ */
