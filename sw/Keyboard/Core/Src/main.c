/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// HID Keyboard
struct keyboardHID_t {
	uint8_t modifiers;
	uint8_t reserved;
	uint8_t key1;
	uint8_t key2;
	uint8_t key3;
	uint8_t key4;
	uint8_t key5;
	uint8_t key6;

};

// SK6812-E
typedef union
{
  struct
  {
    uint8_t g;
    uint8_t r;
    uint8_t b;
  } color;
  uint32_t data;
} PixelRGB_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define NUM_ROWS 5

#define NEOPIXEL_ZERO 9
#define NEOPIXEL_ONE 19
#define NUM_PIXELS 14
#define DMA_BUFF_SIZE (NUM_PIXELS * 32) + 1

//*****************************************************************************
//
// HID Keyboard LED definitions as used in the first byte of the output report
// used in the BIOS keyboard protocol.
//
//*****************************************************************************
#define HID_KEYB_NUM_LOCK       0x01
#define HID_KEYB_CAPS_LOCK      0x02
#define HID_KEYB_SCROLL_LOCK    0x04
#define HID_KEYB_COMPOSE        0x08
#define HID_KEYB_KANA           0x10

//*****************************************************************************
//
// HID Keyboard key modifiers as provided in the first byte of the input report
// used in the BIOS keyboard protocol.
//
//*****************************************************************************
#define HID_KEYB_LEFT_CTRL      0x01
#define HID_KEYB_LEFT_SHIFT     0x02
#define HID_KEYB_LEFT_ALT       0x04
#define HID_KEYB_LEFT_GUI       0x08
#define HID_KEYB_RIGHT_CTRL     0x10
#define HID_KEYB_RIGHT_SHIFT    0x20
#define HID_KEYB_RIGHT_ALT      0x40
#define HID_KEYB_RIGHT_GUI      0x80

