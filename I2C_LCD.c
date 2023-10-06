/*
 * I2C_LCD.c
 *
 *  Created on: Sep 13, 2023
 *      Author: Eric Rivera
 *
 *      LCD(2x16) connected to PCF8574 to enable I2C communication in following byte sequence:
 *
 *      [D7 D6 D5 D4 X E RW RS]		X - not connected to an actual LCD pin but needs to be enabled (1)
 *      							E - enable pin
 *      							RW - read write pin (1 for read, 0 for write)
 *      							RS - register select (0 for command, 1 for reading/writing to/from CG or DDRAM)
 *      							DX - data pins for 4 bit operation mode
 */

#include <stdint.h>
#include "I2C_LCD.h"
#include "stm32l4xx_hal.h"		//need to change this according to your board/MCU

#define LCD_ADDRESS 0x4E

extern I2C_HandleTypeDef hi2c1;	//may need to change this depending on which I2C is in use

void I2C_LCD_send_cmd(uint8_t cmd) {
	uint8_t send_data[4]; 	//sending 4 bytes of data
							//(upper and lower nibbles of command with enable high then low)

	uint8_t data_high = cmd & 0xF0;	//bit mask to only get upper nibble
	uint8_t data_low = (cmd << 4);	//shift lower nibble to upper 4 bits

	send_data[0] = data_high | 0x0C;	//send upper command nibble with enable high then low
	send_data[1] = data_high | 0x08;
	send_data[2] = data_low  | 0x0C;	//send lower command nibble with enable high then low
	send_data[3] = data_low  | 0x08;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDRESS, (uint8_t*) send_data, 4, 100);	//send command over I2C
}

void I2C_LCD_send_data(uint8_t data) {
	uint8_t send_data[4]; 	//sending 4 bytes of data
							//(upper and lower nibbles of data with enable high then low)

	uint8_t data_high = data & 0xF0; //bit mask to only get upper nibble
	uint8_t data_low = (data << 4);	 //shift lower nibble to upper 4 bits

	send_data[0] = data_high | 0x0D;	//send upper data nibble with RS high and enable high then low
	send_data[1] = data_high | 0x09;
	send_data[2] = data_low  | 0x0D;	//send lower data nibble with RS high and enable high then low
	send_data[3] = data_low  | 0x09;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDRESS, (uint8_t*) send_data, 4, 100);	//send data over I2C
}

void I2C_LCD_init() {	//Initialize LCD according to the 8 4-bit initialization commands found in data sheet
	HAL_Delay(50);
	I2C_LCD_send_cmd(0x30);
	HAL_Delay(5);
	I2C_LCD_send_cmd(0x30);
	HAL_Delay(1);
	I2C_LCD_send_cmd(0x30);
	HAL_Delay(10);
	I2C_LCD_send_cmd(0x20);	//return home
	HAL_Delay(1);
	I2C_LCD_send_cmd(0x28);	//function set, set 4 bit, 2 display lines
	HAL_Delay(1);
	I2C_LCD_send_cmd(0x0C);	//display DDRAM, no cursor, no cursor blink
	HAL_Delay(1);
	I2C_LCD_send_cmd(0x01);	//clear display
	HAL_Delay(1);
	I2C_LCD_send_cmd(0x06);	//entry mode set, increment on write
	HAL_Delay(100);
}

void I2C_LCD_send_string(char* str) {
	while(*str) {
		I2C_LCD_send_data(*str++);
		HAL_Delay(1); //account for busy signal
	}
}

void I2C_LCD_send_char(char letter) {
	I2C_LCD_send_data(letter);
}

void I2C_LCD_send_int(int num) {
	int digits = 0;
	int temp = num;
	do{							//find put how many digits num has
		digits++;
		temp /= 10;
	} while(temp != 0);

	char str[digits];			//initialize char string to send

	for(int i = (digits - 1); i >= 0; i--) {	//change num to string of chars
		str[i] = (num % 10) + '0';
		num /= 10;
	}

	for(int i = 0; i < digits; i++) {	//send chars one by one
		I2C_LCD_send_char(str[i]);
	}
	HAL_Delay(1);	//account for busy signal
}


void I2C_LCD_clear() {
	I2C_LCD_send_cmd(0x01);
	HAL_Delay(1); //account for busy signal
}

void I2C_LCD_set_cursor(int row, int col) {	//set DDRAM address to defined row (0-1) and column (0-15)
	switch(row){
	case 0:
		col |= 0x80;	//command to set DDRAM address is 0x80 where the lower bits are the address
		break;
	case 1:
		col |= 0xC0;	//row 2 starts at address 0x40
		break;
	}
	I2C_LCD_send_cmd(col);
	HAL_Delay(1);	//account for busy signal
}

void I2C_LCD_shift_display_right() {
	I2C_LCD_send_cmd(0x1C);	//shift display right
	HAL_Delay(1);	//account for busy signal
}

void I2C_LCD_shift_display_left() {
	I2C_LCD_send_cmd(0x18);	//shift display right
	HAL_Delay(1);	//account for busy signal
}

void I2C_LCD_scroll_display_left() {
	for(int i = 0; i < 40; i++) {
		I2C_LCD_shift_display_left();
		HAL_Delay(750);
	}
}



