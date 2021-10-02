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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**************************************************************************************************/

#include "ui_MainWindow.h"

#include "IQSourceAbstractWorker.h"
#include "QPSKSourceAbstractWorker.h"

#include <QMainWindow>
#include <QThread>

#include <lrpt.h>

/**************************************************************************************************/

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    /* Data source type */
    enum SrcType {
        NO_SRC,
        IQ_FILE,
        QPSK_FILE,
        SDR_RECEIVER
    };

    /* Common settings */
    QString lastSrcFileDir; /* Last directory of source file */

    int iqSrcFileMTU; /* MTU for reading I/Q file */
    int qpskSrcFileMTU; /* MTU for reading QPSK file */

    int iqRBSize; /* Ring buffer size for I/Q data */
    int qpskRBSize; /* Ring buffer size for QPSK data */

    int demodChunkSize; /* MTU for demodulator */

    int decoderChunkSize; /* MTU for decoder */

    /* State flags and variables */
    IQSourceAbstractWorker *iqSrcWorker = nullptr;
    QThread *iqSrcThread = nullptr;
    lrpt_iq_file_t *iqSrcFile = NULL;

    QPSKSourceAbstractWorker *qpskSrcWorker = nullptr;
    QThread *qpskSrcThread = nullptr;
    lrpt_qpsk_file_t *qpskSrcFile = NULL;

    QThread *decoderThread = nullptr;

    SrcType srcMode = NO_SRC;
    bool processing = false;

    /* Processing data values */
    bool pllStatus = false;
    double pllFreq = 0; /* In Hz */
    double pllPhaseErr = 0;
    double alcGain = 0; /* In dB */
    int sigQ = 0; /* In percents */
    int sigLvl = 0; /* In units of samples amplitude */
    bool framingStatus = false;
    int nPacketsGood = 0;
    int nPacketsTotal = 0;

    /* Status bar labels */
    QLabel *PLLStatusLbl;
    QLabel *PLLFreqLbl;
    QLabel *PLLPhaseErrLbl;
    QLabel *ALCGainLbl;
    QLabel *SignalLevelLbl;
    QLabel *FramingStatusLbl;
    QLabel *PacketsLbl;

    /* Read in settings from system storage */
    void restoreSettings();

    /* (Re)allocate global objects */
    void setGlobalObjects(SrcType src);

    /* Update GUI elements upon selecting source/processing */
    void updateUI();

private slots:
    /* Exit application */
    void exitApp();

    /* Show info about application */
    void aboutApp();

    /* Run "Settings" dialog */
    void openSettingsDlg();

    /* Handle source change */
    void setNewSource(int src);

    /* Select source file */
    void browseSrcFile();

    /* Show info about selected file */
    void showFileInfo(const QString &fileName);

    /* Set live LRPT imagery area */
    void setLiveAPIDsImagery();

    /* Handle start/stop processing event */
    void startStopProcessing();

    /* Show buffer utilization status */
    void updateBufferIndicators();

    /* Finish file processing */
    void finishSrcFileWorker();

    /* Finish demodulator */
    void finishDemodulatorWorker();
};

/**************************************************************************************************/

#endif
