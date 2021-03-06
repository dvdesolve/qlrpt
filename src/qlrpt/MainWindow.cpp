/*
 * This file is part of qlrpt.
 *
 * qlrpt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qlrpt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qlrpt. If not, see https://www.gnu.org/licenses/
 *
 * Author: Viktor Drobot
 */

/**************************************************************************************************/

#include "MainWindow.h"

#include "../workers/DecoderWorker.h"
#include "../workers/DemodulatorWorker.h"
#include "GlobalDecls.h"
#include "GlobalObjects.h"
#include "../workers/IQSourceFileWorker.h"
#include "../workers/QPSKSourceFileWorker.h"
#include "../dialogs/SettingsDialog.h"

#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

Q_DECLARE_METATYPE(lrpt_decoder_spacecraft_t);

/**************************************************************************************************/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    /* Set up status bar labels */
    PLLStatusLbl = new QLabel(tr("PLL: ---"), this);
    PLLStatusLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PLLStatusLbl->setLineWidth(1);
    PLLStatusLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(PLLStatusLbl);

    PLLFreqLbl = new QLabel(tr("PLL frequency: --- Hz"), this);
    PLLFreqLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PLLFreqLbl->setLineWidth(1);
    PLLFreqLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(PLLFreqLbl);

    PLLPhaseErrLbl = new QLabel(tr("PLL phase error: ---"), this);
    PLLPhaseErrLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PLLPhaseErrLbl->setLineWidth(1);
    PLLPhaseErrLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(PLLPhaseErrLbl);

    ALCGainLbl = new QLabel(tr("ALC gain: --- dB"), this);
    ALCGainLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    ALCGainLbl->setLineWidth(1);
    ALCGainLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(ALCGainLbl);

    SignalLevelLbl = new QLabel(tr("Signal level: ---"), this);
    SignalLevelLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    SignalLevelLbl->setLineWidth(1);
    SignalLevelLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(SignalLevelLbl);

    FramingStatusLbl = new QLabel(tr("Framing: ---"), this);
    FramingStatusLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    FramingStatusLbl->setLineWidth(1);
    FramingStatusLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(FramingStatusLbl);

    FramesLbl = new QLabel(tr("Frames: ---/--- (---%)"), this);
    FramesLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    FramesLbl->setLineWidth(1);
    FramesLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(FramesLbl);

    CVCDUsLbl = new QLabel(tr("CVCDUs: ---"), this);
    CVCDUsLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    CVCDUsLbl->setLineWidth(1);
    CVCDUsLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(CVCDUsLbl);

    PacketsLbl = new QLabel(tr("Packets: ---"), this);
    PacketsLbl->setFrameStyle(QFrame::Box | QFrame::Raised);
    PacketsLbl->setLineWidth(1);
    PacketsLbl->setMidLineWidth(0);
    statusbar->addPermanentWidget(PacketsLbl);

    DecoderSpacecraftCombB->addItem(tr("Meteor-M2"), QVariant::fromValue(LRPT_DECODER_SC_METEORM2));
    DecoderSpacecraftCombB->addItem(tr("Meteor-M2-1"), QVariant::fromValue(LRPT_DECODER_SC_METEORM2_1));
    DecoderSpacecraftCombB->addItem(tr("Meteor-M2-2"), QVariant::fromValue(LRPT_DECODER_SC_METEORM2_2));

    /* Read stored settings */
    restoreSettings();

    SrcCombB->insertSeparator(SrcCombB->count());

    /* TODO scan for SoapySDR sources and add them to the sources list */
    SrcCombB->addItem(tr("SDR receiver")); /* TODO just for debug, bogus SDR source */

    /* Handle source change */
    connect(SrcCombB, SIGNAL(currentIndexChanged(int)), this, SLOT(setNewSource(int)));

    /* Connections for menu entries */
    connect(SettingsAct, SIGNAL(triggered()), this, SLOT(openSettingsDlg()));
    connect(ExitAct, SIGNAL(triggered()), this, SLOT(exitApp()));
    connect(AboutAct, SIGNAL(triggered()), this, SLOT(aboutApp()));

    /* Source file related connections */
    connect(SrcFileBrowseBtn, SIGNAL(clicked()), this, SLOT(browseSrcFile()));
    connect(SrcFilePath, SIGNAL(textChanged(QString)), this, SLOT(showFileInfo(QString)));

    /* Dump data related connections */
    connect(DDRcvDirBrowseBtn, SIGNAL(clicked()), this, SLOT(browseRcvDumpDir()));
    connect(DDDemodDirBrowseBtn, SIGNAL(clicked()), this, SLOT(browseDemodDumpDir()));
    /* TODO implement directory selection for output images */

    /* Set up connections for data processing and dumping checkboxes */
    connect(DemodFilterCB, SIGNAL(toggled(bool)), this, SLOT(setDDItems()));
    connect(DecoderDiffcodedCB, SIGNAL(toggled(bool)), this, SLOT(setDDItems()));
    connect(DecoderInterleavedCB, SIGNAL(toggled(bool)), this, SLOT(setDDItems()));
    connect(DDRcvCB, SIGNAL(toggled(bool)), this, SLOT(setDDItems()));
    connect(DDDemodCB, SIGNAL(toggled(bool)), this, SLOT(setDDItems()));

    /* Handle locked/unlocked thresholds for PLL correctly */
    connect(DemodPLLLockedSB, SIGNAL(valueChanged(double)), this, SLOT(handlePLLThresholds()));
    connect(DemodPLLUnlockedSB, SIGNAL(valueChanged(double)), this, SLOT(handlePLLThresholds()));

    /* Set up connections for live APIDs checkboxes */
    connect(APID64ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID65ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID66ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID67ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID68ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));
    connect(APID69ShowCB, SIGNAL(stateChanged(int)), this, SLOT(setLiveAPIDsImagery()));

    /* Additional UI interactions */
    connect(SDRGainManualRB, SIGNAL(toggled(bool)), SDRGainSlider, SLOT(setEnabled(bool)));

    /* Connect start/stop button with corresponding slot */
    connect(StartStopBtn, SIGNAL(clicked()), this, SLOT(startStopProcessing()));

    /* Set initial UI state */
    updateUI();
}

