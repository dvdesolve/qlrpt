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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

/**************************************************************************************************/

#include "ui_SettingsDialog.h"

/**************************************************************************************************/

#define IQSrcFileMTU_DEF        131072
#define QPSKSrcFileMTU_DEF      131072

#define IQRBFactor_DEF          10
#define QPSKRBFactor_DEF        10

#define DemodMTU_DEF            131072

#define DecoderSFLFactor_DEF    3

/**************************************************************************************************/

class SettingsDialog : public QDialog, private Ui::SettingsDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private:
    bool fixed = false;

    /* Load settings from system storage */
    void loadSettings(void);

    /* Save settings to system storage */
    void saveSettings(void);

private slots:
    /* Save settings upon exit */
    void accept(void);

    /* Exit and signal about changes if something was fixed */
    void reject(void);

    /* Reset settings to the compiled defaults */
    void restoreDefaults(void);
};

/**************************************************************************************************/

#endif
