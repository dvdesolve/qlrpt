#include "LRPTChanWidget.h"

#include <QPainter>

/**************************************************************************************************/

LRPTChanWidget::LRPTChanWidget(QWidget *parent) : QWidget(parent) {
}

/**************************************************************************************************/

void LRPTChanWidget::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);

    QRect base(0, 0, this->width(), this->height());
    painter.fillRect(base, QColor(Qt::darkGray));
}
