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

#include "DemodulatorWorker.h"

#include "../qlrpt/GlobalObjects.h"

#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

DemodulatorWorker::DemodulatorWorker(
        lrpt_demodulator_t *demod,
        int MTU,
        lrpt_dsp_filter_t *filter,
        lrpt_iq_file_t *filtDump,
        lrpt_qpsk_file_t *demodDump) {
    this->demod = demod;
    this->MTU = MTU;
    this->filter = filter;
    this->filtDump = filtDump;
    this->demodDump = demodDump;
}

/**************************************************************************************************/

DemodulatorWorker::~DemodulatorWorker() {
    lrpt_iq_data_free(iqInput);
    lrpt_qpsk_data_free(qpskOutput);

    lrpt_dsp_ifft_deinit(ifft);
}

/**************************************************************************************************/

void DemodulatorWorker::process() {
    /* TODO check for errors in the whole class */

    /* Allocate I/Q data object for buffered reading */
    iqInput = lrpt_iq_data_alloc(MTU, NULL);

    /* Allocate QPSK data object for temporary storage */
    qpskOutput = lrpt_qpsk_data_alloc(0, NULL);

    /* Initialize IFFT object */
    ifft = lrpt_dsp_ifft_init(256, NULL);

    forever {
        /* Check whether master has requested interruption */
        if (QThread::currentThread()->isInterruptionRequested()) {
            size_t dataLen = iqRBUsed->available();
            size_t dataRead = 0;

            /* Read till I/Q ring buffer become empty */
            while (dataRead < dataLen) {
                size_t n = ((dataLen - dataRead) < static_cast<size_t>(MTU)) ?
                            (dataLen - dataRead) :
                            MTU;

                iqRBUsed->acquire(n);
                lrpt_iq_rb_pop(iqRB, iqInput, n, NULL);
                iqRBFree->release(n);

                processChunk();

                dataRead += n;
            }

            break;
        }

        /* Non-blocking resource request */
        if (iqRBUsed->tryAcquire(MTU)) {
            lrpt_iq_rb_pop(iqRB, iqInput, MTU, NULL);
            iqRBFree->release(MTU);

            processChunk();
        }
    }

    emit finished(); /* Tell caller about job end */
}

/**************************************************************************************************/

void DemodulatorWorker::processChunk() {
    if (filter) {
        lrpt_dsp_filter_apply(filter, iqInput);

        if (filtDump)
            lrpt_iq_data_write_to_file(iqInput, filtDump, false, NULL);
    }

    size_t iqN = lrpt_iq_data_length(iqInput);
    lrpt_iq_data_to_doubles(iqInput, fftIQ, 512, NULL);

    int dec = 0;
    double si = 0.0, sq = 0.0;

    for (int i = 0; i < 512; i++) {
        /* If we still have I/Q data in fftI and fftQ arrays */
        if (i < static_cast<int>(iqN)) {
            si += fftIQ[2 * i];
            sq += fftIQ[2 * i + 1];
        }

        /* Promote decimation (up to the factor of 2) */
        dec++;

        if (dec == 2) {
            /* TODO should we divide here? */
            ifftData[i - 1] = static_cast<int16_t>(si);
            ifftData[i] = static_cast<int16_t>(sq);

            si = sq = 0.0;
            dec = 0;
        }
    }

    lrpt_dsp_ifft_exec(ifft, ifftData);

    QVector<int> coeffs(ifftData, ifftData + 512);
    emit iqWaterfall(coeffs);

    lrpt_demodulator_exec(demod, iqInput, qpskOutput, NULL);

    if (demodDump)
        lrpt_qpsk_data_write_to_file(qpskOutput, demodDump, false, NULL);

    bool pllStatus = lrpt_demodulator_pllstate(demod);
    double pllFreq = lrpt_demodulator_pllfreq(demod);
    double pllPhaseErr = lrpt_demodulator_pllphaseerr(demod);
    double alcGain = lrpt_demodulator_gain(demod);
    double sigLvl = lrpt_demodulator_siglvl(demod);

    emit demodInfo(pllStatus, pllFreq, pllPhaseErr, alcGain, sigLvl);

    size_t n = lrpt_qpsk_data_length(qpskOutput);
    qpskRBFree->acquire(n);
    lrpt_qpsk_rb_push(qpskRB, qpskOutput, n, NULL);
    qpskRBUsed->release(n);

    emit chunkProcessed();
}
