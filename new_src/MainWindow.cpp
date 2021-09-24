#include "MainWindow.h"

#include <QFileDialog>

/**************************************************************************************************/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    /* TODO scan for SoapySDR sources and add them to the sources list */

    SrcCombB->insertSeparator(SrcCombB->count());

    /* TODO just for debug, bogus SDR source */
    SrcCombB->addItem(tr("SDR receiver"));

    /* Manage source selection properly */
    connect(SrcCombB, SIGNAL(currentIndexChanged(int)), this, SLOT(setNewSource(int)));

    /* Browse file button connection */
    connect(SrcFileBrowseBtn, SIGNAL(clicked()), this, SLOT(browseSrcFile()));

    /* Set up status bar labels */
    PLLStatusLbl = new QLabel(tr("PLL status: ---"), this);
    PLLStatusLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PLLStatusLbl->setLineWidth(1);
    PLLStatusLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(PLLStatusLbl);

    PLLFreqLbl = new QLabel(tr("PLL frequency: --- Hz"), this);
    PLLFreqLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PLLFreqLbl->setLineWidth(1);
    PLLFreqLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(PLLFreqLbl);

    ALCGainLbl = new QLabel(tr("ALC gain: --- dB"), this);
    ALCGainLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    ALCGainLbl->setLineWidth(1);
    ALCGainLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(ALCGainLbl);

    FramingStatusLbl = new QLabel(tr("Framing: ---"), this);
    FramingStatusLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    FramingStatusLbl->setLineWidth(1);
    FramingStatusLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(FramingStatusLbl);

    PacketsLbl = new QLabel(tr("Packets: ---/--- (---%)"), this);
    PacketsLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PacketsLbl->setLineWidth(1);
    PacketsLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(PacketsLbl);

    /* Set up connections for live APIDs checkboxes */
    connect(APID64ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID65ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID66ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID67ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID68ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID69ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));

    /* Connect start/stop button with corresponding slot */
    connect(StartStopBtn, SIGNAL(clicked()), this, SLOT(startStopProcessing()));

    /* Set initial UI state */
    updateUIState();
}

MainWindow::~MainWindow() {
}

/**************************************************************************************************/

void MainWindow::updateUIState(void) {
    switch (srcMode) {
        /* When no source is selected (e. g., at startup)*/
        case SrcType::NONE: {
            StartStopBtn->setDisabled(true);

            SrcLbl->setEnabled(true);
            SrcCombB->setEnabled(true);
            SrcRescanBtn->setEnabled(true);

            SrcFilepathLbl->setDisabled(true);
            SrcFilepath->setDisabled(true);
            SrcFileBrowseBtn->setDisabled(true);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(true);
            DecoderTab->setDisabled(true);

            StatusGB->setDisabled(true);
            LRPTGB->setDisabled(true);

            PLLStatusLbl->setDisabled(true);
            PLLFreqLbl->setDisabled(true);
            ALCGainLbl->setDisabled(true);
            FramingStatusLbl->setDisabled(true);
            PacketsLbl->setDisabled(true);

            break;
        }

        case SrcType::IQ_FILE: {
            StartStopBtn->setDisabled(SrcFilepath->text().isEmpty());

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilepathLbl->setDisabled(processing);
            SrcFilepath->setDisabled(processing);
            SrcFileBrowseBtn->setDisabled(processing);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(processing);
            DecoderTab->setDisabled(processing);

            StatusGB->setEnabled(processing);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setEnabled(processing);
            PLLFreqLbl->setEnabled(processing);
            ALCGainLbl->setEnabled(processing);
            FramingStatusLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }

        case SrcType::QPSK_FILE: {
            StartStopBtn->setDisabled(SrcFilepath->text().isEmpty());

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilepathLbl->setDisabled(processing);
            SrcFilepath->setDisabled(processing);
            SrcFileBrowseBtn->setDisabled(processing);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(true);
            DecoderTab->setDisabled(processing);

            StatusGB->setEnabled(processing);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setDisabled(true);
            PLLFreqLbl->setDisabled(true);
            ALCGainLbl->setDisabled(true);
            FramingStatusLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }

        case SrcType::SDR_RECEIVER: {
            StartStopBtn->setEnabled(true);

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilepathLbl->setDisabled(true);
            SrcFilepath->setDisabled(true);
            SrcFileBrowseBtn->setDisabled(true);

            SDRTab->setEnabled(true);

            QList<QWidget *> SDRTabChildWidgets =
                    SDRTab->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);

            /* Enable/disable all widgets on SDR page... */
            for (int i = 0; i < SDRTabChildWidgets.size(); i++)
                SDRTabChildWidgets[i]->setDisabled(processing);

            /* ... except gain-related */
            SDRGainAutoRB->setEnabled(true);
            SDRGainManualRB->setEnabled(true);
            SDRGainSlider->setEnabled(true);

            DemodTab->setDisabled(processing);
            DecoderTab->setDisabled(processing);

            StatusGB->setEnabled(processing);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setEnabled(processing);
            PLLFreqLbl->setEnabled(processing);
            ALCGainLbl->setEnabled(processing);
            FramingStatusLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }
    }
}

/**************************************************************************************************/

void MainWindow::setNewSource(int src) {
    /* TODO implement */
    if (src == -1) /* No source (default) */
        srcMode = SrcType::NONE;
    if (src == 0) /* I/Q file */
        srcMode = SrcType::IQ_FILE;
    else if (src == 1) /* QPSK file */
        srcMode = SrcType::QPSK_FILE;
    else /* Real SDR source */
        srcMode = SrcType::SDR_RECEIVER;

    /* Reflect changes in UI */
    updateUIState();
}

/**************************************************************************************************/

void MainWindow::browseSrcFile(void) {
    QString filter;

    switch (srcMode) {
        case IQ_FILE:
            filter = tr("LRPT I/Q data files (*.iq)");
            break;

        case QPSK_FILE:
            filter = tr("LRPT QPSK data files (*.qpsk)");
            break;

        default:
            filter = tr("All files (*.*)");
        break;
    }

    /* TODO use last directory from settings */
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Select source file"),
                QString(),
                filter);

    if (!fileName.isEmpty()) {
        SrcFilepath->setText(fileName);
        StartStopBtn->setEnabled(true);
    }

    /* TODO show info about file in GUI, if possible */
}

/**************************************************************************************************/

void MainWindow::setLiveAPIDsImagery(void) {
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

/**************************************************************************************************/

void MainWindow::startStopProcessing(void) {
    /* TODO implement */
    processing = !processing;
    StartStopBtn->setText((processing) ? tr("Stop") : tr("Start"));

    /* Reflect changes in UI */
    updateUIState();
}
