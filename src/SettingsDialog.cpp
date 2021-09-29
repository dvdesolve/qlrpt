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

#include "SettingsDialog.h"

#include <QPushButton>
#include <QSettings>

/**************************************************************************************************/

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    /* Read settings from system storage */
    loadSettings();
}

/**************************************************************************************************/

void SettingsDialog::loadSettings(void) {
    QSettings s("desolve", "glrpt");
    int x;

    /* IQ source file MTU */
    x = s.value("IO/IQSrcFileMTU", IQSrcFileMTU_DEF).toInt();

    if ((x < 1) || (x > 1048576)) {
        x = IQSrcFileMTU_DEF;
        s.setValue("IO/IQSrcFileMTU", x);
        fixed = true;
    }

    IQSrcFileMTUSB->setValue(x);

    /* QPSK source file MTU */
    x = s.value("IO/QPSKSrcFileMTU", QPSKSrcFileMTU_DEF).toInt();

    if ((x < 1) || (x > 1048576)) {
        x = QPSKSrcFileMTU_DEF;
        s.setValue("IO/QPSKSrcFileMTU", x);
        fixed = true;
    }

    QPSKSrcFileMTUSB->setValue(x);

    /* IQ ring buffer size factor */
    x = s.value("IO/IQRBFactor", IQRBFactor_DEF).toInt();

    if ((x < 1) || (x > 100)) {
        x = IQRBFactor_DEF;
        s.setValue("IO/IQRBFactor", x);
        fixed = true;
    }

    IQRBFactorSB->setValue(x);

    /* QPSK ring buffer size factor */
    x = s.value("IO/QPSKRBFactor", QPSKRBFactor_DEF).toInt();

    if ((x < 1) || (x > 100)) {
        x = QPSKRBFactor_DEF;
        s.setValue("IO/QPSKRBFactor", x);
        fixed = true;
    }

    QPSKRBFactorSB->setValue(x);

    /* Demodulator MTU */
    DemodMTUAsIQSrcRB->setChecked(s.value("IO/DemodMTUAsIQSrc", true).toBool());

    x = s.value("IO/DemodMTU", DemodMTU_DEF).toInt();

    if ((x < 1) || (x > 1048576)) {
        x = DemodMTU_DEF;
        s.setValue("IO/DemodMTU", x);
        fixed = true;
    }

    DemodMTUSB->setValue(x);

    /* Decoder SFL factor */
    x = s.value("IO/DecoderSFLFactor", DecoderSFLFactor_DEF).toInt();

    if ((x < 3) || (x > 100)) {
        x = DecoderSFLFactor_DEF;
        s.setValue("IO/DecoderSFLFactor", x);
        fixed = true;
    }

    DecoderSFLFactorSB->setValue(x);

    s.sync();
}

/**************************************************************************************************/

void SettingsDialog::saveSettings(void) {
    QSettings s("desolve", "glrpt");

    s.setValue("IO/IQSrcFileMTU", IQSrcFileMTUSB->value());
    s.setValue("IO/QPSKSrcFileMTU", QPSKSrcFileMTUSB->value());
    s.setValue("IO/IQRBFactor", IQRBFactorSB->value());
    s.setValue("IO/QPSKRBFactor", QPSKRBFactorSB->value());
    s.setValue("IO/DemodMTUAsIQSrc", DemodMTUAsIQSrcRB->isChecked());
    s.setValue("IO/DecoderSFLFactor", DecoderSFLFactorSB->value());

    s.sync();
}

/**************************************************************************************************/

void SettingsDialog::accept() {
    saveSettings();
    QDialog::accept();
}

/**************************************************************************************************/

void SettingsDialog::reject() {
    if (fixed)
        QDialog::accept();
    else
        QDialog::reject();
}

/**************************************************************************************************/

void SettingsDialog::restoreDefaults(void) {
    IQSrcFileMTUSB->setValue(IQSrcFileMTU_DEF);
    QPSKSrcFileMTUSB->setValue(IQSrcFileMTU_DEF);

    IQRBFactorSB->setValue(IQRBFactor_DEF);
    QPSKRBFactorSB->setValue(QPSKRBFactor_DEF);

    DemodMTUAsIQSrcRB->setChecked(true);
    DemodMTUSB->setValue(DemodMTU_DEF);

    DecoderSFLFactorSB->setValue(DecoderSFLFactor_DEF);
}
