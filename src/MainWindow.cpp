/*
 * This file is part of glrpt.
 *
 * glrpt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * glrpt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with glrpt. If not, see https://www.gnu.org/licenses/
 *
 * Author: Viktor Drobot
 */

/**************************************************************************************************/

#include "MainWindow.h"

#include "GlobalDecls.h"
#include "GlobalObjects.h"
#include "IQProcessorWorker.h"
#include "IQSourceFileWorker.h"
#include "SettingsDialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    /* Read stored settings */
    restoreSettings();

    /* TODO scan for SoapySDR sources and add them to the sources list */

    SrcCombB->insertSeparator(SrcCombB->count());

    /* TODO just for debug, bogus SDR source */
    SrcCombB->addItem(tr("SDR receiver"));

    /* TODO Manage source selection properly */
    connect(SrcCombB, SIGNAL(currentIndexChanged(int)), this, SLOT(setNewSource(int)));

    /* Connections for menu entries */
    connect(SettingsAct, SIGNAL(triggered()), this, SLOT(openSettingsDlg()));
    connect(ExitAct, SIGNAL(triggered()), this, SLOT(exitApp()));
    connect(AboutAct, SIGNAL(triggered()), this, SLOT(aboutApp()));

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

void MainWindow::restoreSettings(void) {
    QSettings s("desolve", "glrpt");

    lastSrcFileDir = s.value(
                "Paths/LastSrcDir",
                QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();

    int x;

    /* IQ source file MTU */
    x = s.value("IO/IQSrcFileMTU", IQSrcFileMTU_DEF).toInt();

    if ((x < IQSrcFileMTU_MIN) || (x > IQSrcFileMTU_MAX)) {
        x = IQSrcFileMTU_DEF;
        s.setValue("IO/IQSrcFileMTU", x);
    }

    iqSrcFileMTU = x;

    /* QPSK source file MTU */
    x = s.value("IO/QPSKSrcFileMTU", QPSKSrcFileMTU_DEF).toInt();

    if ((x < QPSKSrcFileMTU_MIN) || (x > QPSKSrcFileMTU_MAX)) {
        x = QPSKSrcFileMTU_DEF;
        s.setValue("IO/QPSKSrcFileMTU", x);
    }

    qpskSrcFileMTU = x;

    /* IQ ring buffer size */
    x = s.value("IO/IQRBSize", IQRBSize_DEF).toInt();

    if ((x < IQRBSize_MIN) || (x > IQRBSize_MAX)) {
        x = IQRBSize_DEF;
        s.setValue("IO/IQRBSize", x);
    }

    iqRBSize = x;

    /* QPSK ring buffer size */
    x = s.value("IO/QPSKRBSize", QPSKRBSize_DEF).toInt();

    if ((x < QPSKRBSize_MIN) || (x > QPSKRBSize_MAX)) {
        x = QPSKRBSize_DEF;
        s.setValue("IO/QPSKRBSize", x);
    }

    qpskRBSize = x;

    /* Demodulator chunk size */
    x = s.value("IO/DemodChunkSize", DemodChunkSize_DEF).toInt();

    if ((x != 0) && ((x < DemodChunkSize_MIN) || (x > DemodChunkSize_MAX))) {
        x = DemodChunkSize_DEF;
        s.setValue("IO/DemodChunkSize", x);
    }

    demodChunkSize = x;

    /* Decoder chunk size */
    x = s.value("IO/DecoderChunkSize", DecoderChunkSize_DEF).toInt();

    if ((x < DecoderChunkSize_MIN) || (x > DecoderChunkSize_MAX)) {
        x = DecoderChunkSize_DEF;
        s.setValue("IO/DecoderChunkSize", x);
    }

    decoderChunkSize = x;

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
                QMessageBox::critical(this, tr("glrpt error"), tr("Can't allocate I/Q data ring buffer object!"), QMessageBox::Close);

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
            QMessageBox::critical(this, tr("glrpt error"), tr("Can't allocate QPSK data ring buffer object!"), QMessageBox::Close);

            /* Deinit all previously allocated objects */
            setGlobalObjects(NO_SRC);

            return;
        }

        qpskRBUsed = new QSemaphore(0);
        qpskRBFree = new QSemaphore(qpskRBSize);
    }
}

