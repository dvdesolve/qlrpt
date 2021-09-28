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

#include "IQSourceFileWorker.h"

#include "GlobalObjects.h"

/**************************************************************************************************/

IQSourceFileWorker::IQSourceFileWorker(size_t mtu, const QString &srcFile) {
    this->mtu = mtu;
    this->srcFile = srcFile;
}

/**************************************************************************************************/

IQSourceFileWorker::~IQSourceFileWorker() {
    lrpt_iq_data_free(iqData);
    lrpt_iq_file_close(iqFile);
}

/**************************************************************************************************/

void IQSourceFileWorker::process(void) {
    /* TODO check for errors in the whole function */

    /* Convert QString to C string (with UTF-8 support) */
    QByteArray fileNameUTF8 = srcFile.toUtf8();
    const char *fileNameCString = fileNameUTF8.data();

    /* Allocate I/Q data object for buffered reading */
    iqData = lrpt_iq_data_alloc(mtu, NULL);

    /* Open source file for reading */
    iqFile = lrpt_iq_file_open_r(fileNameCString, NULL);

    uint64_t tDataLen = lrpt_iq_file_length(iqFile);
    uint64_t dataRead = 0;

    while (dataRead < tDataLen) {
        lrpt_iq_data_read_from_file(iqData, iqFile, mtu, false, NULL);

        const size_t n = lrpt_iq_data_length(iqData);

        iqRBFree->acquire(n);
        lrpt_iq_rb_push(iqRB, iqData, n, NULL);
        iqRBUsed->release(n);

        dataRead += n;
    }

    emit finished();
}
