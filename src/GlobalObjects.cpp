/*
 * This file is part of qlrpt.
 *
 * qlrpt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qlrpt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qlrpt. If not, see https://www.gnu.org/licenses/
 *
 * Author: Viktor Drobot
 */

/**************************************************************************************************/

#include "GlobalObjects.h"

#include <QSemaphore>

#include <lrpt.h>

/**************************************************************************************************/

/* I/Q data ring buffer */
lrpt_iq_rb_t *iqRB = NULL;

/* Guarding semaphores for I/Q ring buffer */
QSemaphore *iqRBUsed = NULL;
QSemaphore *iqRBFree = NULL;

/* QPSK data ring buffer */
lrpt_qpsk_rb_t *qpskRB = NULL;

/* Guarding semaphores for QPSK ring buffer */
QSemaphore *qpskRBUsed = NULL;
QSemaphore *qpskRBFree = NULL;
