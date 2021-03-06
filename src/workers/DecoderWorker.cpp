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

DecoderWorker::DecoderWorker(lrpt_decoder_t *decoder,
        int MTU,
        lrpt_dsp_dediffcoder_t *dediffcoder,
        bool deint) {
    this->decoder = decoder;
    this->MTU = MTU;
    this->dediffcoder = dediffcoder;
    this->deint = deint;
    imgStdWidth = lrpt_decoder_imgwidth(decoder);
    int_chunks.clear();
}

/**************************************************************************************************/

DecoderWorker::~DecoderWorker() {
    if (deint) {
        for (int i = 0; i < int_chunks.size(); i++)
            lrpt_qpsk_data_free(int_chunks.at(i));
    }

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

            if (!deint) {
                /* We should leave at least 3xSFL at the end of RB for non-interleaved data */
                size_t extra = (dataLen % ((3 * lrpt_decoder_sfl()) / 2));
                dataLen -= extra;
            }

            /* Read till QPSK ring buffer become empty (with account to 3xSFL) */
            /* TODO may be rely on decoder exec function */
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

            if (deint) {
                emit qpskConst(QVector<int>(0));

                QVector<int8_t> soft_data;
                soft_data.reserve(2 * intlv_len);

                /* Combine all interleaved data chunks into single one */
                for (int i = 0; i < int_chunks.size(); i++) {
                    size_t n = lrpt_qpsk_data_length(int_chunks.at(i));

                    QVector<int8_t> sft(2 * n);

                    /* TODO may be free resources ASAP */
                    lrpt_qpsk_data_to_soft(int_chunks.at(i), sft.data(), n, NULL);
                    soft_data.append(sft);
                }

                /* Convert to the liblrpt format */
                lrpt_qpsk_data_t *intlvd = lrpt_qpsk_data_create_from_soft(soft_data.data(), intlv_len, NULL);

                /* Immediate cleanup */
                soft_data.clear();
                soft_data.squeeze(); /* TODO may be necessary in other places where QVectors are used too */

                /* And now we can deinterleave the whole data and then process it chunk-by-chunk as usual */
                lrpt_dsp_deinterleaver_exec(intlvd, NULL);

                dataLen = lrpt_qpsk_data_length(intlvd);
                dataRead = 0;

                /* We should leave at least 3xSFL at the end of data buffer for non-interleaved data */
                size_t extra = (dataLen % ((3 * lrpt_decoder_sfl()) / 2));
                dataLen -= extra;

                /* Read till the end of QPSK data (with account to 3xSFL) */
                /* TODO may be rely on decoder exec function */
                while (dataRead < dataLen) {
                    size_t n = ((dataLen - dataRead) < static_cast<size_t>(MTU)) ?
                                (dataLen - dataRead) :
                                MTU;

                    lrpt_qpsk_data_from_qpsk(qpskInput, intlvd, dataRead, n, NULL);

                    decodeChunk();

                    dataRead += n;
                }
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

    /* TODO request remaining pixels */

    emit finished(); /* Tell caller about job end */
}

/**************************************************************************************************/

void DecoderWorker::processChunk() {
    /* Prepare symbols for drawing on constellation */
    size_t n = lrpt_qpsk_data_length(qpskInput);

    if (n > QPSKConstPoints)
        n = QPSKConstPoints;

    lrpt_qpsk_data_to_soft(qpskInput, const_pts, n, NULL);

    QVector<int> pts(const_pts, const_pts + n);
    emit qpskConst(pts);

    if (deint) {
        /* Create separate chunk of data and store pointer to it */
        n = lrpt_qpsk_data_length(qpskInput);

        lrpt_qpsk_data_t *chunk = lrpt_qpsk_data_create_from_qpsk(qpskInput, 0, n, NULL);

        intlv_len += n; /* Remember new interleaved data length */
        int_chunks.append(chunk);
    }
    else
        decodeChunk();

    emit chunkProcessed();
}

/**************************************************************************************************/

void DecoderWorker::decodeChunk() {
    if (dediffcoder)
        lrpt_dsp_dediffcoder_exec(dediffcoder, qpskInput);

    /* TODO dump processed QPSK data here */

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
}
