#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**************************************************************************************************/

#include "./ui_MainWindow.h"

#include <QMainWindow>

/**************************************************************************************************/

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /* Status bar labels */
    QLabel *PLLStatusLbl;
    QLabel *FramingStatusLbl;
    QLabel *PacketsLbl;
};

/**************************************************************************************************/

#endif
