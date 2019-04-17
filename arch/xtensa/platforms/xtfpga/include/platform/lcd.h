/*
 * arch/xtensa/platform/xtavnet/include/platform/lcd.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001, 2006 Tensilica Inc.
 */

#ifndef __XTENSA_XTAVNET_LCD_H
#define __XTENSA_XTAVNET_LCD_H

<<<<<<< HEAD
#ifdef CONFIG_XTFPGA_LCD
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
/* Display string STR at position POS on the LCD. */
void lcd_disp_at_pos(char *str, unsigned char pos);

/* Shift the contents of the LCD display left or right. */
void lcd_shiftleft(void);
void lcd_shiftright(void);
<<<<<<< HEAD
#else
static inline void lcd_disp_at_pos(char *str, unsigned char pos)
{
}

static inline void lcd_shiftleft(void)
{
}

static inline void lcd_shiftright(void)
{
}
#endif

=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#endif
