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

#ifndef IQSOURCEABSTRACTWORKER_H
#define IQSOURCEABSTRACTWORKER_H

/**************************************************************************************************/

#include <QObject>

/**************************************************************************************************/

class IQSourceAbstractWorker : public QObject {
    Q_OBJECT

public:
    explicit IQSourceAbstractWorker();

public slots:
    virtual void process(void) = 0;
};

/**************************************************************************************************/

#endif