//*****************************************************************************
//
// A subset of the HID keyboard usage IDs.
//
//*****************************************************************************
#define HID_KEYB_USAGE_RESERVED 0x00
#define HID_KEYB_USAGE_ROLLOVER 0x01
#define HID_KEYB_USAGE_A        0x04
#define HID_KEYB_USAGE_B        0x05
#define HID_KEYB_USAGE_C        0x06
#define HID_KEYB_USAGE_D        0x07
#define HID_KEYB_USAGE_E        0x08
#define HID_KEYB_USAGE_F        0x09
#define HID_KEYB_USAGE_G        0x0A
#define HID_KEYB_USAGE_H        0x0B
#define HID_KEYB_USAGE_I        0x0C
#define HID_KEYB_USAGE_J        0x0D
#define HID_KEYB_USAGE_K        0x0E
#define HID_KEYB_USAGE_L        0x0F
#define HID_KEYB_USAGE_M        0x10
#define HID_KEYB_USAGE_N        0x11
#define HID_KEYB_USAGE_O        0x12
#define HID_KEYB_USAGE_P        0x13
#define HID_KEYB_USAGE_Q        0x14
#define HID_KEYB_USAGE_R        0x15
#define HID_KEYB_USAGE_S        0x16
#define HID_KEYB_USAGE_T        0x17
#define HID_KEYB_USAGE_U        0x18
#define HID_KEYB_USAGE_V        0x19
#define HID_KEYB_USAGE_W        0x1A
#define HID_KEYB_USAGE_X        0x1B
#define HID_KEYB_USAGE_Y        0x1C
#define HID_KEYB_USAGE_Z        0x1D
#define HID_KEYB_USAGE_1        0x1E
#define HID_KEYB_USAGE_2        0x1F
#define HID_KEYB_USAGE_3        0x20
#define HID_KEYB_USAGE_4        0x21
#define HID_KEYB_USAGE_5        0x22
#define HID_KEYB_USAGE_6        0x23
#define HID_KEYB_USAGE_7        0x24
#define HID_KEYB_USAGE_8        0x25
#define HID_KEYB_USAGE_9        0x26
#define HID_KEYB_USAGE_0        0x27
#define HID_KEYB_USAGE_ENTER    0x28
#define HID_KEYB_USAGE_ESCAPE   0x29
#define HID_KEYB_USAGE_BACKSPACE 0x2A
#define HID_KEYB_USAGE_TAB      0x2B
#define HID_KEYB_USAGE_SPACE    0x2C
#define HID_KEYB_USAGE_MINUS    0x2D
#define HID_KEYB_USAGE_EQUAL    0x2E
#define HID_KEYB_USAGE_LBRACKET 0x2F
#define HID_KEYB_USAGE_RBRACKET 0x30
#define HID_KEYB_USAGE_BSLASH   0x31
#define HID_KEYB_USAGE_SEMICOLON 0x33
#define HID_KEYB_USAGE_FQUOTE   0x34
#define HID_KEYB_USAGE_BQUOTE   0x35
#define HID_KEYB_USAGE_COMMA    0x36
#define HID_KEYB_USAGE_PERIOD   0x37
#define HID_KEYB_USAGE_FSLASH   0x38
#define HID_KEYB_USAGE_CAPSLOCK 0x39
#define HID_KEYB_USAGE_F1       0x3A
#define HID_KEYB_USAGE_F2       0x3B
#define HID_KEYB_USAGE_F3       0x3C
#define HID_KEYB_USAGE_F4       0x3D
#define HID_KEYB_USAGE_F5       0x3E
#define HID_KEYB_USAGE_F6       0x3F
#define HID_KEYB_USAGE_F7       0x40
#define HID_KEYB_USAGE_F8       0x41
#define HID_KEYB_USAGE_F9       0x42
#define HID_KEYB_USAGE_F10      0x43
#define HID_KEYB_USAGE_F11      0x44
#define HID_KEYB_USAGE_F12      0x45
#define HID_KEYB_USAGE_SCROLLOCK   0x47
#define HID_KEYB_USAGE_PAGE_UP  0x4B
#define HID_KEYB_USAGE_PAGE_DOWN   0x4E
#define HID_KEYB_USAGE_RIGHT_ARROW 0x4F
#define HID_KEYB_USAGE_LEFT_ARROW 0x50
#define HID_KEYB_USAGE_DOWN_ARROW 0x51
#define HID_KEYB_USAGE_UP_ARROW 0x52
#define HID_KEYB_USAGE_NUMLOCK  0x53
#define HID_KEYB_USAGE_KEYPAD_SLASH  0x54
#define HID_KEYB_USAGE_KEYPAD_STAR 0x55
#define HID_KEYB_USAGE_KEYPAD_MINUS 0x56
#define HID_KEYB_USAGE_KEYPAD_PLUS 0x57
#define HID_KEYB_USAGE_KEPAD_ENTER 0x58
#define HID_KEYB_USAGE_KEYPAD_1 0x59
#define HID_KEYB_USAGE_KEYPAD_2 0x5A
#define HID_KEYB_USAGE_KEYPAD_3 0x5B
#define HID_KEYB_USAGE_KEYPAD_4 0x5C
#define HID_KEYB_USAGE_KEYPAD_5 0x5D
#define HID_KEYB_USAGE_KEYPAD_6 0x5E
#define HID_KEYB_USAGE_KEYPAD_7 0x5F
#define HID_KEYB_USAGE_KEYPAD_8 0x60
#define HID_KEYB_USAGE_KEYPAD_9 0x61
#define HID_KEYB_USAGE_KEYPAD_0 0x62
#define HID_KEYB_USAGE_KEPAD_PERIOD 0x63

