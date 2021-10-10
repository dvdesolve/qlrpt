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

#ifndef DECODERWORKER_H
#define DECODERWORKER_H

/**************************************************************************************************/

#include <QObject>

#include <lrpt.h>

/**************************************************************************************************/

class DecoderWorker : public QObject {
    Q_OBJECT

public:
    explicit DecoderWorker(lrpt_decoder_t *decoder, int MTU, lrpt_qpsk_file_t *processedDump = NULL);
    ~DecoderWorker();

public slots:
    void process();

signals:
    void finished();
    void decoderInfo(bool, int, int, int, int);

private:
    lrpt_decoder_t *decoder = NULL;
    int MTU;
    lrpt_qpsk_file_t *processedDump = NULL;

    lrpt_qpsk_data_t *qpskInput = NULL;
    lrpt_qpsk_data_t *remnants = NULL;
    lrpt_qpsk_data_t *oper = NULL;
    lrpt_image_t *img = NULL;

    size_t n_rem = 0;
    size_t n_proc = 0;

    void processChunk();

signals:
    void chunkProcessed();
};

/**************************************************************************************************/

#endif
