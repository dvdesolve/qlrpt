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
    image.clear();
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
    image.append(pxls);

    curHeight = (image.size() / stdWidth); /* Number of full lines to be rendered */
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
    image.clear();

    curHeight = 0;
    curHeightScaled = 0;
    setMinimumHeight(0);
    resize(W, 0);

    update();
}

/**************************************************************************************************/

void LRPTChanWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    /* TODO use more optimal approach - for example, copy old image buffer to new one as with FFT waterfall */
    /* If there is something to be rendered */
    if (curHeight > 0) {
        QImage img = QImage(stdWidth, curHeight, QImage::Format_Grayscale8);

        for (int y = 0; y < curHeight; y++) {
            uchar *line = img.scanLine(y);

            for (int x = 0; x < stdWidth; x++)
                *line++ = image.at(y * stdWidth + x);
        }

        /* TODO support smoothing */
        QImage scaled = img.scaledToWidth(W);
        painter.drawImage(0, 0, scaled);
    }
}
