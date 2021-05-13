#ifndef QPSKWIDGET_H
#define QPSKWIDGET_H

/**************************************************************************************************/

#include <QWidget>

/**************************************************************************************************/

class QPSKWidget : public QWidget {
    Q_OBJECT

public:
    explicit QPSKWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

/**************************************************************************************************/

#endif
