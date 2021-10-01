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

#ifndef QPSKSOURCEFILEWORKER_H
#define QPSKSOURCEFILEWORKER_H

/**************************************************************************************************/

#include "QPSKSourceAbstractWorker.h"

#include <lrpt.h>

/**************************************************************************************************/

class QPSKSourceFileWorker : public QPSKSourceAbstractWorker {
    Q_OBJECT

public:
    explicit QPSKSourceFileWorker(lrpt_qpsk_file_t *qpskFile, int MTU);
    ~QPSKSourceFileWorker();

public slots:
    void process() override;

signals:
    void finished();

private:
    lrpt_qpsk_file_t *qpskFile = NULL;
    int MTU;

    lrpt_qpsk_data_t *qpskData = NULL;
};

/**************************************************************************************************/

#endif
