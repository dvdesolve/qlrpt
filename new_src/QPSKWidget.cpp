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
