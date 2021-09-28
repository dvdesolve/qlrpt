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

#ifndef IQPROCESSORWORKER_H
#define IQPROCESSORWORKER_H

/**************************************************************************************************/

#include <QObject>

#include <lrpt.h>

/**************************************************************************************************/

class IQProcessorWorker : public QObject {
    Q_OBJECT

public:
    explicit IQProcessorWorker(size_t mtu, size_t total);
    ~IQProcessorWorker();

public slots:
    void process(void);

signals:
    void finished();

private:
    size_t mtu;
    size_t total;

    lrpt_iq_data_t *iqData = NULL;
    lrpt_iq_file_t *iqFile = NULL;
};

/**************************************************************************************************/

#endif
