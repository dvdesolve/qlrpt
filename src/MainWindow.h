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

#include <QMainWindow>

/**************************************************************************************************/

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    enum SrcType {
        NO_SRC,
        IQ_FILE,
        QPSK_FILE,
        SDR_RECEIVER
    };

    /* Common settings */
    QString lastSrcFileDir;

    int iqSrcFileMTU;
    int qpskSrcFileMTU;

    int iqRBSize;
    int qpskRBSize;

    int demodChunkSize;

    int decoderChunkSize;

    /* State flags */
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
    int iqBufferUtil = 0;
    int qpskBufferUtil = 0;

    /* Status bar labels */
    QLabel *PLLStatusLbl;
    QLabel *PLLFreqLbl;
    QLabel *PLLPhaseErrLbl;
    QLabel *ALCGainLbl;
    QLabel *SignalLevelLbl;
    QLabel *FramingStatusLbl;
    QLabel *PacketsLbl;

    /* Read in settings from system storage */
    void restoreSettings(void);

    /* (Re)allocate global objects */
    void setGlobalObjects(SrcType src);

    /* Manage GUI elements upon selecting source/processing */
    void updateUIState(void);

private slots:
    /* Open Settings dialog */
    void openSettingsDlg(void);

    /* Exit application */
    void exitApp(void);

    /* Show info about application */
    void aboutApp(void);

    /* Handle source change */
    void setNewSource(int src);

    /* Open source file */
    void browseSrcFile(void);

    /* Set live LRPT imagery area */
    void setLiveAPIDsImagery(void);

    /* Handle start/stop processing event */
    void startStopProcessing(void);
};

/**************************************************************************************************/

#endif
