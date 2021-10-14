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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/**************************************************************************************************/

#include "ui_MainWindow.h"

#include "../workers/DecoderWorker.h"
#include "../workers/IQSourceAbstractWorker.h"
#include "../workers/QPSKSourceAbstractWorker.h"

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
    QString lastRcvDumpFileDir; /* Last directory of received dump */
    QString lastFiltDumpFileDir; /* Last directory of filtered dump */
    QString lastDemodDumpFileDir; /* Last directory of demodulated dump */
    QString lastQPSKProcDumpFileDir; /* Last directory of processed dump */

    int iqSrcFileMTU; /* MTU for reading I/Q file */
    int qpskSrcFileMTU; /* MTU for reading QPSK file */

    int iqRBSize; /* Ring buffer size for I/Q data */
    int qpskRBSize; /* Ring buffer size for QPSK data */

    int demodChunkSize; /* MTU for demodulator */

    int decoderChunkSize; /* MTU for decoder */

    /* Workers and core objects */
    IQSourceAbstractWorker *iqSrcWorker = nullptr;
    QThread *iqSrcThread = nullptr;
    lrpt_iq_file_t *iqSrcFile = NULL;
    lrpt_iq_file_t *ddRcvFile = NULL;

    QPSKSourceAbstractWorker *qpskSrcWorker = nullptr;
    QThread *qpskSrcThread = nullptr;
    lrpt_qpsk_file_t *qpskSrcFile = NULL;
    lrpt_demodulator_t *demodulator = NULL;
    lrpt_dsp_filter_t *chebFilter = NULL;
    lrpt_iq_file_t *ddFiltFile = NULL;
    lrpt_qpsk_file_t *ddDemodFile = NULL;

    DecoderWorker *decoderWorker = nullptr;
    QThread *decoderThread = nullptr;
    lrpt_decoder_t *decoder = NULL;
    lrpt_image_t *lrptImage = NULL;
    lrpt_qpsk_file_t *ddQPSKProcFile = NULL;

    /* State flags and variables */
    SrcType srcMode = NO_SRC;
    bool processing = false;
    int nPacketsGood = 0;
    int nPacketsTotal = 0;

    /* Status bar labels */
    QLabel *PLLStatusLbl;
    QLabel *PLLFreqLbl;
    QLabel *PLLPhaseErrLbl;
    QLabel *ALCGainLbl;
    QLabel *SignalLevelLbl;
    QLabel *FramingStatusLbl;
    QLabel *FramesLbl;
    QLabel *CVCDUsLbl;
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

    /* Handle enabled/disabled state for data dump items */
    void setDDItems();

    /* Select paths for dump files */
    void browseRcvDumpFile();
    void browseFiltDumpFile();
    void browseDemodDumpFile();
    void browseQPSKProcDumpFile();

    /* Set live LRPT imagery area */
    void setLiveAPIDsImagery();

    /* Handle start/stop processing event */
    void startStopProcessing();

    /* Show buffers utilization status */
    void updateBuffersIndicators();

    /* Show demodulation params during processing */
    void updateDemodStatusValues(
            bool pllState,
            double pllFreq,
            double pllPhaseErr,
            double alcGain,
            double sigLvl);

    /* Show decoder params during processing */
    void updateDecoderStatusValues(
            bool frmState,
            int frmTotCnt,
            int frmOkCnt,
            int cvcduCnt,
            int pckCnt,
            int sigQ);

    /* Finish file processing */
    void finishSrcFileWorker();

    /* Finish demodulator */
    void finishDemodulatorWorker();

    /* Finish decoder */
    void finishDecoderWorker();

    void stub(int apid, QVector<int> pxls);
};

/**************************************************************************************************/

#endif