/**************************************************************************************************/

void MainWindow::updateUIState(void) {
    switch (srcMode) {
        /* When no source is selected (e. g., at startup)*/
        case NO_SRC: {
            StartStopBtn->setDisabled(true);

            SrcLbl->setEnabled(true);
            SrcCombB->setEnabled(true);
            SrcRescanBtn->setEnabled(true);

            SrcFilepathLbl->setDisabled(true);
            SrcFilepath->setDisabled(true);
            SrcFileBrowseBtn->setDisabled(true);

            SrcInfoText->setDisabled(true);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(true);
            DecoderTab->setDisabled(true);

            StatusGB->setDisabled(true);
            FFTPlotGB->setDisabled(true);
            QPSKGB->setDisabled(true);
            IQBufferUtilLbl->setDisabled(true);
            IQBufferUtilBar->setDisabled(true);
            QPSKBufferUtilLbl->setDisabled(true);
            QPSKBufferUtilBar->setDisabled(true);
            PhaseErrorLbl->setDisabled(true);
            PhaseErrorBar->setDisabled(true);
            SignalQualityLbl->setDisabled(true);
            SignalQualityBar->setDisabled(true);
            LRPTGB->setDisabled(true);

            PLLStatusLbl->setDisabled(true);
            PLLFreqLbl->setDisabled(true);
            PLLPhaseErrLbl->setDisabled(true);
            ALCGainLbl->setDisabled(true);
            SignalLevelLbl->setDisabled(true);
            FramingStatusLbl->setDisabled(true);
            PacketsLbl->setDisabled(true);

            break;
        }

        case IQ_FILE: {
            StartStopBtn->setDisabled(SrcFilepath->text().isEmpty());

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilepathLbl->setDisabled(processing);
            SrcFilepath->setDisabled(processing);
            SrcFileBrowseBtn->setDisabled(processing);

            SrcInfoText->setEnabled(true);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(processing);
            DecoderTab->setDisabled(processing);

            StatusGB->setEnabled(processing);
            FFTPlotGB->setEnabled(true);
            QPSKGB->setEnabled(true);
            IQBufferUtilLbl->setEnabled(true);
            IQBufferUtilBar->setEnabled(true);
            QPSKBufferUtilLbl->setEnabled(true);
            QPSKBufferUtilBar->setEnabled(true);
            PhaseErrorLbl->setEnabled(true);
            PhaseErrorBar->setEnabled(true);
            SignalQualityLbl->setEnabled(true);
            SignalQualityBar->setEnabled(true);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setEnabled(processing);
            PLLFreqLbl->setEnabled(processing);
            PLLPhaseErrLbl->setEnabled(processing);
            ALCGainLbl->setEnabled(processing);
            SignalLevelLbl->setEnabled(processing);
            FramingStatusLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }

        case QPSK_FILE: {
            StartStopBtn->setDisabled(SrcFilepath->text().isEmpty());

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilepathLbl->setDisabled(processing);
            SrcFilepath->setDisabled(processing);
            SrcFileBrowseBtn->setDisabled(processing);

            SrcInfoText->setEnabled(true);

            SDRTab->setDisabled(true);
            DemodTab->setDisabled(true);
            DecoderTab->setDisabled(processing);

            StatusGB->setEnabled(processing);
            FFTPlotGB->setDisabled(true);
            QPSKGB->setEnabled(true);
            IQBufferUtilLbl->setDisabled(true);
            IQBufferUtilBar->setDisabled(true);
            QPSKBufferUtilLbl->setEnabled(true);
            QPSKBufferUtilBar->setEnabled(true);
            PhaseErrorLbl->setDisabled(true);
            PhaseErrorBar->setDisabled(true);
            SignalQualityLbl->setEnabled(true);
            SignalQualityBar->setEnabled(true);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setDisabled(true);
            PLLFreqLbl->setDisabled(true);
            PLLPhaseErrLbl->setDisabled(true);
            ALCGainLbl->setDisabled(true);
            SignalLevelLbl->setDisabled(true);
            FramingStatusLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }

        case SDR_RECEIVER: { /* TODO need to control specific GUI elements (such as bias tee) depending on receiver's capabilities */
            StartStopBtn->setEnabled(true);

            SrcLbl->setDisabled(processing);
            SrcCombB->setDisabled(processing);
            SrcRescanBtn->setDisabled(processing);

            SrcFilepathLbl->setDisabled(true);
            SrcFilepath->setDisabled(true);
            SrcFileBrowseBtn->setDisabled(true);

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
            SDRGainSlider->setEnabled(true);

            DemodTab->setDisabled(processing);
            DecoderTab->setDisabled(processing);

            StatusGB->setEnabled(processing);
            FFTPlotGB->setEnabled(true);
            QPSKGB->setEnabled(true);
            IQBufferUtilLbl->setEnabled(true);
            IQBufferUtilBar->setEnabled(true);
            QPSKBufferUtilLbl->setEnabled(true);
            QPSKBufferUtilBar->setEnabled(true);
            PhaseErrorLbl->setEnabled(true);
            PhaseErrorBar->setEnabled(true);
            SignalQualityLbl->setEnabled(true);
            SignalQualityBar->setEnabled(true);
            LRPTGB->setEnabled(processing);

            PLLStatusLbl->setEnabled(processing);
            PLLFreqLbl->setEnabled(processing);
            PLLPhaseErrLbl->setEnabled(processing);
            ALCGainLbl->setEnabled(processing);
            SignalLevelLbl->setEnabled(processing);
            FramingStatusLbl->setEnabled(processing);
            PacketsLbl->setEnabled(processing);

            break;
        }
    }
}

