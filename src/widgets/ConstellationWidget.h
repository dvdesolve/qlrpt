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
    QVector<int> points;
};

/**************************************************************************************************/

#endif
