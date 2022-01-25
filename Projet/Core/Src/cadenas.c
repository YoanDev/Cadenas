#include <cadenas.h>


/* Variables */
char numberBuffDisplay[4] = {'0','0','0','0'}; // Combinaison en char
uint16_t numberDisplay[4] = {0,0,0,0}; // Combinaison en entier
uint16_t cursorVal = 0; // Mise en évidence du digit selectionné

uint16_t adcVal; // bouton sélectionné
uint16_t adc_val_verif; // anti rebond adc

uint16_t combinaisonCorrect[4] = {0,0,0,0};
uint16_t valSelected[4] = {0,0,0,0};
uint16_t isOpen = 0; // close or open lock
int debounce = 0;
char state[10];

uint32_t encodeur = 0;
uint32_t encodeur_ref;

char Rxbuff[RXBUFF_SIZE];
uint16_t changeUart = 0;

FATFS myFATFS;
FIL myFile;
RTC_HandleTypeDef hrtc;
char time[10];
char date[10];
uint8_t alarm = 0;

/* Fonctions secondaires */
void copy(uint16_t *a, uint16_t *b)
{
	int i = 0;
	for(i = 0; i<4; i++)
	{
		b[i] = a[i];
	}
}

int compare(uint16_t *a, uint16_t *b)
{
	int i = 0;
	for(i = 0; i<4; i++)
	{
		if(b[i] != a[i])
		return 0;
	}
	return 1;
}

/* Fonctions principales */

// Initialise l'écran LCD
void initLCD(void)
{
	HD44780_Init(16, 2);
	HD44780_Puts(0, 0, "Cadenas"); //Put string to LCD
	HAL_Delay(1000);
	HD44780_CursorOn(); //Show cursor

	HD44780_Clear();
	HD44780_Puts(0, 0, "0");
	HD44780_Puts(1, 0, "0");
	HD44780_Puts(2, 0, "0");
	HD44780_Puts(3, 0, "0");
	HD44780_CursorSet(0,0);
}

// Ouvre le cadenas
void open(void)
{
	TIM4->CCR2 = 25;
	HAL_Delay(2000);
}

// Ferme le cadenas
void close(void)
{
	TIM4->CCR2 = 75;
	HAL_Delay(2000);
}

// Vérifie que les paramètres sont égaux
int verifCombiUart(char *a, uint16_t *b)
{
	int i = 0;
	for(i = 0; i<4; i++)
	{
		if(b[i] != a[i]-48) // conversion char vers int avec (-48)
		{
			return 0;
		}
		//changeUart = a[i]; // Pour debug
	}
	return 1;
}

// Copie a dans b
void changeCombiUart(char *a, uint16_t *b)
{
	int i;
	for(i=0; i<4; i++)
	{
		b[i] = a[i+4]-48;
	}
}

// Gère les boutons poussoirs
void buttonManager()
{
	 if(adcVal<980 && adcVal>970) // bouton select
	 {
		selectManager();
	 }
	 else if(adcVal<630 && adcVal>620) // bouton gauche
	 {
		 if((debounce == 0)&&(cursorVal>0))
		 {
			 cursorVal--;
			 HAL_Delay(1000);
		 }
		 debounce++;
	 }

	 else if(adcVal < 10) // bouton droite
	 {
		 if((debounce == 0)&&(cursorVal<3))
		 {
			 cursorVal++;
			 HAL_Delay(1000);
		 }
		 debounce++;
	 }

	 else if(adcVal<160 && adcVal>150) // bouton haut
		 {
		 if((debounce == 0)&&(numberDisplay[cursorVal]<9))
		 {
			 numberDisplay[cursorVal]++;
			 sprintf(numberBuffDisplay+cursorVal,"%d",numberDisplay[cursorVal]);
		 }
		 debounce++;
	 }
	 else if(adcVal<400 && adcVal>390) // bouton bas
	 {
		 if((debounce == 0)&&(numberDisplay[cursorVal]>0))
		 {
			 numberDisplay[cursorVal]--;
			 sprintf(numberBuffDisplay+cursorVal,"%d",numberDisplay[cursorVal]);
		 }
		 debounce++;
	 }
	 else if(encodeur!=encodeur_ref)
	 {

		 if(encodeur<encodeur_ref) // encodeur decrement
		 {
			 if(numberDisplay[cursorVal]>0)
			 {
				 numberDisplay[cursorVal]--;
		 		 sprintf(numberBuffDisplay+cursorVal,"%d",numberDisplay[cursorVal]);
		 	 }
		  }
		  else // encodeur increment
		  {
			  if(numberDisplay[cursorVal]<9)
			  {
			  	numberDisplay[cursorVal]++;
				  sprintf(numberBuffDisplay+cursorVal,"%d",numberDisplay[cursorVal]);
			  }
		  }
		  encodeur_ref = encodeur;
	 }
}

