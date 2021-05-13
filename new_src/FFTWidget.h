#ifndef FFTWIDGET_H
#define FFTWIDGET_H

/**************************************************************************************************/

#include <QWidget>

/**************************************************************************************************/

class FFTWidget : public QWidget {
    Q_OBJECT

public:
    explicit FFTWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

/**************************************************************************************************/

#endif
