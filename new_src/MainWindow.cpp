#include "MainWindow.h"

/**************************************************************************************************/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    /* Set up status bar labels */
    PLLStatusLbl = new QLabel(tr("PLL: <span style=\"font-weight: bold; color: #ff0000\">unlocked</span>"), this);
    FramingStatusLbl = new QLabel(tr("Framing: <span style=\"font-weight: bold; color: #ff0000\">no</span>"), this);
    PacketsLbl = new QLabel(tr("Packets: 0/0 (0%)"), this);

    PLLStatusLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PLLStatusLbl->setLineWidth(1);
    PLLStatusLbl->setMidLineWidth(0);
    FramingStatusLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    FramingStatusLbl->setLineWidth(1);
    FramingStatusLbl->setMidLineWidth(0);
    PacketsLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PacketsLbl->setLineWidth(1);
    PacketsLbl->setMidLineWidth(0);

    /* Add labels to status bar */
    statusbar->addPermanentWidget(PLLStatusLbl);
    statusbar->addPermanentWidget(FramingStatusLbl);
    statusbar->addPermanentWidget(PacketsLbl);
}

MainWindow::~MainWindow() {
}