// Gère l'ouverture et la fermeture du cadenas
void selectManager(void)
{
	if(isOpen == 0) // Le cadenas est fermé
	{
		if(compare(combinaisonCorrect,numberDisplay)) // La combinaison est correct
		{
			get_time();
			write_log(date);
			write_log(" ");
			write_log(time);
			write_log(" ");
			write_log("Bonne combinaison: ");
			write_log(numberBuffDisplay);
			write_log("\n");

			write_log(date);
			write_log(" ");
			write_log(time);
			write_log(" ");
			write_log("Ouverture du cadenas\n");
			strcpy(state,"");
			strcpy(state,"ouvert");
			open();
			isOpen = 1;
		}
		else // La combinaison est incorrect
		{
			get_time();
			write_log(date);
			write_log(" ");
			write_log(time);
			write_log(" ");
			write_log("Mauvaise combinaison: ");
			write_log(numberBuffDisplay);
			write_log("\n");

			strcpy(state,"");
			strcpy(state,"ferme");
			HD44780_Puts(0, 1, "");
			close();
		}
	}
	else // Le cadenas est ouvert
	{
		get_time();
		write_log(date);
		write_log(" ");
		write_log(time);
		write_log(" ");
		write_log("Nouvelle combinaison par LCD: ");
		write_log(numberBuffDisplay);
		write_log("\n");

		write_log(date);
		write_log(" ");
		write_log(time);
		write_log(" ");
		write_log("Fermeture du cadenas\n");
		copy(numberDisplay,combinaisonCorrect);
		numberDisplay[0] = 0;
		numberDisplay[1] = 0;
		numberDisplay[2] = 0;
		numberDisplay[3] = 0;
		sprintf(numberBuffDisplay+0,"%d",numberDisplay[0]);
		sprintf(numberBuffDisplay+1,"%d",numberDisplay[1]);
		sprintf(numberBuffDisplay+2,"%d",numberDisplay[2]);
		sprintf(numberBuffDisplay+3,"%d",numberDisplay[3]);
		close();
		strcpy(state,"");
		strcpy(state,"ferme");
		isOpen = 0;
	}
}

// Ecrit des logs dans le fichier sur la carte SD
void write_log(char *log)
{
	UINT testByte;
	FRESULT fres;
	char fileName[12] = {0};
	char fileTitle[36] = {0};

  sprintf(fileName,"STMlecon.txt");
  sprintf(fileTitle,log);

  fres = f_mount(&myFATFS,"",1);
  if(fres == FR_OK)
  {
  	HAL_Delay(10);
  	f_mkdir(FILEPATH);
  	HAL_Delay(10);
  	fres = f_open(&myFile, fileName, FA_WRITE | FA_OPEN_APPEND);
  	if(fres == FR_OK)
  	{
  		HAL_Delay(10);
  		fres = f_write(&myFile, fileTitle, strlen(fileTitle), &testByte);
  		if(fres == FR_OK)
  		{
  			HAL_Delay(10);
  			fres = f_close(&myFile);
  		}
  	}
  }
}

// Initialise une date et une heure sur la RTC
void set_time (void)
{
	  RTC_TimeTypeDef sTime;
	  RTC_DateTypeDef sDate;
    /**Initialize RTC and set the Time and Date
    */
  sTime.Hours = 0x10;
  sTime.Minutes = 0x20;
  sTime.Seconds = 0x30;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }
  /* USER CODE BEGIN RTC_Init 3 */

  /* USER CODE END RTC_Init 3 */

  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
  sDate.Month = RTC_MONTH_AUGUST;
  sDate.Date = 0x12;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }
  /* USER CODE BEGIN RTC_Init 4 */

  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);  // backup register

  /* USER CODE END RTC_Init 4 */
}

void set_alarm (void)
{
	  RTC_AlarmTypeDef sAlarm;

    /**Enable the Alarm A
    */
  sAlarm.AlarmTime.Hours = 0x10;
  sAlarm.AlarmTime.Minutes = 0x21;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x12;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }
  /* USER CODE BEGIN RTC_Init 5 */

  /* USER CODE END RTC_Init 5 */
}

// Récupère la date et l'heure sur la RTC
void get_time(void)
{
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

  /* Display time Format: hh:mm:ss */
  sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);

  /* Display date Format: mm-dd-yy */
  sprintf((char*)date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);  // I like the date first
}
