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

#include "QPSKWidget.h"

#include <QPainter>

/**************************************************************************************************/

QPSKWidget::QPSKWidget(QWidget *parent) : QWidget(parent) {
}

/**************************************************************************************************/

void QPSKWidget::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);
    QPen vPen(Qt::white, ((this->width() % 2) == 0) ? 2.0 : 1.0);
    QPen hPen(Qt::white, ((this->height() % 2) == 0) ? 2.0 : 1.0);

    QRect base(0, 0, this->width(), this->height());
    painter.fillRect(base, QColor(Qt::black));

    QLineF vLine((this->width() - 1) / 2.0, 0, (this->width() - 1) / 2.0, this->height());
    QLineF hLine(0, (this->height() - 1) / 2.0, this->width(), (this->height() - 1) / 2.0);
    painter.setPen(vPen);
    painter.drawLine(vLine);
    painter.setPen(hPen);
    painter.drawLine(hLine);
}