#define WINDOWS_KEY 0x65
#define LEFT_SHIFT 0x66
#define RIGHT_SHIFT 0x67
#define LEFT_CTRL 0x68
#define RIGHT_CTRL 0x69
#define LEFT_GUI 0x6A
#define RIGHT_GUI 0x6B
#define LEFT_ALT 0x6C
#define RIGHT_ALT 0x6D
#define CAPS_LOCK 0x6E
#define BLUETOOTH 0x6F
#define PAUSE 0xE8
#define DELETE 0x4C
#define HOME 0x4A
#define END 0x4D
#define INSERT 0x49
#define MUTE 0xEF
#define PREV 0xEA
#define NEXT 0xEB
#define CALC 0xFB
#define VOL_UP 0xED
#define VOL_DN 0xEE
#define APP_A 0xEF
#define APP_B 0xF0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef hlpuart1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch4;
DMA_HandleTypeDef hdma_tim2_ch1;
DMA_HandleTypeDef hdma_tim2_ch2;
DMA_HandleTypeDef hdma_tim2_ch3;
DMA_HandleTypeDef hdma_tim3_ch3;

/* USER CODE BEGIN PV */


struct keyboardHID_t keyboardHID = {0,0,0,0,0,0,0,0};

uint8_t KeyboardMatrix[5][14] = {
    {HID_KEYB_USAGE_ESCAPE,  HID_KEYB_USAGE_1, HID_KEYB_USAGE_2, HID_KEYB_USAGE_3, HID_KEYB_USAGE_4, HID_KEYB_USAGE_5, HID_KEYB_USAGE_6, HID_KEYB_USAGE_7, HID_KEYB_USAGE_8, HID_KEYB_USAGE_9, HID_KEYB_USAGE_0, HID_KEYB_USAGE_MINUS, HID_KEYB_USAGE_EQUAL, HID_KEYB_USAGE_BACKSPACE},
    {HID_KEYB_USAGE_TAB, HID_KEYB_USAGE_Q, HID_KEYB_USAGE_W, HID_KEYB_USAGE_E, HID_KEYB_USAGE_R, HID_KEYB_USAGE_T, HID_KEYB_USAGE_Y, HID_KEYB_USAGE_U, HID_KEYB_USAGE_I, HID_KEYB_USAGE_O, HID_KEYB_USAGE_P, HID_KEYB_USAGE_LBRACKET, HID_KEYB_USAGE_RBRACKET, HID_KEYB_USAGE_BSLASH},
    {HID_KEYB_USAGE_CAPSLOCK, HID_KEYB_USAGE_A, HID_KEYB_USAGE_S, HID_KEYB_USAGE_D, HID_KEYB_USAGE_F, HID_KEYB_USAGE_G, HID_KEYB_USAGE_H, HID_KEYB_USAGE_J, HID_KEYB_USAGE_K, HID_KEYB_USAGE_L, HID_KEYB_USAGE_SEMICOLON, HID_KEYB_USAGE_FQUOTE, HID_KEYB_USAGE_ENTER, HID_KEYB_USAGE_UP_ARROW},
    {LEFT_SHIFT, HID_KEYB_USAGE_Z, HID_KEYB_USAGE_X, HID_KEYB_USAGE_C, HID_KEYB_USAGE_V, HID_KEYB_USAGE_B, HID_KEYB_USAGE_N, HID_KEYB_USAGE_M, HID_KEYB_USAGE_COMMA, HID_KEYB_USAGE_PERIOD, HID_KEYB_USAGE_FSLASH, RIGHT_SHIFT, HID_KEYB_USAGE_LEFT_ARROW, HID_KEYB_USAGE_DOWN_ARROW},
    {LEFT_CTRL, LEFT_GUI, LEFT_ALT, 0, 0, HID_KEYB_USAGE_SPACE, 0, 0, 0, RIGHT_ALT, 0, HID_KEYB_RIGHT_GUI, RIGHT_CTRL, HID_KEYB_USAGE_RIGHT_ARROW}
  };

uint8_t matrix[5][14][6] = {
		{"Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "=", "-", "Back"},
		{"Tab","Q","W","E","R","T","Y","U","I","O","P","[","]","\\"},
		{"Caps","A","S","D","F","G","H","J","K","L",";","'","\n\r","Up"},
		{"LShift","Z","X","C","V","B","N","M",",",".","/","RShift","Left","Down"},
		{"LCtrl","Win","LAlt","", "", " ","","","", "RAlt","Fn","Menu","RCtrl","Right"},
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ICACHE_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
		HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
	} else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2){
		HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_2);
	} else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
		HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_3);
	} else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4){
		HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_4);
	}
}

