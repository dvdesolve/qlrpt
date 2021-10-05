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

#include "DemodulatorWorker.h"

#include "GlobalObjects.h"

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
}

/**************************************************************************************************/

void DemodulatorWorker::process() {
    /* TODO check for errors in the whole function */

    /* Allocate I/Q data object for buffered reading */
    iqInput = lrpt_iq_data_alloc(MTU, NULL);

    /* Allocate QPSK data object for temporary storage */
    qpskOutput = lrpt_qpsk_data_alloc(0, NULL);

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

                /* TODO may be move into separate private function */
                if (filter) {
                    lrpt_dsp_filter_apply(filter, iqInput);

                    if (filtDump)
                        lrpt_iq_data_write_to_file(iqInput, filtDump, false, NULL);
                }

                lrpt_demodulator_exec(demod, iqInput, qpskOutput, NULL);

                if (demodDump)
                    lrpt_qpsk_data_write_to_file(qpskOutput, demodDump, false, NULL);

                bool pllStatus = lrpt_demodulator_pllstate(demod);
                double pllFreq = lrpt_demodulator_pllfreq(demod);
                double pllPhaseErr = lrpt_demodulator_pllphaseerr(demod);
                double alcGain = lrpt_demodulator_gain(demod);
                double sigLvl = lrpt_demodulator_siglvl(demod);

                emit demodInfo(pllStatus, pllFreq, pllPhaseErr, alcGain, sigLvl);

                size_t m = lrpt_qpsk_data_length(qpskOutput);
                qpskRBFree->acquire(m);
                lrpt_qpsk_rb_push(qpskRB, qpskOutput, m, NULL);
                qpskRBUsed->release(m);

                dataRead += n;

                emit chunkProcessed();
            }

            break;
        }

        /* Non-blocking resource request */
        if (iqRBUsed->tryAcquire(MTU)) {
            lrpt_iq_rb_pop(iqRB, iqInput, MTU, NULL);
            iqRBFree->release(MTU);

            /* TODO may be move into separate private function */
            if (filter) {
                lrpt_dsp_filter_apply(filter, iqInput);

                if (filtDump)
                    lrpt_iq_data_write_to_file(iqInput, filtDump, false, NULL);
            }

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
    }

    emit finished(); /* Tell caller about job end */
}
