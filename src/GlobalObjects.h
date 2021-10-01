/*
 * This file is part of glrpt.
 *
 * glrpt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * glrpt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with glrpt. If not, see https://www.gnu.org/licenses/
 *
 * Author: Viktor Drobot
 */

/**************************************************************************************************/

#ifndef GLOBALOBJECTS_H
#define GLOBALOBJECTS_H

/**************************************************************************************************/

#include <QSemaphore>

#include <lrpt.h>

/**************************************************************************************************/

extern lrpt_iq_rb_t *iqRB;

extern QSemaphore *iqRBUsed;
extern QSemaphore *iqRBFree;

extern lrpt_qpsk_rb_t *qpskRB;

extern QSemaphore *qpskRBUsed;
extern QSemaphore *qpskRBFree;

/**************************************************************************************************/

#endif