void AddKeyToPressedList(uint8_t keycode) {

  for (int i = 2; i < 8; i++){
    //
    // Is this key already included in the list of keys in the pressed
    // state?
    //
    if (((uint8_t*)&keyboardHID)[i] == keycode){
      //
      // Yes - drop out.
      //
      return;
    }
  }

  // not already holding, so lets add it!

  for (int i = 2; i < 8; i++){
      if (((uint8_t*)&keyboardHID)[i] == 0){
    	  ((uint8_t*)&keyboardHID)[i] = keycode;
    	  return;
      }
  }
}

void RemoveKeyFromPressedList(uint8_t keycode) {
  //
  // Find the usage code in the current list.
  //
  for (int i = 0; i < 6; i++)
  {
    if (((uint8_t*)&keyboardHID)[i] == keycode)
    {
      // found the key, shuffle everything rightward
		for (int j = i + 1; j < 8; j++){
			((uint8_t*)&keyboardHID)[j-1] = ((uint8_t*)&keyboardHID)[j];
		}
		((uint8_t*)&keyboardHID)[7] = 0; // remove a possible duplicate
  }
}
}

void PressKey(uint8_t c)
{

  switch (c)
  {
  case LEFT_SHIFT:
    keyboardHID.modifiers |= HID_KEYB_LEFT_SHIFT;
    break;
  case RIGHT_SHIFT:
	  keyboardHID.modifiers |= HID_KEYB_RIGHT_SHIFT;
    break;
  case LEFT_CTRL:
	  keyboardHID.modifiers |= HID_KEYB_LEFT_CTRL;
    break;
  case RIGHT_CTRL:
	  keyboardHID.modifiers |= HID_KEYB_RIGHT_CTRL;
    break;
  case LEFT_ALT:
	  keyboardHID.modifiers |= HID_KEYB_LEFT_ALT;
    break;
  case RIGHT_ALT:
	  keyboardHID.modifiers |= HID_KEYB_RIGHT_ALT;
    break;
  case LEFT_GUI:
	  keyboardHID.modifiers |= HID_KEYB_LEFT_GUI;
    break;
  case RIGHT_GUI:
	  keyboardHID.modifiers |= HID_KEYB_RIGHT_GUI;
    break;
  case CAPS_LOCK:
    //LockKeyToggle((void *)&g_sKeyboardDevice, HID_KEYB_CAPS_LOCK);
    break;
  case BLUETOOTH:
    //ToggleBluetooth();
    break;
  case APP_A:
  case APP_B:
    return;
  default:
	  AddKeyToPressedList(c);
    //KeyStateChange((void *)&g_sKeyboardDevice, modifierFlags, c, true);
    return;
  }
}

