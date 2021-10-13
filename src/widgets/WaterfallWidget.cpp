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

#include "WaterfallWidget.h"

#include <QPainter>

/**************************************************************************************************/

WaterfallWidget::WaterfallWidget(QWidget *parent) : QWidget(parent) {
    waterfall = new QImage(W, H, QImage::Format_RGB32);
    waterfall->fill(QColor(Qt::black));
}

/**************************************************************************************************/

WaterfallWidget::~WaterfallWidget() {
    delete waterfall;
}

/**************************************************************************************************/

QSize WaterfallWidget::minimumSizeHint() const {
    return QSize(W, H);
}

/**************************************************************************************************/

QSize WaterfallWidget::sizeHint() const {
    return QSize(W, H);
}

/**************************************************************************************************/

void WaterfallWidget::clearWaterfall() {
    bin_val = 0;
    bin_max = 1000;
    curr_bin_max = 0;

    waterfall->fill(QColor(Qt::black));

    update();
}

/**************************************************************************************************/

void WaterfallWidget::drawWaterfall(QVector<int> freqs) {
    memmove(waterfall->bits() + waterfall->bytesPerLine(),
            waterfall->bits(),
            waterfall->sizeInBytes() - waterfall->bytesPerLine());

    QRgb *topline = reinterpret_cast<QRgb *>(waterfall->scanLine(0));

    int idx = 256;

    /* Process positive frequencies */
    for (int i = 0; i < 128; i++) {
      /* Calculate vector magnitude of signal at each frequency (so-called "bin") */
      int val = ifft_binval(freqs.at(idx), freqs.at(idx + 1), false);
      idx += 2;

      /* Color-code signal strength */
      topline[i] = colorize(val);
    }

    idx = 2;

    /* Process negative frequencies */
    for (int i = 1; i < 128; i++) {
      /* Calculate vector magnitude of signal at each frequency (so-called "bin") */
      int val = ifft_binval(freqs.at(idx), freqs.at(idx + 1), false);
      idx += 2;

      /* Color-code signal strength */
      topline[127 + i] = colorize(val);
    }

    /* Reset binning function */
    ifft_binval(freqs.at(0), freqs.at(0), true);

    update();
}

/**************************************************************************************************/

void WaterfallWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    painter.drawImage(0, 0, *waterfall);
}

/**************************************************************************************************/

int WaterfallWidget::ifft_binval(int si, int sq, bool reset) {
    if (reset) { /* Calculate sliding window average of bin_max value */
        bin_max = curr_bin_max;

        if (bin_max == 0)
            bin_max = 1;

        curr_bin_max = 0;
    }
    else { /* Calculate average signal power at each frequency (bin) */
        bin_val  = (bin_val * amp_avg_mult);
        bin_val += (si * si + sq * sq);
        bin_val /= amp_avg_win;

        /* Record max bin value */
        if (curr_bin_max < bin_val)
            curr_bin_max = bin_val;

        /* Scale bin values to 255 depending on max value */
        int x = (255 * bin_val) / bin_max;

        if (x > 255)
            x = 255;

        return x;
    }

    return 0 ;
}

/**************************************************************************************************/

inline QRgb WaterfallWidget::colorize(int val) {
    return QColor(pal[3 * val + 0], pal[3 * val + 1], pal[3 * val + 2]).rgb();
}
