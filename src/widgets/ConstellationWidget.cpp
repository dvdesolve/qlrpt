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
 * Author: Neoklis Kyriazis
 * Author: Viktor Drobot
 */

/**************************************************************************************************/

#include "ConstellationWidget.h"

#include <QPainter>

/**************************************************************************************************/

ConstellationWidget::ConstellationWidget(QWidget *parent) : QWidget(parent) {
    points.clear();
}

/**************************************************************************************************/

QSize ConstellationWidget::minimumSizeHint() const {
    return QSize(W, H);
}

/**************************************************************************************************/

QSize ConstellationWidget::sizeHint() const {
    return QSize(W, H);
}

/**************************************************************************************************/

void ConstellationWidget::clearConst() {
    points.clear();
    update();
}

/**************************************************************************************************/

void ConstellationWidget::drawConst(QVector<int> pts) {
    points = pts;
    update();
}

/**************************************************************************************************/

void ConstellationWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    QImage constellation = QImage(W, H, QImage::Format_RGB32);
    constellation.fill(QColor(Qt::black));

    for (int i = 0; i < points.size() / 2; i++) {
        /* Usual QPSK coding */
        int x = (W2 + points.at(2 * i) / 2);
        int y = (H2 - points.at(2 * i + 1) / 2);

        QRgb *line = reinterpret_cast<QRgb *>(constellation.scanLine(y));
        line[x] = QColor(Qt::white).rgb();
    }

    painter.drawImage(0, 0, constellation);

    QPen pen(Qt::white, 1.0);
    QLineF vLine(W2, 0, W2, H_1);
    QLineF hLine(0, H2, W_1, H2);

    painter.setPen(pen);
    painter.drawLine(vLine);
    painter.drawLine(hLine);
}
