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
    enum SrcType {
        NONE,
        IQ_FILE,
        QPSK_FILE,
        SDR_RECEIVER
    };

    /* State flags */
    SrcType srcMode = NONE;
    bool processing = false;

    /* Processing data */
    bool PLLStatus = false;
    double PLLFreq = 0; /* In Hz */
    double PLLPhaseErr = 0;
    double ALCGain = 0; /* In dB */
    int SignalQ = 0; /* In percents */
    int SignalLvl = 0; /* In units of samples amplitude */
    bool FramingStatus = false;
    int PacketsGood = 0;
    int PacketsTotal = 0;

    /* Status bar labels */
    QLabel *PLLStatusLbl;
    QLabel *PLLFreqLbl;
    QLabel *ALCGainLbl;
    QLabel *FramingStatusLbl;
    QLabel *PacketsLbl;

    void updateUIState(void);

private slots:
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
