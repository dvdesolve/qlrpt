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

#ifndef IQSOURCEFILEWORKER_H
#define IQSOURCEFILEWORKER_H

/**************************************************************************************************/

#include <QObject>

#include <lrpt.h>

/**************************************************************************************************/

class IQSourceFileWorker : public QObject {
    Q_OBJECT

public:
    explicit IQSourceFileWorker(size_t mtu, const QString &srcFile);
    ~IQSourceFileWorker();

public slots:
    void process(void);

signals:
    void finished();

private:
    size_t mtu;
    QString srcFile;

    lrpt_iq_data_t *iqData = NULL;
    lrpt_iq_file_t *iqFile = NULL;
};

/**************************************************************************************************/

#endif