/**************************************************************************************************/

MainWindow::~MainWindow() {
    /* Free allocated global resources */
    lrpt_iq_rb_free(iqRB);
    lrpt_qpsk_rb_free(qpskRB);
    delete iqRBUsed;
    delete iqRBFree;
    delete qpskRBUsed;
    delete qpskRBFree;
}

/**************************************************************************************************/

void MainWindow::closeEvent(QCloseEvent *event) {
    /* TODO implement */
    /* TODO save settings here */
    event->accept();
}

/**************************************************************************************************/

void MainWindow::restoreSettings() {
    QSettings s("desolve", "qlrpt");

    lastSrcFileDir = s.value(
                "Paths/LastSrcDir",
                QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();

    int xi;
    double xf;

    /* IQ source file MTU */
    xi = s.value("IO/IQSrcFileMTU", IQSrcFileMTU_DEF).toInt();

    if ((xi < IQSrcFileMTU_MIN) || (xi > IQSrcFileMTU_MAX)) {
        xi = IQSrcFileMTU_DEF;
        s.setValue("IO/IQSrcFileMTU", xi);
    }

    iqSrcFileMTU = xi * 1024; /* Convert from ksamples/s */

    /* QPSK source file MTU */
    xi = s.value("IO/QPSKSrcFileMTU", QPSKSrcFileMTU_DEF).toInt();

    if ((xi < QPSKSrcFileMTU_MIN) || (xi > QPSKSrcFileMTU_MAX)) {
        xi = QPSKSrcFileMTU_DEF;
        s.setValue("IO/QPSKSrcFileMTU", xi);
    }

    qpskSrcFileMTU = xi * 1024; /* Convert from ksymbols/s */

    /* IQ ring buffer size */
    xi = s.value("IO/IQRBSize", IQRBSize_DEF).toInt();

    if ((xi < IQRBSize_MIN) || (xi > IQRBSize_MAX)) {
        xi = IQRBSize_DEF;
        s.setValue("IO/IQRBSize", xi);
    }

    iqRBSize = xi * 1024; /* Convert from ksamples/s */
    IQBufferUtilBar->setMaximum(iqRBSize); /* Update maximum of indicator buffer */

    /* QPSK ring buffer size */
    xi = s.value("IO/QPSKRBSize", QPSKRBSize_DEF).toInt();

    if ((xi < QPSKRBSize_MIN) || (xi > QPSKRBSize_MAX)) {
        xi = QPSKRBSize_DEF;
        s.setValue("IO/QPSKRBSize", xi);
    }

    qpskRBSize = xi * 1024; /* Convert from ksymbols/s */
    QPSKBufferUtilBar->setMaximum(qpskRBSize); /* Update maximum of indicator buffer */

    /* Demodulator chunk size */
    xi = s.value("IO/DemodChunkSize", DemodChunkSize_DEF).toInt();

    if ((xi != 0) && ((xi < DemodChunkSize_MIN) || (xi > DemodChunkSize_MAX))) {
        xi = DemodChunkSize_DEFINIT;
        s.setValue("IO/DemodChunkSize", xi);
    }

    demodChunkSize = xi * 1024; /* Convert from ksamples/s */

    /* Decoder chunk size */
    xi = s.value("IO/DecoderChunkSize", DecoderChunkSize_DEF).toInt();

    if ((xi < DecoderChunkSize_MIN) || (xi > DecoderChunkSize_MAX)) {
        xi = DecoderChunkSize_DEF;
        s.setValue("IO/DecoderChunkSize", xi);
    }

    decoderChunkSize = (xi * lrpt_decoder_sfl()) / 2;  /* Convert from SFLs to the number of QPSK symbols */

    /* SDR frequency */
    xf = s.value("SDR/Frequency", SDRFrequency_DEF).toDouble();

    if ((xf < SDRFrequency_MIN) || (xf > SDRFrequency_MAX)) {
        xf = SDRFrequency_DEF;
        s.setValue("SDR/Frequency", xf);
    }

    SDRFrequencySB->setValue(xf);

    /* SDR bandwidth */
    xf = s.value("SDR/Bandwidth", SDRBandwidth_DEF).toDouble();

    if ((xf < SDRBandwidth_MIN) || (xf > SDRBandwidth_MAX)) {
        xf = SDRBandwidth_DEF;
        s.setValue("SDR/Bandwidth", xf);
    }

    SDRBandwidthSB->setValue(xf);

    /* Whether to use I/Q sample filtering during demodulation */
    DemodFilterCB->setChecked(s.value("Demodulator/Filter", true).toBool());

    /* Filter ripple level (in percents) */
    xf = s.value("Demodulator/FilterRipple", DemodFilterRipple_DEF).toDouble();

    if ((xf < DemodFilterRipple_MIN) || (xf > DemodFilterRipple_MAX)) {
        xf = DemodFilterRipple_DEF;
        s.setValue("Demodulator/FilterRipple", xf);
    }

    filterRipple = xf;

    /* Number of filter poles */
    xi = s.value("Demodulator/FilterPoles", DemodFilterPoles_DEF).toInt();

    if ((xi < DemodFilterPoles_MIN) || (xi > DemodFilterPoles_MAX) || ((xi % 2) != 0)) {
        xi = DemodFilterPoles_DEF;
        s.setValue("Demodulator/FilterPoles", xi);
    }

    filterPoles = xi;

    /* PLL locked threshold */
    xf = s.value("Demodulator/PLLLocked", DemodPLLLocked_DEF).toDouble();

    if ((xf < DemodPLLLocked_MIN) || (xf > DemodPLLLocked_MAX)) {
        xf = DemodPLLLocked_DEF;
        s.setValue("Demodulator/PLLLocked", xf);
    }

    DemodPLLLockedSB->setValue(xf);

    /* PLL unlocked threshold */
    xf = s.value("Demodulator/PLLUnlocked", DemodPLLUnlocked_DEF).toDouble();

    if ((xf < DemodPLLUnlocked_MIN) || (xf > DemodPLLUnlocked_MAX)) {
        xf = DemodPLLUnlocked_DEF;
        s.setValue("Demodulator/PLLUnlocked", xf);
    }

    DemodPLLUnlockedSB->setValue(xf);

    /* RRC order */
    xi = s.value("Demodulator/RRCOrder", DemodRRCOrder_DEF).toInt();

    if ((xi < DemodRRCOrder_MIN) || (xi > DemodRRCOrder_MAX)) {
        xi = DemodRRCOrder_DEF;
        s.setValue("Demodulator/RRCOrder", xi);
    }

    DemodRRCOrderSB->setValue(xi);

    /* RRC alpha */
    xf = s.value("Demodulator/RRCAlpha", DemodRRCAlpha_DEF).toDouble();

    if ((xf < DemodRRCAlpha_MIN) || (xf > DemodRRCAlpha_MAX)) {
        xf = DemodRRCAlpha_DEF;
        s.setValue("Demodulator/RRCAlpha", xf);
    }

    DemodRRCAlphaSB->setValue(xf);

    /* Interpolation factor */
    xi = s.value("Demodulator/RRCOrder", DemodRRCOrder_DEF).toInt();

    if ((xi < DemodRRCOrder_MIN) || (xi > DemodRRCOrder_MAX)) {
        xi = DemodRRCOrder_DEF;
        s.setValue("Demodulator/RRCOrder", xi);
    }

    DemodRRCOrderSB->setValue(xi);

    /* TODO bind with spacecraft type */
    /* Whether to use offset QPSK demodulator */
    DemodOffsetCB->setChecked(s.value("Demodulator/OQPSK", false).toBool());

    /* Spacecraft type */
    lrpt_decoder_spacecraft_t xsc =
            s.value("Decoder/Spacecraft",
                    QVariant::fromValue(LRPT_DECODER_SC_METEORM2)).value<lrpt_decoder_spacecraft_t>();

    DecoderSpacecraftCombB->setCurrentIndex(DecoderSpacecraftCombB->findData(xsc));

    /* TODO bind with spacecraft type */
    /* Differential coding */
    DecoderDiffcodedCB->setChecked(s.value("Decoder/Diffcoding", false).toBool());

    /* TODO bind with spacecraft type */
    /* Interleaving */
    DecoderInterleavedCB->setChecked(s.value("Decoder/Interleaving", false).toBool());

    s.sync();
}

/**************************************************************************************************/

void MainWindow::setGlobalObjects(SrcType src) {
    /* First we need to free previously allocated global resources */
    lrpt_iq_rb_free(iqRB);
    lrpt_qpsk_rb_free(qpskRB);
    delete iqRBUsed;
    delete iqRBFree;
    delete qpskRBUsed;
    delete qpskRBFree;

    if (src == NO_SRC) { /* NULLify global resources */
        iqRB = NULL;
        qpskRB = NULL;
        iqRBUsed = nullptr;
        iqRBFree = nullptr;
        qpskRBUsed = nullptr;
        qpskRBFree = nullptr;
    }
    else { /* Re-allocate global resources */
        /* I/Q data buffer for SDR and file sources */
        if ((src == IQ_FILE) || (src == SDR_RECEIVER)) {
            iqRB = lrpt_iq_rb_alloc(iqRBSize, NULL); /* TODO implement verbose error reporting */

            if (!iqRB) {
                QMessageBox::critical(this, tr("qlrpt error"), tr("Can't allocate I/Q data ring buffer object!"), QMessageBox::Close);

                /* Deinit all previously allocated objects */
                setGlobalObjects(NO_SRC);

                return;
            }

            iqRBUsed = new QSemaphore(0);
            iqRBFree = new QSemaphore(iqRBSize);
        }
        else {
            iqRB = NULL;
            iqRBUsed = nullptr;
            iqRBFree = nullptr;
        }

        /* QPSK data buffer for all sources */
        qpskRB = lrpt_qpsk_rb_alloc(qpskRBSize, NULL); /* TODO implement verbose error reporting */

        if (!qpskRB) {
            QMessageBox::critical(this, tr("qlrpt error"), tr("Can't allocate QPSK data ring buffer object!"), QMessageBox::Close);

            /* Deinit all previously allocated objects */
            setGlobalObjects(NO_SRC);

            return;
        }

        qpskRBUsed = new QSemaphore(0);
        qpskRBFree = new QSemaphore(qpskRBSize);
    }
}

/**************************************************************************************************/

void MainWindow::updateUI() {
    switch (srcMode) {
        /* No source is selected */
        case NO_SRC: {
            StartStopBtn->setDisabled(true);

            SrcLbl->setEnabled(true);
            SrcCombB->setEnabled(true);
            SrcRescanBtn->setEnabled(true);

            SrcFilePathLbl->setDisabled(true);
            SrcFilePath->setDisabled(true);
            SrcFileBrowseBtn->setDisabled(true);

            TotalProgressBar->setValue(0);
            TotalProgressBar->setDisabled(true);

            SrcInfoText->setDisabled(true);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(true);
            DecoderTab->setDisabled(true);
            PPTab->setDisabled(true);
            DDTab->setDisabled(true);

            StatusGB->setDisabled(true);
            WaterfallGB->setDisabled(true);
            ConstellationGB->setDisabled(true);
            IQBufferUtilBar->setDisabled(true);
            QPSKBufferUtilBar->setDisabled(true);
            LockQualityBar->setDisabled(true);
            SignalQualityBar->setDisabled(true);
            LRPTGB->setDisabled(true);

            PLLStatusLbl->setDisabled(true);
            PLLFreqLbl->setDisabled(true);
            PLLPhaseErrLbl->setDisabled(true);
            ALCGainLbl->setDisabled(true);
            SignalLevelLbl->setDisabled(true);
            FramingStatusLbl->setDisabled(true);
            FramesLbl->setDisabled(true);
            CVCDUsLbl->setDisabled(true);
            PacketsLbl->setDisabled(true);

            break;
        }

        case IQ_FILE: {
            StartStopBtn->setDisabled(SrcFilePath->text().isEmpty() || processing);

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilePathLbl->setDisabled(processing);
            SrcFilePath->setDisabled(processing);
            SrcFileBrowseBtn->setDisabled(processing);

            TotalProgressBar->setValue(0);
            TotalProgressBar->setEnabled(true);

            SrcInfoText->setEnabled(true);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(processing);
            DecoderTab->setDisabled(processing);
            PPTab->setDisabled(processing);
            DDTab->setDisabled(processing);

            setDDItems();

            StatusGB->setEnabled(processing);
            WaterfallGB->setEnabled(true);
            ConstellationGB->setEnabled(true);
            IQBufferUtilBar->setEnabled(true);
            QPSKBufferUtilBar->setEnabled(true);
            LockQualityBar->setEnabled(true);
            SignalQualityBar->setEnabled(true);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setEnabled(processing);
            PLLFreqLbl->setEnabled(processing);
            PLLPhaseErrLbl->setEnabled(processing);
            ALCGainLbl->setEnabled(processing);
            SignalLevelLbl->setEnabled(processing);
            FramingStatusLbl->setEnabled(processing);
            FramesLbl->setEnabled(processing);
            CVCDUsLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }

        case QPSK_FILE: {
            StartStopBtn->setDisabled(SrcFilePath->text().isEmpty() || processing);

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilePathLbl->setDisabled(processing);
            SrcFilePath->setDisabled(processing);
            SrcFileBrowseBtn->setDisabled(processing);

            TotalProgressBar->setValue(0);
            TotalProgressBar->setEnabled(true);

            SrcInfoText->setEnabled(true);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(true);
            DecoderTab->setDisabled(processing);
            PPTab->setDisabled(processing);
            DDTab->setDisabled(processing);

            setDDItems();

            StatusGB->setEnabled(processing);
            WaterfallGB->setDisabled(true);
            ConstellationGB->setEnabled(true);
            IQBufferUtilBar->setDisabled(true);
            QPSKBufferUtilBar->setEnabled(true);
            LockQualityBar->setDisabled(true);
            SignalQualityBar->setEnabled(true);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setDisabled(true);
            PLLFreqLbl->setDisabled(true);
            PLLPhaseErrLbl->setDisabled(true);
            ALCGainLbl->setDisabled(true);
            SignalLevelLbl->setDisabled(true);
            FramingStatusLbl->setEnabled(processing);
            FramesLbl->setEnabled(processing);
            CVCDUsLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }

        case SDR_RECEIVER: { /* TODO need to control specific GUI elements (such as bias tee) depending on receiver's capabilities */
            StartStopBtn->setEnabled(true);

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilePathLbl->setDisabled(true);
            SrcFilePath->setDisabled(true);
            SrcFileBrowseBtn->setDisabled(true);

            TotalProgressBar->setValue(0);
            TotalProgressBar->setEnabled(true);

            SrcInfoText->setEnabled(true);

            SDRTab->setEnabled(true);

            QList<QWidget *> SDRTabChildWidgets =
                    SDRTab->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);

            /* Enable/disable all widgets on SDR page... */
            for (int i = 0; i < SDRTabChildWidgets.size(); i++)
                SDRTabChildWidgets[i]->setDisabled(processing);

            /* ... except gain-related */
            SDRGainAutoRB->setEnabled(true);
            SDRGainManualRB->setEnabled(true);
            SDRGainSlider->setEnabled(SDRGainManualRB->isChecked());

            DemodTab->setDisabled(processing);
            DecoderTab->setDisabled(processing);
            PPTab->setDisabled(processing);
            DDTab->setDisabled(processing);

            setDDItems();

            StatusGB->setEnabled(processing);
            WaterfallGB->setEnabled(true);
            ConstellationGB->setEnabled(true);
            IQBufferUtilBar->setEnabled(true);
            QPSKBufferUtilBar->setEnabled(true);
            LockQualityBar->setEnabled(true);
            SignalQualityBar->setEnabled(true);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setEnabled(processing);
            PLLFreqLbl->setEnabled(processing);
            PLLPhaseErrLbl->setEnabled(processing);
            ALCGainLbl->setEnabled(processing);
            SignalLevelLbl->setEnabled(processing);
            FramingStatusLbl->setEnabled(processing);
            FramesLbl->setEnabled(processing);
            CVCDUsLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }
    }
}

