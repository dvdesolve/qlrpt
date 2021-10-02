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

#ifndef DEMODULATORWORKER_H
#define DEMODULATORWORKER_H

/**************************************************************************************************/

#include "QPSKSourceAbstractWorker.h"

#include <lrpt.h>

/**************************************************************************************************/

class DemodulatorWorker : public QPSKSourceAbstractWorker {
    Q_OBJECT

public:
    explicit DemodulatorWorker(lrpt_demodulator_t *demod, int MTU);
    ~DemodulatorWorker();

public slots:
    void process() override;

signals:
    void finished();

private:
    lrpt_demodulator_t *demod = NULL;
    int MTU;

    lrpt_iq_data_t *iqInput = NULL;
    lrpt_qpsk_data_t *qpskOutput = NULL;
};

/**************************************************************************************************/

#endif
