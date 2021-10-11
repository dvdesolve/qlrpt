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

#include "IQSourceFileWorker.h"

#include "GlobalObjects.h"

#include <lrpt.h>

/**************************************************************************************************/

IQSourceFileWorker::IQSourceFileWorker(lrpt_iq_file_t *iqFile, int MTU) {
    this->iqFile = iqFile;
    this->MTU = MTU;
}

/**************************************************************************************************/

IQSourceFileWorker::~IQSourceFileWorker() {
    lrpt_iq_data_free(iqData);
}

/**************************************************************************************************/

void IQSourceFileWorker::process() {
    /* TODO check for errors in the whole function */

    /* Allocate I/Q data object for buffered reading */
    iqData = lrpt_iq_data_alloc(MTU, NULL);

    uint64_t dataLen = lrpt_iq_file_length(iqFile);
    uint64_t dataRead = 0;

    while (dataRead < dataLen) {
        lrpt_iq_data_read_from_file(iqData, iqFile, MTU, false, NULL);
        size_t n = lrpt_iq_data_length(iqData);

        iqRBFree->acquire(n);
        lrpt_iq_rb_push(iqRB, iqData, n, NULL);
        iqRBUsed->release(n);

        dataRead += n;

        emit chunkProcessed(); /* Tell caller about chunk being read */
    }

    emit finished(); /* Tell caller about job end */
}
