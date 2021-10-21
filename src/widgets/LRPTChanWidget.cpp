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

#include "LRPTChanWidget.h"

#include <QPainter>

/**************************************************************************************************/

LRPTChanWidget::LRPTChanWidget(QWidget *parent) : QWidget(parent) {
    preview = QImage(0, 0, QImage::Format_Grayscale8);
}

/**************************************************************************************************/

QSize LRPTChanWidget::minimumSizeHint() const {
    return QSize(W, curHeightScaled);
}

/**************************************************************************************************/

QSize LRPTChanWidget::sizeHint() const {
    return QSize(W, curHeightScaled);
}

/**************************************************************************************************/

void LRPTChanWidget::renderPixels(QVector<int> pxls) {
    int addHeight = pxls.size() / stdWidth;
    QImage new_preview(stdWidth, curHeight + addHeight, QImage::Format_Grayscale8);

    /* If old image contains some data - copy it to the top of new one */
    if (curHeight > 0)
        memcpy(new_preview.bits(), preview.bits(), preview.sizeInBytes());

    for (int y = 0; y < addHeight; y++) {
        uchar *line = new_preview.scanLine(curHeight + y);

        for (int x = 0; x < stdWidth; x++)
            *line++ = pxls.at(x);
    }

    preview = new_preview;

    curHeight += addHeight; /* Number of full lines to be rendered */
    curHeightScaled = (curHeight * W / stdWidth + 1); /* Add extra line */
    setMinimumHeight(curHeightScaled);
    resize(W, curHeightScaled);

    update();
}

/**************************************************************************************************/

void LRPTChanWidget::setStdWidth(int width) {
    stdWidth = width;
}

/**************************************************************************************************/

void LRPTChanWidget::clearImage() {
    preview = QImage(0, 0, QImage::Format_Grayscale8);

    curHeight = 0;
    curHeightScaled = 0;
    setMinimumHeight(0);
    resize(W, 0);

    update();
}

/**************************************************************************************************/

void LRPTChanWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    /* If there is something to be rendered */
    if (curHeight > 0) {
        /* TODO support optional smoothing */
        QImage scaled = preview.scaledToWidth(W);
        painter.drawImage(0, 0, scaled);
    }
}