/**************************************************************************************************/

void MainWindow::openSettingsDlg(void) {
    SettingsDialog d;

    if (d.exec() == QDialog::Accepted)
        restoreSettings();
}

/**************************************************************************************************/

void MainWindow::exitApp(void) {
    /* TODO handle running processing, unsaved data etc */
    QCoreApplication::quit();
}

/**************************************************************************************************/

void MainWindow::aboutApp(void) {
    QMessageBox::about(this, tr("About glrpt"), tr("Interactive GUI application for receiving, decoding and displaying LRPT images."));
}

/**************************************************************************************************/

void MainWindow::setNewSource(int src) {
    /* TODO implement SDR source selection */

    /* Clear previously filled data in source fields */
    SrcFilepath->clear();
    SrcInfoText->clear();

    if (src == -1) /* No source (default) */
        srcMode = NO_SRC;
    else if (src == 0) /* I/Q file */
        srcMode = IQ_FILE;
    else if (src == 1) /* QPSK file */
        srcMode = QPSK_FILE;
    else /* Real SDR source */
        srcMode = SDR_RECEIVER;
    /* TODO other sources (such as TCP socket) can be added in the future */

    /* Set global objects after source switching */
    setGlobalObjects(srcMode);

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

    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Select source file"),
                lastSrcFileDir,
                filter);

    if (!fileName.isEmpty()) {
        /* Update UI elements */
        SrcFilepath->setText(fileName);
        StartStopBtn->setEnabled(true);

        /* Error object for reporting */
        lrpt_error_t *err = lrpt_error_init();

        /* Convert QString to C string (with UTF-8 support) */
        QByteArray fileNameUTF8 = fileName.toUtf8();
        const char *fileNameCString = fileNameUTF8.data();

        switch (srcMode) {
            case IQ_FILE: {
                lrpt_iq_file_t *f = lrpt_iq_file_open_r(fileNameCString, err);

                if (!f) {
                    QMessageBox::critical(
                                this,
                                tr("glrpt error"),
                                tr("Error while opening I/Q data file %1:\n").arg(fileName) +
                                ((err) ? QString::fromUtf8(lrpt_error_message(err)) : ""),
                                QMessageBox::Close);

                    lrpt_error_deinit(err);

                    return;
                }

                SrcInfoText->setPlainText(
                            tr("Source type: I/Q file\n") +
                            tr("I/Q file version: ") + QString::number(lrpt_iq_file_version(f)).append('\n') +
                            tr("Flags: ") + ((lrpt_iq_file_is_offsetted(f)) ? tr("offsetted") : "").append('\n') +
                            tr("Sampling rate: ") + QString::number(lrpt_iq_file_samplerate(f)).append(" samples/s\n") +
                            tr("Device name: ") + QString::fromUtf8(lrpt_iq_file_devicename(f)).append('\n') +
                            tr("File size: ") + QString::number(lrpt_iq_file_length(f)).append(" samples")
                            );

                lrpt_iq_file_close(f);

                break;
            }

            case QPSK_FILE: {
                lrpt_qpsk_file_t *f = lrpt_qpsk_file_open_r(fileNameCString, err);

                if (!f) {
                    QMessageBox::critical(
                                this,
                                tr("glrpt error"),
                                tr("Error while opening QPSK data file %1:\n").arg(fileName) +
                                ((err) ? QString::fromUtf8(lrpt_error_message(err)) : ""),
                                QMessageBox::Close);

                    lrpt_error_deinit(err);

                    return;
                }

                SrcInfoText->setPlainText(
                            tr("Source type: QPSK file\n") +
                            tr("QPSK file version: ") + QString::number(lrpt_qpsk_file_version(f)).append('\n') +
                            tr("Flags: ") + ((lrpt_qpsk_file_is_diffcoded(f)) ? tr("diffcoded, ") : "") + ((lrpt_qpsk_file_is_interleaved(f)) ? tr("interleaved") : "") + '\n' +
                            tr("Symbol type: ") + ((lrpt_qpsk_file_is_hardsymboled(f)) ? tr("hard") : tr("soft")).append('\n') +
                            tr("Symbol rate: ") + QString::number(lrpt_qpsk_file_symrate(f)).append(" symbols/s\n") +
                            tr("File size: ") + QString::number(lrpt_qpsk_file_length(f)).append(" symbols")
                            );

                lrpt_qpsk_file_close(f);

                break;
            }

            default:
                break;
        }

        /* Free resources allocated by error object */
        lrpt_error_deinit(err);

        /* Store last directory */
        lastSrcFileDir = QFileInfo(fileName).absolutePath();

        QSettings s("desolve", "glrpt");

        s.setValue("Paths/LastSrcDir", lastSrcFileDir);
        s.sync();
    }
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
    processing = !processing;

    StartStopBtn->setText((processing) ? tr("Stop") : tr("Start"));
    SettingsAct->setDisabled(processing);

    if ((srcMode == IQ_FILE) && processing) {
        iqSrcThread = new QThread();
        iqSrcWorker = new IQSourceFileWorker(NULL, iqSrcFileMTU);

        iqSrcWorker->moveToThread(iqSrcThread);

//        connect(readerThread, SIGNAL(started()), readerWorker, SLOT(process()));
//        connect(readerWorker, SIGNAL(finished()), readerThread, SLOT(quit()));
//        connect(readerWorker, SIGNAL(finished()), readerWorker, SLOT(deleteLater()));
//        connect(readerThread, SIGNAL(finished()), readerThread, SLOT(deleteLater()));

        iqSrcThread->start();
    }

    /* Reflect changes in UI */
    updateUIState();
}
