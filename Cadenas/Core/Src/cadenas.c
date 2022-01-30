#include <cadenas.h>

RTC_HandleTypeDef hrtc;
uint8_t alarm = 0;

GestionCadenas cadenas = {
		.combinaisonCorrect = {0,0,0,0},
		.cursorVal = 0,
		.isOpen = 0,
		.numberBuffDisplay = {'0','0','0','0'},
		.numberDisplay = {0,0,0,0},
};

EncodeurStruct encodeur;

/* Copy a in b */
void copy(uint16_t *a, uint16_t *b)
{
	int i = 0;
	for(i = 0; i<4; i++)
	{
		b[i] = a[i];
	}
}

/* return 1 if a == b */
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

/* Initialize the LCD */
void initLCD(void)
{
	HD44780_Init(16, 2);
	HD44780_Puts(0, 0, "Lock"); //Put string to LCD
	HAL_Delay(1000);
	HD44780_CursorOn(); //Show cursor

	HD44780_Clear();
	HD44780_Puts(0, 0, "0");
	HD44780_Puts(1, 0, "0");
	HD44780_Puts(2, 0, "0");
	HD44780_Puts(3, 0, "0");
	HD44780_CursorSet(0,0);
	HD44780_Puts(0, 1, "close");
}



/* open the lock */
void open(void)
{
	TIM4->CCR2 = 25;
	HAL_Delay(2000);
}

/* Close the lock */
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
	}
	return 1;
}


/* Copy a in b with conversion ascii to int */
void changeTabUart(char *a, uint16_t *b)
{
	int i;
	for(i=0; i<4; i++)
	{
		b[i] = a[i+4]-48;
	}
}

// New combinaison by uart
void changeCombiUart(char *Rxbuff)
{
	TimeHeure date;
	changeTabUart(Rxbuff, cadenas.combinaisonCorrect);
	date = get_time();
	write_log(date.date);
	write_log(" ");
	write_log(date.time);
	write_log(" ");
	write_log("Nouvelle combinaison part uart: ");
	write_log(Rxbuff+4);
	write_log("\n");
}

/* Manage LCD's button */
void buttonManager(uint16_t adcVal,int debounce)
{
	// Nouvelle valeur de l'encodeur
	encodeur.encodeur = TIM3->CNT;
	if(adcVal>610 && adcVal<620) // bouton select
	{
		selectManager();
	}
	else if(adcVal>850 && adcVal<860) // bouton gauche
	{
		if((debounce == 0)&&(cadenas.cursorVal>0))
		{
			cadenas.cursorVal--;
			HAL_Delay(1000);
		}
		debounce++;
	}

	 else if(adcVal>815 && adcVal<825) // bouton droite
	 {
		 if((debounce == 0)&&(cadenas.cursorVal<3))
		 {
			 cadenas.cursorVal++;
			 HAL_Delay(1000);
		 }
		 debounce++;
	 }

	 else if(adcVal>925 && adcVal<935) // bouton haut
		 {
		 if((debounce == 0)&&(cadenas.numberDisplay[cadenas.cursorVal]<9))
		 {
			 cadenas.numberDisplay[cadenas.cursorVal]++;
			 sprintf(cadenas.numberBuffDisplay+cadenas.cursorVal,"%d",
					 cadenas.numberDisplay[cadenas.cursorVal]);
		 }
		 debounce++;
	 }
	 else if(adcVal>895 && adcVal<905) // bouton bas
	 {
		 if((debounce == 0)&&(cadenas.numberDisplay[cadenas.cursorVal]>0))
		 {
			 cadenas.numberDisplay[cadenas.cursorVal]--;
			 sprintf(cadenas.numberBuffDisplay+cadenas.cursorVal,"%d",cadenas.numberDisplay[cadenas.cursorVal]);
		 }
		 debounce++;
	 }
	 else if(encodeur.encodeur!=encodeur.encodeur_ref)
	 {

		 if(encodeur.encodeur<encodeur.encodeur_ref) // encodeur decrement
		 {
			 if(cadenas.numberDisplay[cadenas.cursorVal]>0)
			 {
				 cadenas.numberDisplay[cadenas.cursorVal]--;
		 		 sprintf(cadenas.numberBuffDisplay+cadenas.cursorVal,"%d",cadenas.numberDisplay[cadenas.cursorVal]);
		 	 }
		  }
		  else // encodeur increment
		  {
			  if(cadenas.numberDisplay[cadenas.cursorVal]<9)
			  {
			  	cadenas.numberDisplay[cadenas.cursorVal]++;
				  sprintf(cadenas.numberBuffDisplay+cadenas.cursorVal,"%d",cadenas.numberDisplay[cadenas.cursorVal]);
			  }
		  }
		  encodeur.encodeur_ref = encodeur.encodeur;
	 }
	 else
	 {
		 debounce = 0;
	 }
}

