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

#include "GlobalObjects.h"

#include "IQSourceAbstractWorker.h"

#include <QSemaphore>
#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

/* I/Q data ring buffer */
lrpt_iq_rb_t *iqRB = NULL;

/* Guarding semaphores for I/Q ring buffer */
QSemaphore *iqRBUsed = NULL;
QSemaphore *iqRBFree = NULL;

/* Thread for I/Q source (SDR or file) */
QThread *iqSrcThread = NULL;

/* Worker object for I/Q source */
IQSourceAbstractWorker *iqSrcWorker = NULL;

/* QPSK data ring buffer */
lrpt_qpsk_rb_t *qpskRB = NULL;

/* Guarding semaphores for QPSK ring buffer */
QSemaphore *qpskRBUsed = NULL;
QSemaphore *qpskRBFree = NULL;

/* Thread for QPSK source (demodulator or file) */
QThread *qpskSrcThread = NULL;

/* Thread for decoder */
QThread *decoderThread = NULL;
