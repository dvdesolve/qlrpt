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

#ifndef LRPTCHANWIDGET_H
#define LRPTCHANWIDGET_H

/**************************************************************************************************/

#include <QWidget>

/**************************************************************************************************/

class LRPTChanWidget : public QWidget {
    Q_OBJECT

public:
    explicit LRPTChanWidget(QWidget *parent = nullptr);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void renderPixels(QVector<int> pxls);
    void setStdWidth(int width);

    void clearImage();

protected:
    void paintEvent(QPaintEvent *) override;

private:
    /* Image widget will always be 300 pixels wide */
    const int W = 300;

    int curHeight = 0;
    int curHeightScaled = 0;
    int stdWidth = 0;

    QVector<int> image;
};

/**************************************************************************************************/

#endif
