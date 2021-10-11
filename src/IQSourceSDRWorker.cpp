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

#include "IQSourceSDRWorker.h"

#include "GlobalObjects.h"

#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

IQSourceSDRWorker::IQSourceSDRWorker(/* TODO implement */int MTU) {
    this->MTU = MTU;
}

/**************************************************************************************************/

IQSourceSDRWorker::~IQSourceSDRWorker() {
    lrpt_iq_data_free(iqData);
}

/**************************************************************************************************/

void IQSourceSDRWorker::process() {
    /* TODO check for errors in the whole function */

    /* Allocate I/Q data object for buffered reading */
    iqData = lrpt_iq_data_alloc(MTU, NULL);

    forever {
        /* Check whether master has requested interruption */
        if (QThread::currentThread()->isInterruptionRequested()) {
            /* TODO implement immediate exiting */
        }

        /* TODO implement reading from SDR source */

        size_t n = lrpt_iq_data_length(iqData);

        iqRBFree->tryAcquire(n);
        lrpt_iq_rb_push(iqRB, iqData, n, NULL);
        iqRBUsed->release(n);

        emit chunkProcessed(); /* Tell caller about chunk being read */
    }

    emit finished(); /* Tell caller about job end */
}