/**************************************************************************************************/

void MainWindow::exitApp() {
    /* TODO implement */
    QCoreApplication::quit();
}

/**************************************************************************************************/

void MainWindow::aboutApp() {
    QMessageBox::about(this,
                       tr("About qlrpt"),
                       tr("<b>qlrpt " qlrpt_VERSION_FULL "</b><br/>Interactive GUI application for receiving, decoding and displaying LRPT images."));
}

/**************************************************************************************************/

void MainWindow::openSettingsDlg() {
    SettingsDialog d;

    /* Re-read settings and update globals if something changed */
    if (d.exec() == QDialog::Accepted) {
        restoreSettings();
        setGlobalObjects(srcMode);
    }
}

/**************************************************************************************************/

void MainWindow::setNewSource(int src) {
    /* Clear previously filled data in source fields */
    SrcFilePath->clear();
    SrcInfoText->clear();

    if (src == -1) /* No source (default) */
        srcMode = NO_SRC;
    else if (src == 0) /* I/Q file */
        srcMode = IQ_FILE;
    else if (src == 1) /* QPSK file */
        srcMode = QPSK_FILE;
    else /* Real SDR source */
        srcMode = SDR_RECEIVER; /* TODO implement */
    /* TODO other sources (such as TCP socket) can be added in the future here */

    /* Reset global objects after source switching */
    setGlobalObjects(srcMode);

    /* Reflect changes in UI */
    updateUI();
}

