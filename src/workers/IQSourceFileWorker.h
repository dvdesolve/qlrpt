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

#ifndef IQSOURCEFILEWORKER_H
#define IQSOURCEFILEWORKER_H

/**************************************************************************************************/

#include "IQSourceAbstractWorker.h"

#include <lrpt.h>

/**************************************************************************************************/

class IQSourceFileWorker : public IQSourceAbstractWorker {
    Q_OBJECT

public:
    explicit IQSourceFileWorker(lrpt_iq_file_t *iqFile, int MTU);
    ~IQSourceFileWorker();

public slots:
    void process() override;

private:
    lrpt_iq_file_t *iqFile = NULL;
    int MTU;

    lrpt_iq_data_t *iqData = NULL;

signals:
    void readProgress(int);
};

/**************************************************************************************************/

#endif
