#include "fatfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4_hd44780.h"

#define RXBUFF_SIZE 8
#define FILEPATH "/"

extern char numberBuffDisplay[4];
extern uint16_t numberDisplay[4];
extern uint16_t adcVal;
extern uint16_t adc_val_verif;
extern uint16_t cursorVal;
extern uint16_t combinaisonCorrect[4];
extern int debounce;
extern uint16_t changeUart;
extern char Rxbuff[RXBUFF_SIZE];
extern char state[10];
extern uint32_t encodeur;
extern RTC_HandleTypeDef hrtc;
extern uint8_t alarm;
extern char time[10];
extern char date[10];

void copy(uint16_t *a, uint16_t *b);
int compare(uint16_t *a, uint16_t *b);
void initLCD(void);
void changeCombiUart(char *Rxbuff,uint16_t *deverouill);
int verifCombiUart(char *a, uint16_t *b);
void open(void);
void close(void);
void buttonManager(void);
void selectManager(void);
void write_log(char *log);
void set_alarm (void);
void get_time(void);
