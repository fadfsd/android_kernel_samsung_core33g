/*
<<<<<<< HEAD
 * Driver for the LCD display on the Tensilica XTFPGA board family.
 * http://www.mytechcorp.com/cfdata/productFile/File1/MOC-16216B-B-A0A04.pdf
=======
 * Driver for the LCD display on the Tensilica LX60 Board.
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001, 2006 Tensilica Inc.
<<<<<<< HEAD
 * Copyright (C) 2015 Cadence Design Systems Inc.
 */

#include <linux/delay.h>
=======
 */

/*
 *
 * FIXME: this code is from the examples from the LX60 user guide.
 *
 * The lcd_pause function does busy waiting, which is probably not
 * great. Maybe the code could be changed to use kernel timers, or
 * change the hardware to not need to wait.
 */

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#include <linux/init.h>
#include <linux/io.h>

#include <platform/hardware.h>
#include <platform/lcd.h>
<<<<<<< HEAD

/* LCD instruction and data addresses. */
#define LCD_INSTR_ADDR		((char *)IOADDR(CONFIG_XTFPGA_LCD_BASE_ADDR))
#define LCD_DATA_ADDR		(LCD_INSTR_ADDR + 4)

=======
#include <linux/delay.h>

#define LCD_PAUSE_ITERATIONS	4000
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#define LCD_CLEAR		0x1
#define LCD_DISPLAY_ON		0xc

/* 8bit and 2 lines display */
#define LCD_DISPLAY_MODE8BIT	0x38
<<<<<<< HEAD
#define LCD_DISPLAY_MODE4BIT	0x28
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#define LCD_DISPLAY_POS		0x80
#define LCD_SHIFT_LEFT		0x18
#define LCD_SHIFT_RIGHT		0x1c

<<<<<<< HEAD
static void lcd_put_byte(u8 *addr, u8 data)
{
#ifdef CONFIG_XTFPGA_LCD_8BIT_ACCESS
	ACCESS_ONCE(*addr) = data;
#else
	ACCESS_ONCE(*addr) = data & 0xf0;
	ACCESS_ONCE(*addr) = (data << 4) & 0xf0;
#endif
}

static int __init lcd_init(void)
{
	ACCESS_ONCE(*LCD_INSTR_ADDR) = LCD_DISPLAY_MODE8BIT;
	mdelay(5);
	ACCESS_ONCE(*LCD_INSTR_ADDR) = LCD_DISPLAY_MODE8BIT;
	udelay(200);
	ACCESS_ONCE(*LCD_INSTR_ADDR) = LCD_DISPLAY_MODE8BIT;
	udelay(50);
#ifndef CONFIG_XTFPGA_LCD_8BIT_ACCESS
	ACCESS_ONCE(*LCD_INSTR_ADDR) = LCD_DISPLAY_MODE4BIT;
	udelay(50);
	lcd_put_byte(LCD_INSTR_ADDR, LCD_DISPLAY_MODE4BIT);
	udelay(50);
#endif
	lcd_put_byte(LCD_INSTR_ADDR, LCD_DISPLAY_ON);
	udelay(50);
	lcd_put_byte(LCD_INSTR_ADDR, LCD_CLEAR);
=======
static int __init lcd_init(void)
{
	*LCD_INSTR_ADDR = LCD_DISPLAY_MODE8BIT;
	mdelay(5);
	*LCD_INSTR_ADDR = LCD_DISPLAY_MODE8BIT;
	udelay(200);
	*LCD_INSTR_ADDR = LCD_DISPLAY_MODE8BIT;
	udelay(50);
	*LCD_INSTR_ADDR = LCD_DISPLAY_ON;
	udelay(50);
	*LCD_INSTR_ADDR = LCD_CLEAR;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	mdelay(10);
	lcd_disp_at_pos("XTENSA LINUX", 0);
	return 0;
}

void lcd_disp_at_pos(char *str, unsigned char pos)
{
<<<<<<< HEAD
	lcd_put_byte(LCD_INSTR_ADDR, LCD_DISPLAY_POS | pos);
	udelay(100);
	while (*str != 0) {
		lcd_put_byte(LCD_DATA_ADDR, *str);
=======
	*LCD_INSTR_ADDR = LCD_DISPLAY_POS | pos;
	udelay(100);
	while (*str != 0) {
		*LCD_DATA_ADDR = *str;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
		udelay(200);
		str++;
	}
}

void lcd_shiftleft(void)
{
<<<<<<< HEAD
	lcd_put_byte(LCD_INSTR_ADDR, LCD_SHIFT_LEFT);
=======
	*LCD_INSTR_ADDR = LCD_SHIFT_LEFT;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	udelay(50);
}

void lcd_shiftright(void)
{
<<<<<<< HEAD
	lcd_put_byte(LCD_INSTR_ADDR, LCD_SHIFT_RIGHT);
=======
	*LCD_INSTR_ADDR = LCD_SHIFT_RIGHT;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
	udelay(50);
}

arch_initcall(lcd_init);
