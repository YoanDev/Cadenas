#include "fatfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4_hd44780.h"


#define FILEPATH "/"



extern RTC_HandleTypeDef hrtc;

typedef struct
{
	char numberBuffDisplay[4]; // Combinaison en char
	uint16_t numberDisplay[4]; // Combinaison en entier
	uint16_t cursorVal; // Mise en évidence du digit selectionné
	uint16_t combinaisonCorrect[4];
	uint16_t isOpen; // close or open lock
}GestionCadenas;

typedef struct
{
	uint32_t encodeur;
	uint32_t encodeur_ref;
}EncodeurStruct;

typedef struct
{
	char time[10];
	char date[10];
}TimeHeure;

void copy(uint16_t *a, uint16_t *b);
int compare(uint16_t *a, uint16_t *b);
void initLCD(void);
void changeCombiUart(char *Rxbuff);
int verifCombiUart(char *a, uint16_t *b);
void open(void);
void close(void);
void buttonManager(uint16_t adcVal,int debounce);
void selectManager(void);
void miseAJourLcd(void);
void ManageButtonEncoder(int debounce);
void CallUart(char *Rxbuff);
void write_log(char *log);
void set_alarm (void);
TimeHeure get_time(void);
