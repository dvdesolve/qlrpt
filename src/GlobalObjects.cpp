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

#include <QCoreApplication>
#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
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

/**************************************************************************************************/

int initGlobalObjects(void) {
    QRect scrRect = QGuiApplication::screens().at(0)->virtualGeometry();

    int iqRBSize = 131072 * 10; /* TODO use stored settings instead */

    iqRB = lrpt_iq_rb_alloc(iqRBSize, NULL);

    if (!iqRB) {
        QMessageBox mbox(
                    QMessageBox::Critical,
                    QCoreApplication::translate("Initialization Routine", "glrpt error"),
                    QCoreApplication::translate("Initialization Routine", "Can't allocate I/Q data ring buffer object!"),
                    QMessageBox::Close);

        QSize mSize = mbox.sizeHint();

        mbox.move(QPoint(
                      scrRect.width() / 2 - mSize.width()/2,
                      scrRect.height()/2 - mSize.height() / 2));
        mbox.exec();

        return 1;
    }

    iqRBUsed = new QSemaphore(0);
    iqRBFree = new QSemaphore(iqRBSize);

    qpskRB = lrpt_qpsk_rb_alloc(16384 * 10, NULL); /* TODO use SFL and stored settings instead */

    if (!qpskRB) {
        QMessageBox mbox(
                    QMessageBox::Critical,
                    QCoreApplication::translate("Initialization Routine", "glrpt error"),
                    QCoreApplication::translate("Initialization Routine", "Can't allocate QPSK data ring buffer object!"),
                    QMessageBox::Close);

        QSize mSize = mbox.sizeHint();

        mbox.move(QPoint(
                      scrRect.width() / 2 - mSize.width()/2,
                      scrRect.height()/2 - mSize.height() / 2));
        mbox.exec();

        return 1;
    }

    return 0;
}

/**************************************************************************************************/

void deinitGlobalObjects(void) {
    lrpt_iq_rb_free(iqRB);
    delete iqRBUsed;
    delete iqRBFree;

    lrpt_qpsk_rb_free(qpskRB);
}
