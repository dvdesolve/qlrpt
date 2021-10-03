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

#include "DecoderWorker.h"

#include "GlobalObjects.h"

#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

DecoderWorker::DecoderWorker(lrpt_decoder_t *decoder, int MTU) {
    this->decoder = decoder;
    this->MTU = MTU;
}

/**************************************************************************************************/

DecoderWorker::~DecoderWorker() {
    lrpt_qpsk_data_free(qpskInput);
    lrpt_decoder_deinit(decoder);
}

/**************************************************************************************************/

void DecoderWorker::process() {
    /* TODO check for errors in the whole function */

    /* Allocate I/Q data object for buffered reading */
    qpskInput = lrpt_qpsk_data_alloc(MTU, NULL);

    forever {
        /* Check whether master has requested interruption */
        if (QThread::currentThread()->isInterruptionRequested()) {
            size_t dataLen = qpskRBUsed->available();
            size_t dataRead = 0;

            /* Read till QPSK ring buffer become empty */
            while (dataRead < dataLen) {
                size_t n = ((dataLen - dataRead) < static_cast<size_t>(MTU)) ?
                            (dataLen - dataRead) :
                            MTU;

                qpskRBUsed->acquire(n);
                lrpt_qpsk_rb_pop(qpskRB, qpskInput, n, NULL);
                qpskRBFree->release(n);

                /* TODO decode acquired data */
                /* TODO dump data to be sure that all will be saved */
                QThread::currentThread()->msleep(8); /* TODO debug */

                dataRead += n;

                emit chunkProcessed();
            }

            break;
        }

        /* Non-blocking resource request */
        if (qpskRBUsed->tryAcquire(MTU)) {
            lrpt_qpsk_rb_pop(qpskRB, qpskInput, MTU, NULL);
            qpskRBFree->release(MTU);

            /* TODO decode acquired data */
            /* TODO dump data to be sure that all will be saved */
            QThread::currentThread()->msleep(8); /* TODO debug */

            emit chunkProcessed();
        }
    }

    emit finished(); /* Tell caller about job end */
}
