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

#include "IQProcessorWorker.h"

#include "GlobalObjects.h"

/**************************************************************************************************/

IQProcessorWorker::IQProcessorWorker(size_t mtu, size_t total) {
    this->mtu = mtu;
    this->total = total;
}

/**************************************************************************************************/

IQProcessorWorker::~IQProcessorWorker() {
    lrpt_iq_data_free(iqData);
    lrpt_iq_file_close(iqFile);
}

/**************************************************************************************************/

void IQProcessorWorker::process(void) {
    /* TODO check for errors in the whole function */

    /* Allocate I/Q data object for buffered reading */
    iqData = lrpt_iq_data_alloc(mtu, NULL);

    /* Open destination file for writing */
    iqFile = lrpt_iq_file_open_w_v1("/tmp/1.iq", false, 512000, "RTLSDR v3", NULL);

    uint64_t dataWritten = 0;

    while (dataWritten < total) {
        const size_t n = ((total - dataWritten) < mtu) ? total - dataWritten : mtu;

        iqRBUsed->acquire(n);
        lrpt_iq_rb_pop(iqRB, iqData, n, NULL);
        iqRBFree->release(n);

        lrpt_iq_data_write_to_file(iqData, iqFile, false, NULL);

        dataWritten += n;
    }

    emit finished();
}