/**************************************************************************************************/

void MainWindow::browseSrcFile() {
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

    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Select source file to open"),
                lastSrcFileDir,
                filter);

    if (!fileName.isEmpty()) {
        /* Show info about selected file */
        showFileInfo(fileName);

        /* Store last directory */
        lastSrcFileDir = QFileInfo(fileName).absolutePath();

        QSettings s("desolve", "qlrpt");

        s.setValue("Paths/LastSrcDir", lastSrcFileDir);
        s.sync();
    }
}

/**************************************************************************************************/

void MainWindow::showFileInfo(const QString &fileName) {
    if (fileName.isEmpty()) {
        StartStopBtn->setDisabled(true);

        return;
    }

    SrcFilePath->setText(fileName);
    StartStopBtn->setEnabled(true);
    SrcInfoText->clear();

    /* Error object for reporting */
    lrpt_error_t *err = lrpt_error_init();

    /* Convert QString to C string (with UTF-8 support) */
    QByteArray fileNameUTF8 = fileName.toUtf8();
    const char *fileNameCString = fileNameUTF8.data();

    switch (srcMode) {
        case IQ_FILE: {
            lrpt_iq_file_t *f = lrpt_iq_file_open_r(fileNameCString, err);

            if (!f) {
                if (!err)
                    LogText->insertPlainText(tr("Can't open I/Q data file [%1]!").arg(fileName));
                else
                    LogText->insertPlainText(tr("Can't open I/Q data file [%1]: %2").
                                             arg(fileName, QString::fromUtf8(lrpt_error_message(err))));

                lrpt_error_deinit(err);

                return;
            }

            TotalProgressBar->setMaximum(lrpt_iq_file_length(f));

            SrcInfoText->insertPlainText(tr("Source type: I/Q file\n"));
            SrcInfoText->insertPlainText(tr("I/Q file version: %1\n").
                                         arg(QString::number(lrpt_iq_file_version(f))));
            SrcInfoText->insertPlainText(tr("Flags: %1\n").
                                         arg((lrpt_iq_file_is_offsetted(f)) ?
                                                  tr("offsetted") :
                                                  ""));
            SrcInfoText->insertPlainText(tr("Sampling rate: %1 samples/s\n").
                                         arg(QString::number(lrpt_iq_file_samplerate(f))));
            SrcInfoText->insertPlainText(tr("Bandwidth: %1 Hz\n").
                                         arg(QString::number(lrpt_iq_file_bandwidth(f))));
            SrcInfoText->insertPlainText(tr("Device name: %1\n").
                                         arg(QString::fromUtf8(lrpt_iq_file_devicename(f))));
            SrcInfoText->insertPlainText(tr("File size: %1 samples\n").
                                         arg(QString::number(lrpt_iq_file_length(f))));

            lrpt_iq_file_close(f);

            break;
        }

        case QPSK_FILE: {
            lrpt_qpsk_file_t *f = lrpt_qpsk_file_open_r(fileNameCString, err);

            if (!f) {
                if (!err)
                    LogText->insertPlainText(tr("Can't open QPSK data file [%1]!").arg(fileName));
                else
                    LogText->insertPlainText(tr("Can't open QPSK data file [%1]: %2").
                                             arg(fileName, QString::fromUtf8(lrpt_error_message(err))));

                lrpt_error_deinit(err);

                return;
            }

            TotalProgressBar->setMaximum(lrpt_qpsk_file_length(f));

            SrcInfoText->insertPlainText(tr("Source type: QPSK file\n"));
            SrcInfoText->insertPlainText(tr("QPSK file version: %1\n").
                                         arg(QString::number(lrpt_qpsk_file_version(f))));
            SrcInfoText->insertPlainText(tr("Flags: %1%2\n").
                                         arg((lrpt_qpsk_file_is_diffcoded(f)) ?
                                                 tr("diffcoded ") :
                                                 "",
                                             (lrpt_qpsk_file_is_interleaved(f)) ?
                                                 tr("interleaved") :
                                                 ""));
            SrcInfoText->insertPlainText(tr("Symbol type: %1\n").
                                         arg((lrpt_qpsk_file_is_hardsymboled(f)) ?
                                                 tr("hard") :
                                                 tr("soft")));
            SrcInfoText->insertPlainText(tr("Symbol rate: %1 symbols/s\n").
                                         arg(QString::number(lrpt_qpsk_file_symrate(f))));
            SrcInfoText->insertPlainText(tr("File size: %1 symbols\n").
                                         arg(QString::number(lrpt_qpsk_file_length(f))));

            lrpt_qpsk_file_close(f);

            break;
        }

        default:
            break;
    }

    /* Free resources allocated by error object */
    lrpt_error_deinit(err);
}