void ReleaseKey(uint8_t c)
{

  switch (c)
  {
  case LEFT_SHIFT:
	  keyboardHID.modifiers &= ~HID_KEYB_LEFT_SHIFT;
    break;
  case RIGHT_SHIFT:
	  keyboardHID.modifiers &= ~HID_KEYB_RIGHT_SHIFT;
    break;
  case LEFT_CTRL:
	  keyboardHID.modifiers &= ~HID_KEYB_LEFT_CTRL;
    break;
  case RIGHT_CTRL:
	  keyboardHID.modifiers &= ~HID_KEYB_RIGHT_CTRL;
    break;
  case LEFT_ALT:
	  keyboardHID.modifiers &= ~HID_KEYB_LEFT_ALT;
    break;
  case RIGHT_ALT:
	  keyboardHID.modifiers &= ~HID_KEYB_RIGHT_ALT;
    break;
  case LEFT_GUI:
	  keyboardHID.modifiers &= ~HID_KEYB_LEFT_GUI;
    break;
  case RIGHT_GUI:
	  keyboardHID.modifiers &= ~HID_KEYB_RIGHT_GUI;
    break;
  case APP_A:
  case APP_B:
    return;
  default:
	  RemoveKeyFromPressedList(c);
    return;
  }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

	PixelRGB_t pixel[NUM_ROWS][NUM_PIXELS] = {0};
	  uint32_t dmaBuffer[NUM_ROWS][DMA_BUFF_SIZE] = {0};
	  uint32_t *pBuff;
	  int i, j, k;
	  uint16_t stepSize;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ICACHE_Init();
  MX_LPUART1_UART_Init();
  MX_USB_Device_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  USBD_HandleTypeDef* device = MX_USB_Device_Init();
  uint8_t lastState[5][14];
   uint8_t lastLastState[5][14];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

   HAL_GPIO_WritePin(LOGIC_TRANS_EN_GPIO_Port, LOGIC_TRANS_EN_Pin, GPIO_PIN_SET);


   	   k = 0;
     stepSize = 4;

    while (1){
   	uint8_t wrote = 0;
   	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
   	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);

   	// iterate through all rows and columns
   for (int row = 0; row < 5; row++){

   	       	//HAL_GPIO_WritePin(ROW0_GPIO_Port, ROW0_Pin, GPIO_PIN_SET);

	  HAL_GPIO_WritePin(ROW0_GPIO_Port, ROW0_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_RESET);

		switch (row){
			case 0:
				HAL_GPIO_WritePin(ROW0_GPIO_Port, ROW0_Pin, GPIO_PIN_SET);
				break;
			case 1:
				HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
				break;
			case 2:
				HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
				break;
			case 3:
				HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
				break;
			case 4:
				HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_SET);
				break;
		}

		for (int col = 0; col < 14; col += 2){

			// write to muxes

			HAL_GPIO_WritePin(COLMUXA_GPIO_Port, COLMUXA_Pin, (col >> 1) & 0b001);
			HAL_GPIO_WritePin(COLMUXB_GPIO_Port, COLMUXB_Pin, (col >> 1) & 0b010);
			HAL_GPIO_WritePin(COLMUXC_GPIO_Port, COLMUXC_Pin, (col >> 1) & 0b100);

			uint8_t colA = HAL_GPIO_ReadPin(COLOUTA_GPIO_Port, COLOUTA_Pin);
			uint8_t colB = HAL_GPIO_ReadPin(COLOUTB_GPIO_Port, COLOUTB_Pin);

			// read from muxes
			if (colA && lastState[row][col + 0] == 1 && lastLastState[row][col + 0] == 0){
				PressKey(KeyboardMatrix[row][col + 0]);
				//HAL_UART_Transmit(&hlpuart1, matrix[row][col + 0], sizeof(matrix[row][col + 0]), 100);
				//HAL_UART_Transmit(&hlpuart1, " ", sizeof(" "), 100)
			} else if (!colA && lastState[row][col + 0] == 0 && lastLastState[row][col + 0] == 1){
				ReleaseKey(KeyboardMatrix[row][col + 0]);
			}

			if (colB && lastState[row][col + 1] == 1 && lastLastState[row][col + 1] == 0){
				PressKey(KeyboardMatrix[row][col + 1]);
				//HAL_UART_Transmit(&hlpuart1, matrix[row][col + 1], sizeof(matrix[row][col + 1]), 100);
				//HAL_UART_Transmit(&hlpuart1, " ", sizeof(" "), 100);
			} else if (!colB && lastState[row][col + 1] == 0 && lastLastState[row][col + 1] == 1){
				ReleaseKey(KeyboardMatrix[row][col + 1]);
			}

			lastLastState[row][col + 0] = lastState[row][col + 0];
			lastLastState[row][col + 1] = lastState[row][col + 1];
			lastState[row][col + 0] = colA;
			lastState[row][col + 1] = colB;
		}
	   }

	   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
	   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);

	   USBD_HID_SendReport(device, &keyboardHID, sizeof(struct keyboardHID_t));

	   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);

	   // LEDs

	   k = (k + stepSize) % 765;

	      	 for (int x = 0; x < NUM_ROWS; x++){

	  		   if (k < 255)
	  		   {
	  			 pixel[x][0].color.g = 254 - k; //[254, 0]
	  			 pixel[x][0].color.r =  k + 1;  //[1, 255]
	  			 pixel[x][0].color.b = 0;
	  		   }
	  		   else if (k < 510)
	  		   {
	  			 pixel[x][0].color.g = 0;
	  			 pixel[x][0].color.r = 509 - k; //[254, 0]
	  			 pixel[x][0].color.b = k - 254; //[1, 255]
	  			 j++;
	  		   }
	  		   else if (k < 765)
	  		   {
	  			 pixel[x][0].color.g = k - 509; //[1, 255];
	  			 pixel[x][0].color.r = 0;
	  			 pixel[x][0].color.b = 764 - k; //[254, 0]
	  		   }


	  			//not so bright
	  	       pixel[x][0].color.g >>= 4;
	  	       pixel[x][0].color.r >>= 4;
	  	       pixel[x][0].color.b >>= 4;

	  		   pBuff = dmaBuffer[x];
	  		   for (i = 0; i < NUM_PIXELS; i++)
	  		   {
	  			  for (j = 23; j >= 0; j--)
	  			  {
	  				if ((pixel[x][i].data >> j) & 0x01)
	  				{
	  				  *pBuff = NEOPIXEL_ONE;
	  				}
	  				else
	  				{
	  				  *pBuff = NEOPIXEL_ZERO;
	  				}
	  				pBuff++;
	  			}
	  		   }
	  		   dmaBuffer[x][DMA_BUFF_SIZE - 1] = 0; // last element must be 0!

	  		   switch (x){
	  			   case 0: // ROW0 = PA3 = T2C4
	  				   HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_4, dmaBuffer[x], DMA_BUFF_SIZE);
	  				   break;
	  			   case 1: // ROW1 = PA2 = T2C3
	  				   HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, dmaBuffer[x], DMA_BUFF_SIZE);
	  				   break;
	  			   case 2: // ROW2 = PA1 = T2C2
	  				   HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, dmaBuffer[x], DMA_BUFF_SIZE);
	  				   break;
	  			   case 3: // ROW3 = PB0 = T3C3
	  				   HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_3, dmaBuffer[x], DMA_BUFF_SIZE);
	  				   break;
	  			   case 4: // ROW4 = PA0 = T2C1
	  				   HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, dmaBuffer[x], DMA_BUFF_SIZE);
	  				   break;
	  		   }

	  		   for (i = (NUM_PIXELS - 1); i > 0; i--)
	  		   {
	  			 pixel[x][i].data = pixel[x][i - 1].data;
	  		   }

	      	 }



	   HAL_Delay(10);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the SYSCFG APB clock
  */
  __HAL_RCC_CRS_CLK_ENABLE();

  /** Configures CRS
  */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,1000);
  RCC_CRSInitStruct.ErrorLimitValue = 34;
  RCC_CRSInitStruct.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 39;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 39;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, ROW3_Pin|COLMUXC_Pin|ROW2_Pin|COLMUXA_Pin
                          |COLMUXB_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, ROW1_Pin|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ROW4_Pin|ROW0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LOGIC_TRANS_EN_Pin|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : ROW3_Pin ROW2_Pin */
  GPIO_InitStruct.Pin = ROW3_Pin|ROW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : COLMUXC_Pin COLMUXA_Pin COLMUXB_Pin */
  GPIO_InitStruct.Pin = COLMUXC_Pin|COLMUXA_Pin|COLMUXB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : COLOUTA_Pin */
  GPIO_InitStruct.Pin = COLOUTA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(COLOUTA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ROW1_Pin */
  GPIO_InitStruct.Pin = ROW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ROW1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : COLOUTB_Pin */
  GPIO_InitStruct.Pin = COLOUTB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(COLOUTB_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ROW4_Pin ROW0_Pin */
  GPIO_InitStruct.Pin = ROW4_Pin|ROW0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : LOGIC_TRANS_EN_Pin */
  GPIO_InitStruct.Pin = LOGIC_TRANS_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LOGIC_TRANS_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
