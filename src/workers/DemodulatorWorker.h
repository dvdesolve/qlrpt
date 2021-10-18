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

#ifndef DEMODULATORWORKER_H
#define DEMODULATORWORKER_H

/**************************************************************************************************/

#include "QPSKSourceAbstractWorker.h"

#include <lrpt.h>

/**************************************************************************************************/

class DemodulatorWorker : public QPSKSourceAbstractWorker {
    Q_OBJECT

public:
    explicit DemodulatorWorker(
            lrpt_demodulator_t *demod,
            int MTU,
            lrpt_dsp_filter_t *filter = NULL,
            lrpt_iq_file_t *filtDump = NULL,
            lrpt_qpsk_file_t *demodDump = NULL);
    ~DemodulatorWorker();

public slots:
    void process() override;

signals:
    void demodInfo(bool, double, double, double, double);
    void iqWaterfall(QVector<int>);

private:
    lrpt_demodulator_t *demod = NULL;
    int MTU;
    lrpt_dsp_filter_t *filter = NULL;
    lrpt_iq_file_t *filtDump = NULL;
    lrpt_qpsk_file_t *demodDump = NULL;

    lrpt_iq_data_t *iqInput = NULL;
    lrpt_qpsk_data_t *qpskOutput = NULL;

    lrpt_dsp_ifft_t *ifft = NULL;
    int16_t ifftData[512];
    double fftIQ[1024];

    void processChunk();
};

/**************************************************************************************************/

#endif