/**************************************************************************************************/

void MainWindow::setDDItems() {
    bool rcvEnabled = (srcMode == SDR_RECEIVER) ? true : false;

    DDRcvCB->setEnabled(rcvEnabled);
    DDRcvDirPath->setEnabled(rcvEnabled && DDRcvCB->isChecked());
    DDRcvDirBrowseBtn->setEnabled(rcvEnabled && DDRcvCB->isChecked());

    bool demodEnabled;

    if ((srcMode == IQ_FILE) || (srcMode == SDR_RECEIVER))
        demodEnabled = true;
    else
        demodEnabled = false;

    DDDemodCB->setEnabled(demodEnabled);
    DDDemodDirPath->setEnabled(demodEnabled && DDDemodCB->isChecked());
    DDDemodDirBrowseBtn->setEnabled(demodEnabled && DDDemodCB->isChecked());
}

/**************************************************************************************************/

void MainWindow::handlePLLThresholds() {
    if (DemodPLLLockedSB->value() >= DemodPLLUnlockedSB->value())
        DemodPLLThresholdsLbl->setStyleSheet("QLabel { color: #ff0000 }");
    else
        DemodPLLThresholdsLbl->setStyleSheet("");
}

/**************************************************************************************************/

void MainWindow::browseRcvDumpDir() {
    /* TODO browse for directory */
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Specify dump file to save"),
                lastRcvDumpFileDir,
                tr("LRPT I/Q data files (*.iq)"));

    if (!fileName.isEmpty()) { /* TODO disable start-stop button in other case */
        if (QFileInfo(fileName).suffix() != "iq")
            fileName += ".iq";

        DDRcvDirPath->setText(fileName);

        /* Store last directory */
        lastRcvDumpFileDir = QFileInfo(fileName).absolutePath();

        QSettings s("desolve", "qlrpt");

        s.setValue("Paths/LastRcvDumpDir", lastRcvDumpFileDir);
        s.sync();
    }
}

