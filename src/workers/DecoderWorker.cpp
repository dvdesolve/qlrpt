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

#include "../qlrpt/GlobalObjects.h"

#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

DecoderWorker::DecoderWorker(
        lrpt_decoder_t *decoder,
        int MTU,
        lrpt_dsp_dediffcoder_t *dediffcoder,
        lrpt_qpsk_file_t *processedDump) {
    this->decoder = decoder;
    this->MTU = MTU;
    this->dediffcoder = dediffcoder;
    this->processedDump = processedDump;
    imgStdWidth = lrpt_decoder_imgwidth(decoder);
}

/**************************************************************************************************/

DecoderWorker::~DecoderWorker() {
    lrpt_qpsk_data_free(qpskInput);
    lrpt_qpsk_data_free(remnants);
    lrpt_qpsk_data_free(temp);
}

/**************************************************************************************************/

void DecoderWorker::process() {
    /* TODO check for errors in the whole class */

    /* Allocate QPSK data object for buffered reading */
    qpskInput = lrpt_qpsk_data_alloc(MTU, NULL);

    /* Allocate helper QPSK data object for storing remnants */
    remnants = lrpt_qpsk_data_alloc(0, NULL);

    /* Allocate temporary QPSK storage for handling extra data */
    temp = lrpt_qpsk_data_alloc(MTU, NULL);

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
    /* TODO should deinterleave here */

    if (dediffcoder)
        lrpt_dsp_dediffcoder_exec(dediffcoder, qpskInput);

    /* TODO dump processed QPSK data here */

    /* Prepare symbols for drawing on constellation */
    size_t n = lrpt_qpsk_data_length(qpskInput);

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

    /* Recalculate how much data left unprocessed */
    n_rem = lrpt_qpsk_data_length(oper) - n_proc;

    /* Copy the remnants */
    if (n_rem != 0) {
        if (oper == qpskInput)
            lrpt_qpsk_data_from_qpsk(remnants, qpskInput, n_proc, n_rem, NULL);
        else {
            lrpt_qpsk_data_from_qpsk(temp, remnants, n_proc, n_rem, NULL);
            lrpt_qpsk_data_from_qpsk(remnants, temp, 0, n_rem, NULL);
        }
    }

    bool frmStatus = lrpt_decoder_framingstate(decoder);
    int frmTotCnt = lrpt_decoder_framestot_cnt(decoder);
    int frmOkCnt = lrpt_decoder_framesok_cnt(decoder);
    int cvcduCnt = lrpt_decoder_cvcdu_cnt(decoder);
    int pckCnt = lrpt_decoder_packets_cnt(decoder);
    int sigQ = lrpt_decoder_sigqual(decoder);

    emit decoderInfo(frmStatus, frmTotCnt, frmOkCnt, cvcduCnt, pckCnt, sigQ);

    /* Get new pixels */
    size_t cnt[6];
    lrpt_decoder_pxls_avail(decoder, cnt);

    for (int i = 0; i < 6; i++) {
        size_t linesAvail = (cnt[i] / (8 * imgStdWidth));
        size_t diff = (linesAvail - linesCnt[i]);
        size_t pxlsRequired = (diff * 8 * imgStdWidth);

        if (pxlsRequired > 0) {
            uint8_t *apid_pxls = new uint8_t[pxlsRequired];
            lrpt_decoder_pxls_get(decoder, apid_pxls, 64 + i, linesCnt[i] * 8 * imgStdWidth, pxlsRequired, NULL);

            linesCnt[i] = linesAvail;

            QVector<int> pxls(apid_pxls, apid_pxls + pxlsRequired);
            emit pxlsAvail(64 + i, pxls);

            delete [] apid_pxls;
        }
    }

    /* TODO we should emit not only this signal but a number of newly available pixels in each APID */
    emit chunkProcessed();
}
