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

#include "QPSKSourceFileWorker.h"

#include "GlobalObjects.h"

#include <lrpt.h>

/**************************************************************************************************/

QPSKSourceFileWorker::QPSKSourceFileWorker(lrpt_qpsk_file_t *qpskFile, int MTU) {
    this->qpskFile = qpskFile;
    this->MTU = MTU;
}

/**************************************************************************************************/

QPSKSourceFileWorker::~QPSKSourceFileWorker() {
    lrpt_qpsk_data_free(qpskData);
}

/**************************************************************************************************/

void QPSKSourceFileWorker::process() {
    /* TODO check for errors in the whole function */

    /* Allocate QPSK data object for buffered reading */
    qpskData = lrpt_qpsk_data_alloc(MTU, NULL);

    uint64_t dataLen = lrpt_qpsk_file_length(qpskFile);
    uint64_t dataRead = 0;

    while (dataRead < dataLen) {
        lrpt_qpsk_data_read_from_file(qpskData, qpskFile, MTU, false, NULL);
        size_t n = lrpt_qpsk_data_length(qpskData);

        qpskRBFree->acquire(n);
        lrpt_qpsk_rb_push(qpskRB, qpskData, n, NULL);
        qpskRBUsed->release(n);

        dataRead += n;

        emit chunkProcessed(); /* Tell caller about chunk being read */
    }

    emit finished(); /* Tell caller about job end */
}
