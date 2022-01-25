/**

LCD		STM32F4XX		DESCRIPTION

GND		GND				Ground
VCC		+5V				Power supply for LCD
V0		Potentiometer	Contrast voltage. Connect to potentiometer
RS		PA9				Register select
RW		GND				Read/write
E		PC7				Enable pin
D0		-				Data 0 - doesn't care
D1		-				Data 1 - doesn't care
D2		-				Data 2 - doesn't care
D3		-				Data 3 - doesn't  care
D4		PB5				Data 4
D5		PB4				Data 5
D6		PB10			Data 6
D7		PA8				Data 7
A		+3V3			Back light positive power
K		GND				Ground for back light
*/
#define D6_LCD_Pin GPIO_PIN_10
#define D6_LCD_GPIO_Port GPIOB
#define E_LCD_Pin GPIO_PIN_7
#define E_LCD_GPIO_Port GPIOC
#define D7_LCD_Pin GPIO_PIN_8
#define D7_LCD_GPIO_Port GPIOA
#define RS_LCD_Pin GPIO_PIN_9
#define RS_LCD_GPIO_Port GPIOA
#define D5_LCD_Pin GPIO_PIN_4
#define D5_LCD_GPIO_Port GPIOB
#define D4_LCD_Pin GPIO_PIN_5
#define D4_LCD_GPIO_Port GPIOB

#include "stm32f4xx_hal.h"

/**
 * @brief  Initializes HD44780 LCD
 * @brief  cols: width of lcd
 * @param  rows: height of lcd
 * @retval None
 */
void HD44780_Init(uint8_t cols, uint8_t rows);

/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void HD44780_DisplayOn(void);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void HD44780_DisplayOff(void);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void HD44780_Clear(void);

/**
 * @brief  Puts string on lcd
 * @param  x location
 * @param  y location
 * @param  *str: pointer to string to display
 * @retval None
 */
void HD44780_Puts(uint8_t x, uint8_t y, char* str);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void HD44780_BlinkOn(void);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void HD44780_BlinkOff(void);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void HD44780_CursorOn(void);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void HD44780_CursorOff(void);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void HD44780_ScrollLeft(void);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void HD44780_ScrollRight(void);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param *data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void HD44780_CreateChar(uint8_t location, uint8_t* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void HD44780_PutCustom(uint8_t x, uint8_t y, uint8_t location);

void HD44780_CursorSet(uint8_t col, uint8_t row);

