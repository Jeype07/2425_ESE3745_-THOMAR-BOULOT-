/*
 * shell.c
 *
 *  Created on: Oct 1, 2023
 *      Author: nicolas
 */
#include "usart.h"
#include "mylibs/shell.h"
#include "stm32g4xx_hal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tim.h"

uint8_t prompt[]="user@Nucleo-STM32G474RET6>>";
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n";
uint8_t newline[]="\r\n";
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n";
uint8_t brian[]="Brian is in the kitchen\r\n";
uint8_t uartRxReceived;
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];

extern  uint32_t buffer;
uint32_t adc_val = 0;
extern ADC_HandleTypeDef hadc1;

char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;
char* 		command[4] = {"WhereisBrian?","start","stop","speed"};
int 		nbcommand = 4;
int 		pas = 1;		//pas de modification de la vitesse du moteur
int 		delai = 100;	//delai entre chaque modification de vitesse du moteur en ms
int 		trigger = 50; 	//valeur du rapport après start
int 		percentage = 50;//valeur du rapport après start

void Shell_Init(void){
	memset(argv, 0, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, 0, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, 0, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, 0, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

void Shell_Loop(void){
	if(uartRxReceived){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}

	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"help")==0){

			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Print all available functions here\r\n");
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);

			for (size_t i = 0; i < nbcommand; i++) {
				// Convertir l'élément du tableau en chaîne de caractères
				int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "%s\r\n", command[i]);
				// Transmettre le texte via UART
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}

		}

		else if(strcmp(argv[0],"start")==0){	//lancement des PWM
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Power on\r\n");
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
			setPWM(NULL_SPEED);
		}
		else if(strcmp(argv[0],"stop")==0){		//arrêt des PWM après arrêt progressif du moteur
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Power off\r\n");
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			if(percentage != NULL_SPEED){
				while(percentage != NULL_SPEED){
					if(percentage > NULL_SPEED){
						percentage -= pas;
						setPWM(percentage);
					}
					else{
						percentage += pas;
						setPWM(percentage);
					}
					HAL_Delay(delai);
				}
			}
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
			HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
			HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
		}

		else if(argc == 2 && strcmp(argv[0], "speed") == 0){	//commande de vitesse du moteur avec changement progressif
			percentage = atoi(argv[1]);  // Convertit l'argument en pourcentage

			if(percentage>=0 && percentage<=100){	//test sur la valeur de vitesse entrée

				while(trigger!=percentage){
					if(trigger>percentage){
						trigger -= pas;
						setPWM(trigger);
					}
					else{
						trigger += pas;
						setPWM(trigger);
					}
					HAL_Delay(delai);
				}
			}
			else{
				int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Value must be between 0 and 100\r\n");
				HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			}
		}
		else if(strcmp(argv[0],"current")==0){		//commande pour demander l'affichage du courant
			float voltage = V_REF * adc_val  / ADC_RESOLUTION;
			float current = voltage - OFFSET / PRECISION;
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "current : %f  %lu\r\n", current, adc_val);
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}

		else if(strcmp(argv[0],"currentpool")==0){
			HAL_ADC_Start(&hadc1);
			uint32_t adc_value = 0;
			float voltage = 0.0f;
			float current = 0.0f;
			if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
				adc_value = HAL_ADC_GetValue(&hadc1);
				voltage = (adc_value * ADC_VREF) / ADC_RESOLUTION;
				current = (voltage - ADC_V_OFFSET) / ADC_NOMINAL_SENSITIVITY;
			}
			HAL_ADC_Stop(&hadc1);
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "current : %f A\r\n", current);
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
		}

		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	uartRxReceived = 1;
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	adc_val = buffer;
}

void setPWM(int dutycycle){		//règle le rapport cyclique des PWM
	int val_CCR = (TIM1->ARR*dutycycle)/100;
	TIM1->CCR1=val_CCR;
	TIM1->CCR2=TIM1->ARR-val_CCR;
}

