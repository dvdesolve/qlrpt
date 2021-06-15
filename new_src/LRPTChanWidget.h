#ifndef LRPTCHANWIDGET_H
#define LRPTCHANWIDGET_H

/**************************************************************************************************/

#include <QWidget>

/**************************************************************************************************/

class LRPTChanWidget : public QWidget {
    Q_OBJECT

public:
    explicit LRPTChanWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

/**************************************************************************************************/

#endif
