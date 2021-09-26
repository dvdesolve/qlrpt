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

#include "IQProducerFile.h"

#include "GlobalObjects.h"

#include <lrpt.h>

/**************************************************************************************************/

IQProducerFile::IQProducerFile(lrpt_iq_file_t *iqFile) {
    this->iqFile = iqFile;
}

/**************************************************************************************************/

void IQProducerFile::run() {
    /* TODO use stored settings instead */
    int mtu = 131072;

    /* TODO error reporting */
    /* TODO check for NULL iqData */
    lrpt_iq_data_t *iqData = lrpt_iq_data_alloc(mtu, NULL);

    const uint64_t dataLen = lrpt_iq_file_length(iqFile);
    uint64_t totDataRead = 0;
    size_t iqDataLen = 0;

    while (totDataRead < dataLen) {
        lrpt_iq_data_read_from_file(iqData, iqFile, mtu, false, NULL);
        iqDataLen = lrpt_iq_data_length(iqData);
        totDataRead += iqDataLen;

        iqRBFree->acquire(iqDataLen);
        lrpt_iq_rb_push(iqRB, iqData, iqDataLen, NULL);
        iqRBUsed->release(iqDataLen);
    }
}
