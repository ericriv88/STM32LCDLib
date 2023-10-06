/*
 * I2C_LCD.h
 *
 *  Created on: Sep 13, 2023
 *      Author: Eric Rivera
 */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

//void I2C_LCD_send_cmd(uint8_t cmd);
//void I2C_LCD_send_data(uint8_t data);
void I2C_LCD_init();
void I2C_LCD_send_string(char* str);
void I2C_LCD_send_char(char letter);
void I2C_LCD_send_int(int num);
void I2C_LCD_clear();
void I2C_LCD_set_cursor(int row, int col);
void I2C_LCD_shift_display_right();
void I2C_LCD_shift_display_left();
void I2C_LCD_scroll_display_left();
#endif /* INC_I2C_LCD_H_ */
