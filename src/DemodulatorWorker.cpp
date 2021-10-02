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

#include "DemodulatorWorker.h"

#include "GlobalObjects.h"

#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

DemodulatorWorker::DemodulatorWorker(lrpt_demodulator_t *demod, int MTU) {
    this->demod = demod;
    this->MTU = MTU;
}

/**************************************************************************************************/

DemodulatorWorker::~DemodulatorWorker() {
    lrpt_iq_data_free(iqInput);
    lrpt_qpsk_data_free(qpskOutput);
}

/**************************************************************************************************/

void DemodulatorWorker::process() {
    /* TODO check for errors in the whole function */

    /* Allocate I/Q data object for buffered reading */
    iqInput = lrpt_iq_data_alloc(MTU, NULL);

    forever {
        /* Check whether master has requested interruption */
        if (QThread::currentThread()->isInterruptionRequested()) {
            size_t dataLen = iqRBUsed->available();
            size_t dataRead = 0;

            /* Read till I/Q ring buffer become empty */
            while (dataRead < dataLen) {
                size_t n = ((dataLen - dataRead) < static_cast<size_t>(MTU)) ?
                            (dataLen - dataRead) :
                            MTU;

                iqRBUsed->acquire(n);
                lrpt_iq_rb_pop(iqRB, iqInput, n, NULL);
                iqRBFree->release(n);

                /* TODO demod acquired data */
                /* TODO dump data to be sure that all will be saved */
                QThread::currentThread()->msleep(8); /* TODO debug */

                dataRead += n;

                emit chunkProcessed();
            }

            break;
        }

        /* Non-blocking resource request */
        if (iqRBUsed->tryAcquire(MTU)) {
            lrpt_iq_rb_pop(iqRB, iqInput, MTU, NULL);
            iqRBFree->release(MTU);

            /* TODO demod acquired data */
            /* TODO dump data to be sure that all will be saved */
            QThread::currentThread()->msleep(8); /* TODO debug */

            emit chunkProcessed();
        }
    }

    emit finished(); /* Tell caller about job end */
}
