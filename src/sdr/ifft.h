/*
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details:
 *
 *  http://www.gnu.org/copyleft/gpl.txt
 */

#ifndef IFFT_H
#define IFFT_H

#include <glib.h>

#include <stdint.h>

#define IFFT_DECIMATE  2

void Deinit_Ifft(void);
void IFFT(int16_t *data);
gboolean Initialize_IFFT(int16_t width);

#endif
