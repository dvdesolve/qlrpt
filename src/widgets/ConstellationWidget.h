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

#ifndef CONSTELLATIONWIDGET_H
#define CONSTELLATIONWIDGET_H

/**************************************************************************************************/

#include <QWidget>

/**************************************************************************************************/

class ConstellationWidget : public QWidget {
    Q_OBJECT

public:
    explicit ConstellationWidget(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void clearConst();

public slots:
    void drawConst(QVector<int> pts);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    /* Constellation widget will be always 129x129 pixels */
    const int W = 129;
    const int W2 = W / 2;
    const int W_1 = W - 1;
    const int H = 129;
    const int H2 = H / 2;
    const int H_1 = H - 1;

    QVector<int> points;
};

/**************************************************************************************************/

#endif
