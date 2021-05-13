#include "FFTWidget.h"

#include <QPainter>

/**************************************************************************************************/

FFTWidget::FFTWidget(QWidget *parent) : QWidget(parent) {
}

/**************************************************************************************************/

void FFTWidget::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);

    QRect base(0, 0, this->width(), this->height());
    painter.fillRect(base, QColor(Qt::black));
}