/**************************************************************************************************/

void MainWindow::browseDemodDumpDir() {
    /* TODO browse for directory */
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr("Specify dump file to save"),
                lastDemodDumpFileDir,
                tr("LRPT QPSK data files (*.qpsk)"));

    if (!fileName.isEmpty()) { /* TODO disable start-stop button in other case */
        if (QFileInfo(fileName).suffix() != "qpsk")
            fileName += ".qpsk";

        DDDemodDirPath->setText(fileName);

        /* Store last directory */
        lastDemodDumpFileDir = QFileInfo(fileName).absolutePath();

        QSettings s("desolve", "qlrpt");

        s.setValue("Paths/LastDemodDumpDir", lastDemodDumpFileDir);
        s.sync();
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

/**************************************************************************************************/

void MainWindow::startStopProcessing() {
    processing = !processing; /* Switch mode */

    StartStopBtn->setText((processing) ? tr("Stop") : tr("Start"));
    SettingsAct->setDisabled(processing);

    if (processing) {
        LRPTChan64Widget->clearImage();
        LRPTChan65Widget->clearImage();
        LRPTChan66Widget->clearImage();
        LRPTChan67Widget->clearImage();
        LRPTChan68Widget->clearImage();
        LRPTChan69Widget->clearImage();

        LogText->insertPlainText(tr("Processing started at %1 UTC\n").
                arg(QDateTime::currentDateTimeUtc().toString("HH:mm:ss")));
    }
    else /* TODO print final summary */
        LogText->insertPlainText(tr("Processing finished at %1 UTC\n\n").
                arg(QDateTime::currentDateTimeUtc().toString("HH:mm:ss")));

    if ((srcMode == IQ_FILE) && processing) {
        /* Convert QString to C string (with UTF-8 support) */
        QByteArray fileNameUTF8 = SrcFilePath->text().toUtf8();
        const char *fileNameCString = fileNameUTF8.data();

        /* Open I/Q file for reading */
        iqSrcFile = lrpt_iq_file_open_r(fileNameCString, NULL); /* TODO error reporting */

        /* Initialize QPSK demodulator */
        demodulator = lrpt_demodulator_init(
                    DemodOffsetCB->isChecked(),
                    DemodPLLBandwidthSB->value(),
                    DemodInterpolationFactorSB->value(),
                    lrpt_iq_file_samplerate(iqSrcFile),
                    (DecoderInterleavedCB->isChecked()) ? 80000 : 72000,
                    DemodRRCOrderSB->value(),
                    DemodRRCAlphaSB->value(),
                    DemodPLLLockedSB->value(),
                    DemodPLLUnlockedSB->value(),
                    NULL); /* TODO error reporting */

        /* Initialize Chebyshev filter (if requested) */
        if (DemodFilterCB->isChecked())
            chebFilter = lrpt_dsp_filter_init(
                        lrpt_iq_file_bandwidth(iqSrcFile),
                        lrpt_iq_file_samplerate(iqSrcFile),
                        filterRipple,
                        filterPoles,
                        LRPT_DSP_FILTER_TYPE_LOWPASS,
                        NULL); /* TODO error reporting */

        /* TODO add timestamp and unique name */
        /* Open demodulated QPSK file for writing (if requested) */
        if (DDDemodCB->isChecked() && DDDemodDirPath->isEnabled() && !DDDemodDirPath->text().isEmpty()) {
            fileNameUTF8 = DDDemodDirPath->text().toUtf8();
            fileNameCString = fileNameUTF8.data();

            ddDemodFile = lrpt_qpsk_file_open_w_v1(
                        fileNameCString,
                        DecoderDiffcodedCB->isChecked(),
                        DecoderInterleavedCB->isChecked(),
                        false, /* TODO add GUI flag for soft/hard symbols */
                        (DecoderInterleavedCB->isChecked()) ? 80000 : 72000,
                        NULL); /* TODO error reporting */
        }

        /* Initialize LRPT decoder */
        lrpt_decoder_spacecraft_t sc = DecoderSpacecraftCombB->currentData().value<lrpt_decoder_spacecraft_t>();

        LRPTChan64Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan65Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan66Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan67Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan68Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan69Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));

        decoder = lrpt_decoder_init(sc, NULL); /* TODO error reporting */

        /* Initialize dediffcoder (if requested) */
        if (DecoderDiffcodedCB->isChecked())
            dediffcoder = lrpt_dsp_dediffcoder_init(NULL); /* TODO error reporting */

        /* Allocate new thread and worker for I/Q file reading */
        iqSrcThread = new QThread();
        iqSrcWorker = new IQSourceFileWorker(iqSrcFile, iqSrcFileMTU);

        /* Move worker into separate thread and set up connections */
        iqSrcWorker->moveToThread(iqSrcThread);

        connect(iqSrcThread, SIGNAL(started()), iqSrcWorker, SLOT(process()));
        connect(iqSrcWorker, SIGNAL(finished()), this, SLOT(finishSrcFileWorker()));
        connect(iqSrcWorker, SIGNAL(chunkProcessed()), this, SLOT(updateBuffersIndicators()));
        connect(iqSrcWorker, SIGNAL(readProgress(int)), this, SLOT(updateReadProgress(int)));

        /* Allocate new thread and worker for demodulator */
        qpskSrcThread = new QThread();
        qpskSrcWorker = new DemodulatorWorker(demodulator,
                    (demodChunkSize == 0) ? (DemodChunkSize_DEFINIT * 1024) : demodChunkSize,
                    chebFilter,
                    ddDemodFile);

        /* Move worker into separate thread and set up connections */
        qpskSrcWorker->moveToThread(qpskSrcThread);

        connect(qpskSrcThread, SIGNAL(started()), qpskSrcWorker, SLOT(process()));
        connect(qpskSrcWorker, SIGNAL(finished()), this, SLOT(finishDemodulatorWorker()));
        connect(qpskSrcWorker, SIGNAL(chunkProcessed()), this, SLOT(updateBuffersIndicators()));
        connect(
                    qpskSrcWorker,
                    SIGNAL(demodInfo(bool,double,double,double,double)),
                    this,
                    SLOT(updateDemodStatusValues(bool,double,double,double,double)));
        connect(qpskSrcWorker, SIGNAL(iqWaterfall(QVector<int>)), WaterfallPlot, SLOT(drawWaterfall(QVector<int>)));

        /* Allocate new thread and worker for decoder */
        decoderThread = new QThread();
        decoderWorker = new DecoderWorker(decoder, decoderChunkSize, dediffcoder, DecoderInterleavedCB->isChecked());

        /* Move worker into separate thread and set up connections */
        decoderWorker->moveToThread(decoderThread);

        connect(decoderThread, SIGNAL(started()), decoderWorker, SLOT(process()));
        connect(decoderWorker, SIGNAL(finished()), this, SLOT(finishDecoderWorker()));
        connect(decoderWorker, SIGNAL(chunkProcessed()), this, SLOT(updateBuffersIndicators()));
        connect(
                    decoderWorker,
                    SIGNAL(decoderInfo(bool,int,int,int,int,int)),
                    this,
                    SLOT(updateDecoderStatusValues(bool,int,int,int,int,int)));
        connect(decoderWorker, SIGNAL(qpskConst(QVector<int>)), ConstellationPlot, SLOT(drawConst(QVector<int>)));
        connect(decoderWorker, SIGNAL(pxlsAvail(int,QVector<int>)), this, SLOT(renderImage(int,QVector<int>)));

        /* Start worker threads */
        /* TODO change cursor to busy */
        iqSrcThread->start();
        qpskSrcThread->start();
        decoderThread->start();
    }

    if ((srcMode == QPSK_FILE) && processing) {
        /* Convert QString to C string (with UTF-8 support) */
        QByteArray fileNameUTF8 = SrcFilePath->text().toUtf8();
        const char *fileNameCString = fileNameUTF8.data();

        /* Open QPSK file for reading */
        qpskSrcFile = lrpt_qpsk_file_open_r(fileNameCString, NULL); /* TODO error reporting */

        /* Initialize LRPT decoder */
        lrpt_decoder_spacecraft_t sc = DecoderSpacecraftCombB->currentData().value<lrpt_decoder_spacecraft_t>();

        LRPTChan64Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan65Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan66Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan67Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan68Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));
        LRPTChan69Widget->setStdWidth(lrpt_decoder_spacecraft_imgwidth(sc));

        decoder = lrpt_decoder_init(sc, NULL); /* TODO error reporting */

        /* Initialize dediffcoder (if requested) */
        if (DecoderDiffcodedCB->isChecked())
            dediffcoder = lrpt_dsp_dediffcoder_init(NULL); /* TODO error reporting */

        /* Allocate new thread and worker for QPSK file reading */
        qpskSrcThread = new QThread();
        qpskSrcWorker = new QPSKSourceFileWorker(qpskSrcFile, qpskSrcFileMTU);

        /* Move worker into separate thread and set up connections */
        qpskSrcWorker->moveToThread(qpskSrcThread);

        connect(qpskSrcThread, SIGNAL(started()), qpskSrcWorker, SLOT(process()));
        connect(qpskSrcWorker, SIGNAL(finished()), this, SLOT(finishSrcFileWorker()));
        connect(qpskSrcWorker, SIGNAL(chunkProcessed()), this, SLOT(updateBuffersIndicators()));
        connect(qpskSrcWorker, SIGNAL(readProgress(int)), this, SLOT(updateReadProgress(int)));

        /* Allocate new thread and worker for decoder */
        decoderThread = new QThread();
        decoderWorker = new DecoderWorker(decoder, decoderChunkSize, dediffcoder, DecoderInterleavedCB->isChecked());

        /* Move worker into separate thread and set up connections */
        decoderWorker->moveToThread(decoderThread);

        connect(decoderThread, SIGNAL(started()), decoderWorker, SLOT(process()));
        connect(decoderWorker, SIGNAL(finished()), this, SLOT(finishDecoderWorker()));
        connect(decoderWorker, SIGNAL(chunkProcessed()), this, SLOT(updateBuffersIndicators()));
        connect(
                    decoderWorker,
                    SIGNAL(decoderInfo(bool,int,int,int,int,int)),
                    this,
                    SLOT(updateDecoderStatusValues(bool,int,int,int,int,int)));
        connect(decoderWorker, SIGNAL(qpskConst(QVector<int>)), ConstellationPlot, SLOT(drawConst(QVector<int>)));
        connect(decoderWorker, SIGNAL(pxlsAvail(int,QVector<int>)), this, SLOT(renderImage(int,QVector<int>)));

        /* Start worker thread */
        /* TODO change cursor to busy */
        qpskSrcThread->start();
        decoderThread->start();
    }

    /* Reflect changes in UI */
    updateUI();
}