/* Manage select button */
void selectManager(void)
{
	TimeHeure date;
	if(cadenas.isOpen == 0) // Le cadenas est fermé
	{
		if(compare(cadenas.combinaisonCorrect,cadenas.numberDisplay)) // La combinaison est correct
		{
			date = get_time();
			write_log(date.date);
			write_log(" ");
			write_log(date.time);
			write_log(" ");
			write_log("Bonne combinaison: ");
			write_log(cadenas.numberBuffDisplay);
			write_log("\n");

			write_log(date.date);
			write_log(" ");
			write_log(date.time);
			write_log(" ");
			write_log("Ouverture du cadenas\n");
			HD44780_Clear();
			HD44780_Puts(0, 1, "open");
			open();
			cadenas.isOpen = 1;
		}
		else // The combinaison is false
		{
			date = get_time();
			write_log(date.date);
			write_log(" ");
			write_log(date.time);
			write_log(" ");
			write_log("Mauvaise combinaison: ");
			write_log(cadenas.numberBuffDisplay);
			write_log("\n");
			HD44780_Puts(0, 1, "close");
			close();
		}
	}
	else // The lock is open
	{
		date = get_time();
		write_log(date.date);
		write_log(" ");
		write_log(date.time);
		write_log(" ");
		write_log("Nouvelle combinaison par LCD: ");
		write_log(cadenas.numberBuffDisplay);
		write_log("\n");

		write_log(date.date);
		write_log(" ");
		write_log(date.time);
		write_log(" ");
		write_log("Fermeture du cadenas\n");
		copy(cadenas.numberDisplay,cadenas.combinaisonCorrect);
		cadenas.numberDisplay[0] = 0;
		cadenas.numberDisplay[1] = 0;
		cadenas.numberDisplay[2] = 0;
		cadenas.numberDisplay[3] = 0;
		sprintf(cadenas.numberBuffDisplay+0,"%d",cadenas.numberDisplay[0]);
		sprintf(cadenas.numberBuffDisplay+1,"%d",cadenas.numberDisplay[1]);
		sprintf(cadenas.numberBuffDisplay+2,"%d",cadenas.numberDisplay[2]);
		sprintf(cadenas.numberBuffDisplay+3,"%d",cadenas.numberDisplay[3]);
		close();
		HD44780_Puts(0, 1, "close");
		cadenas.isOpen = 0;
	}
}

/* Refresh the lcd */
void miseAJourLcd(void)
{
	 // Display the digits
	HD44780_Puts(cadenas.cursorVal, 0, &cadenas.numberBuffDisplay[cadenas.cursorVal]);
	HD44780_CursorSet(cadenas.cursorVal,0);
}

/* Manage the button of encoder */
void ManageButtonEncoder(int debounce)
{
	if(debounce == 0)
		{
			// Décale vers la droite le digit selectionné
			if(cadenas.cursorVal<3)
			{
				cadenas.cursorVal++;
				debounce++;
			}

			// Teste la combinaison
			else if(cadenas.cursorVal == 3)
			{
				//copy(numberDisplay,combinaisonCorrect);
				selectManager();
			}
		}
		debounce = 0;
}

void CallUart(char *Rxbuff)
{
	// Si les 4 premiers digits sont égaux à la combinaison pour ouvrir le cadenas
	if(verifCombiUart(Rxbuff,cadenas.numberDisplay))
	{
		changeCombiUart(Rxbuff);
	}
}

/* Write log into the SD card */
void write_log(char *log)
{
	FATFS myFATFS;
	FIL myFile;
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

/* Set the time into the RTC */
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

/* Get the time into the RTC */
TimeHeure get_time(void)
{
	TimeHeure date;
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

  /* Display time Format: hh:mm:ss */
  sprintf((char*)date.time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);

  /* Display date Format: mm-dd-yy */
  sprintf((char*)date.date,"%02d-%02d-%2d",gDate.Date, gDate.Month, 2000 + gDate.Year);  // I like the date first
  return date;
}
