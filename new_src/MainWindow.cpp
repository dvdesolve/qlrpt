#include "MainWindow.h"

/**************************************************************************************************/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    /* TODO scan for SoapySDR sources */

    /* TODO Add the rest of sources */
    SourceCombB->insertSeparator(SourceCombB->count());

    /* Manage source selection properly */
    connect(SourceCombB, SIGNAL(currentIndexChanged(int)), this, SLOT(setNewSource(int)));

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

    /* Set up connections for live APIDs checkboxes */
    connect(APID64ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID65ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID66ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID67ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID68ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID69ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
}

MainWindow::~MainWindow() {
}

/**************************************************************************************************/

void MainWindow::setNewSource(int src) {
    if (src == 0) { /* I/Q file */
    }
    else if (src == 1) { /* QPSK file */
    }
    else { /* Real SDR source */
    }
}

/**************************************************************************************************/

void MainWindow::setLiveAPIDsImagery() {
    LRPTChan64Widget->setVisible(APID64ShowCB->isChecked());
    APID64Lbl->setVisible(APID64ShowCB->isChecked());
    LRPTChan65Widget->setVisible(APID65ShowCB->isChecked());
    APID65Lbl->setVisible(APID65ShowCB->isChecked());
    LRPTChan66Widget->setVisible(APID66ShowCB->isChecked());
    APID66Lbl->setVisible(APID66ShowCB->isChecked());
    LRPTChan67Widget->setVisible(APID67ShowCB->isChecked());
    APID67Lbl->setVisible(APID67ShowCB->isChecked());
    LRPTChan68Widget->setVisible(APID68ShowCB->isChecked());
    APID68Lbl->setVisible(APID68ShowCB->isChecked());
    LRPTChan69Widget->setVisible(APID69ShowCB->isChecked());
    APID69Lbl->setVisible(APID69ShowCB->isChecked());
}