/**************************************************************************************************/

void MainWindow::updateReadProgress(int len) {
    TotalProgressBar->setValue(len);
}

/**************************************************************************************************/

void MainWindow::updateBuffersIndicators() {
    if ((srcMode == IQ_FILE) || (srcMode == SDR_RECEIVER))
        IQBufferUtilBar->setValue(iqRBUsed->available());

    QPSKBufferUtilBar->setValue(qpskRBUsed->available());
}

/**************************************************************************************************/

void MainWindow::updateDemodStatusValues(
        bool pllState,
        double pllFreq,
        double pllPhaseErr,
        double alcGain,
        double sigLvl) {
    PLLStatusLbl->setText(tr("PLL: <span style=\"font-weight: bold; color: %1\">%2</span>").
                          arg(
                              ((pllState) ? "green" : "red"),
                              ((pllState) ? tr("locked") : tr("unlocked"))));
    PLLFreqLbl->setText(tr("PLL frequency: %1 Hz").arg(QString::number(pllFreq, 'f', 1)));

    PLLPhaseErrLbl->setText(tr("PLL phase error: %1").arg(QString::number(pllPhaseErr, 'f', 3)));

    double x;

    if (pllPhaseErr < 0.5)
        x = 0.5;
    else if (pllPhaseErr > 1.0)
        x = 1.0;
    else x = pllPhaseErr;

    /* Display range is 0.5 (100%) -- 1.0 (0%) */
    LockQualityBar->setValue(100 * (2.0 - x / 0.5));

    ALCGainLbl->setText(tr("ALC gain: %1 dB").arg(QString::number(alcGain, 'f', 1)));
    SignalLevelLbl->setText(tr("Signal level: %1").arg(QString::number(sigLvl, 'f', 0)));
}

