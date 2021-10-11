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

#include "DecoderWorker.h"

#include "GlobalObjects.h"

#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

DecoderWorker::DecoderWorker(lrpt_decoder_t *decoder, int MTU, lrpt_qpsk_file_t *processedDump) {
    this->decoder = decoder;
    this->MTU = MTU;
    this->processedDump = processedDump;
}

/**************************************************************************************************/

DecoderWorker::~DecoderWorker() {
    lrpt_qpsk_data_free(qpskInput);
    lrpt_qpsk_data_free(remnants);
}

/**************************************************************************************************/

void DecoderWorker::process() {
    /* TODO check for errors in the whole class */

    /* Allocate QPSK data object for buffered reading */
    qpskInput = lrpt_qpsk_data_alloc(MTU, NULL);

    /* Allocate helper QPSK data object for storing remnants */
    remnants = lrpt_qpsk_data_alloc(0, NULL);

    forever {
        /* Check whether master has requested interruption */
        if (QThread::currentThread()->isInterruptionRequested()) {
            size_t dataLen = qpskRBUsed->available();
            size_t dataRead = 0;

            /* We should leave at least 3xSFL at the end of RB */
            size_t extra = (dataLen % (3 * lrpt_decoder_sfl()));
            dataLen -= extra;

            /* Read till QPSK ring buffer become empty (with account to 3xSFL) */
            while (dataRead < dataLen) {
                size_t n = ((dataLen - dataRead) < static_cast<size_t>(MTU)) ?
                            (dataLen - dataRead) :
                            MTU;

                qpskRBUsed->acquire(n);
                lrpt_qpsk_rb_pop(qpskRB, qpskInput, n, NULL);
                qpskRBFree->release(n);

                processChunk();

                dataRead += n;
            }

            break;
        }

        /* Non-blocking resource request */
        if (qpskRBUsed->tryAcquire(MTU)) {
            lrpt_qpsk_rb_pop(qpskRB, qpskInput, MTU, NULL);
            qpskRBFree->release(MTU);

            processChunk();
        }
    }

    emit finished(); /* Tell caller about job end */
}

/**************************************************************************************************/

void DecoderWorker::processChunk() {
    /* TODO should deinterleave and dediffcode here */
    /* TODO dump processed QPSK data here */

    /* Prepare symbols for drawing on constellation */
    int n = lrpt_qpsk_data_length(qpskInput);

    if (n > QPSKConstPoints)
        n = QPSKConstPoints;

    lrpt_qpsk_data_to_soft(qpskInput, const_pts, n, NULL);

    QVector<int> pts(const_pts, const_pts + n);
    emit qpskConst(pts);

    /* Append data to remnants */
    if (n_rem != 0) {
        lrpt_qpsk_data_append(remnants, qpskInput, 0, lrpt_qpsk_data_length(qpskInput), NULL);
        oper = remnants;
    }
    else
        oper = qpskInput;

    lrpt_decoder_exec(decoder, oper, &n_proc, NULL);

    bool frmStatus = lrpt_decoder_framingstate(decoder);
    int frmTotCnt = lrpt_decoder_framestot_cnt(decoder);
    int frmOkCnt = lrpt_decoder_framesok_cnt(decoder);
    int cvcduCnt = lrpt_decoder_cvcdu_cnt(decoder);
    int pckCnt = lrpt_decoder_packets_cnt(decoder);
    int sigQ = lrpt_decoder_sigqual(decoder);

    emit decoderInfo(frmStatus, frmTotCnt, frmOkCnt, cvcduCnt, pckCnt, sigQ);

    /* Recalculate how much data left unprocessed */
    n_rem = lrpt_qpsk_data_length(oper) - n_proc;

    /* Copy the remnants */
    if (n_rem != 0) {
        if (oper == qpskInput)
            lrpt_qpsk_data_from_qpsk(remnants, qpskInput, n_proc, n_rem, NULL);
        else {
            lrpt_qpsk_data_t *tmp =
                    lrpt_qpsk_data_create_from_qpsk(remnants, n_proc, n_rem, NULL);
            lrpt_qpsk_data_from_qpsk(remnants, tmp, 0, n_rem, NULL);
            lrpt_qpsk_data_free(tmp);
        }
    }

    /* TODO we should emit not only this signal but a number of newly available pixels in each APID */
    emit chunkProcessed();
}
