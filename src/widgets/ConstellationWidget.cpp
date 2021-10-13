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

#include "ConstellationWidget.h"

#include <QPainter>

/**************************************************************************************************/

ConstellationWidget::ConstellationWidget(QWidget *parent) : QWidget(parent) {
    points.clear();
}

/**************************************************************************************************/

QSize ConstellationWidget::minimumSizeHint() const {
    return QSize(129, 129);
}

/**************************************************************************************************/

QSize ConstellationWidget::sizeHint() const {
    return QSize(129, 129);
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

    QRect base(0, 0, width(), height());
    painter.fillRect(base, QColor(Qt::black));

    QPen pen(Qt::white, 1.0);
    QLineF vLine(width() / 2, 0, width() / 2, height());
    QLineF hLine(0, height() / 2, width(), height() / 2);

    painter.setPen(pen);
    painter.drawLine(vLine);
    painter.drawLine(hLine);

    /* TODO may be use QPixmap? */
    for (int i = 0; i < points.size() / 2; i++) {
        int x = points.at(2 * i) / 2;
        int y = points.at(2 * i + 1) / 2;

        /* Usual QPSK coding */

        painter.drawPoint(width() / 2 + x, height() / 2 - y);
    }
}