/**************************************************************************************************/

void MainWindow::updateDecoderStatusValues(bool frmState,
        int frmTotCnt,
        int frmOkCnt,
        int cvcduCnt,
        int pckCnt,
        int sigQ) {
    FramingStatusLbl->setText(tr("Framing: <span style=\"font-weight: bold; color: %1\">%2</span>").
                              arg(
                                  ((frmState) ? "green" : "red"),
                                  ((frmState) ? tr("ok") : tr("fail"))));
    FramesLbl->setText(tr("Frames: %1/%2 (%3%)").
                       arg(QString::number(frmOkCnt),
                           QString::number(frmTotCnt),
                           QString::number((frmTotCnt == 0) ? 0 : static_cast<int>(100.0 * frmOkCnt / frmTotCnt))));
    CVCDUsLbl->setText(tr("CVCDUs: %1").arg(QString::number(cvcduCnt)));
    PacketsLbl->setText(tr("Packets: %1").arg(QString::number(pckCnt)));
    SignalQualityBar->setValue(sigQ);
}

/**************************************************************************************************/

void MainWindow::renderImage(int apid, QVector<int> pxls) {
    switch (apid) {
        case 64: {
            LRPTChan64Widget->renderPixels(pxls);
            break;
        }

        case 65: {
            LRPTChan65Widget->renderPixels(pxls);
            break;
        }

        case 66: {
            LRPTChan66Widget->renderPixels(pxls);
            break;
        }

        case 67: {
            LRPTChan67Widget->renderPixels(pxls);
            break;
        }

        case 68: {
            LRPTChan68Widget->renderPixels(pxls);
            break;
        }

        case 69: {
            LRPTChan69Widget->renderPixels(pxls);
            break;
        }

        default:
            break;
    }
}

/**************************************************************************************************/

void MainWindow::finishSrcFileWorker() {
    if (srcMode == IQ_FILE) {
        /* Ask worker thread for stop and wait for it */
        iqSrcThread->quit();
        iqSrcThread->wait();

        /* Delete both thread and worker and explicitly NULLify their pointers */
        delete iqSrcThread;
        delete iqSrcWorker;

        iqSrcThread = nullptr;
        iqSrcWorker = nullptr;

        /* Close I/Q source file */
        lrpt_iq_file_close(iqSrcFile);
        iqSrcFile = NULL;

        /* Ask demodulator to stop */
        qpskSrcThread->requestInterruption();
    }
    else if (srcMode == QPSK_FILE) {
        /* Ask worker thread for stop and wait for it */
        qpskSrcThread->quit();
        qpskSrcThread->wait();

        /* Delete both thread and worker and explicitly NULLify their pointers */
        delete qpskSrcThread;
        delete qpskSrcWorker;

        qpskSrcThread = nullptr;
        qpskSrcWorker = nullptr;

        /* Close QPSK source file */
        lrpt_qpsk_file_close(qpskSrcFile);
        qpskSrcFile = NULL;

        /* Ask decoder to stop */
        decoderThread->requestInterruption();
    }
}

/**************************************************************************************************/

void MainWindow::finishDemodulatorWorker() {
    /* Ask worker thread for stop and wait for it */
    qpskSrcThread->quit();
    qpskSrcThread->wait();

    /* Delete both thread and worker and explicitly NULLify their pointers */
    delete qpskSrcThread;
    delete qpskSrcWorker;

    qpskSrcThread = nullptr;
    qpskSrcWorker = nullptr;

    /* Free demodulator object */
    lrpt_demodulator_deinit(demodulator);
    demodulator = NULL;

    /* Free Chebyshev filter object */
    lrpt_dsp_filter_deinit(chebFilter);
    chebFilter = NULL;

    /* Close intermediate dump files */
    lrpt_qpsk_file_close(ddDemodFile);
    ddDemodFile = NULL;

    /* Request decoder to stop */
    decoderThread->requestInterruption();
}

/**************************************************************************************************/

void MainWindow::finishDecoderWorker() {
    /* Ask worker thread for stop and wait for it */
    decoderThread->quit();
    decoderThread->wait();

    /* Delete both thread and worker and explicitly NULLify their pointers */
    delete decoderThread;
    delete decoderWorker;

    decoderThread = nullptr;
    decoderWorker = nullptr;

    /* Free decoder object */
    lrpt_decoder_deinit(decoder);
    decoder = NULL;

    /* Free dediffcoder object */
    lrpt_dsp_dediffcoder_deinit(dediffcoder);
    dediffcoder = NULL;

    /* TODO close intermediate dump files */

    setGlobalObjects(srcMode);

    IQBufferUtilBar->setValue(0);
    QPSKBufferUtilBar->setValue(0);
    LockQualityBar->setValue(0);
    SignalQualityBar->setValue(0);

    WaterfallPlot->clearWaterfall();
    ConstellationPlot->clearConst();

    PLLStatusLbl->setText(tr("PLL: ---"));
    PLLFreqLbl->setText(tr("PLL frequency: --- Hz"));
    PLLPhaseErrLbl->setText(tr("PLL phase error: ---"));
    ALCGainLbl->setText(tr("ALC gain: --- dB"));
    SignalLevelLbl->setText(tr("Signal level: ---"));
    FramingStatusLbl->setText(tr("Framing: ---"));
    FramesLbl->setText(tr("Frames: ---/--- (---%)"));
    CVCDUsLbl->setText(tr("CVCDUs: ---"));
    PacketsLbl->setText(tr("Packets: ---"));

    startStopProcessing();
}
