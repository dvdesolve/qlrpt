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

#include "SettingsDialog.h"

#include "../qlrpt/GlobalDecls.h"

#include <QPushButton>
#include <QSettings>

/**************************************************************************************************/

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setupUi(this);

    /* Dialog button box connections */
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));

    /* Some UI interactions */
    connect(DemodChunkSizeCustomRB, SIGNAL(toggled(bool)), DemodChunkSizeSB, SLOT(setEnabled(bool)));

    /* TODO check if MTUs are lesser than ring buffers */

    /* Read settings from system storage */
    loadSettings();
}

/**************************************************************************************************/

void SettingsDialog::loadSettings() {
    QSettings s("desolve", "qlrpt");
    int x;

    /* IQ source file MTU */
    x = s.value("IO/IQSrcFileMTU", IQSrcFileMTU_DEF).toInt();

    if ((x < IQSrcFileMTU_MIN) || (x > IQSrcFileMTU_MAX)) {
        x = IQSrcFileMTU_DEF;
        s.setValue("IO/IQSrcFileMTU", x);
        fixed = true;
    }

    IQSrcFileMTUSB->setValue(x);

    /* QPSK source file MTU */
    x = s.value("IO/QPSKSrcFileMTU", QPSKSrcFileMTU_DEF).toInt();

    if ((x < QPSKSrcFileMTU_MIN) || (x > QPSKSrcFileMTU_MAX)) {
        x = QPSKSrcFileMTU_DEF;
        s.setValue("IO/QPSKSrcFileMTU", x);
        fixed = true;
    }

    QPSKSrcFileMTUSB->setValue(x);

    /* IQ ring buffer size */
    x = s.value("IO/IQRBSize", IQRBSize_DEF).toInt();

    if ((x < IQRBSize_MIN) || (x > IQRBSize_MAX)) {
        x = IQRBSize_DEF;
        s.setValue("IO/IQRBSize", x);
        fixed = true;
    }

    IQRBSizeSB->setValue(x);

    /* QPSK ring buffer size */
    x = s.value("IO/QPSKRBSize", QPSKRBSize_DEF).toInt();

    if ((x < QPSKRBSize_MIN) || (x > QPSKRBSize_MAX)) {
        x = QPSKRBSize_DEF;
        s.setValue("IO/QPSKRBSize", x);
        fixed = true;
    }

    QPSKRBSizeSB->setValue(x);

    /* Demodulator chunk size */
    x = s.value("IO/DemodChunkSize", DemodChunkSize_DEF).toInt();

    if (x == 0) {
        DemodChunkSizeDefaultRB->setChecked(true);
        DemodChunkSizeSB->setValue(DemodChunkSize_DEFINIT);
    }
    else {
        if ((x < DemodChunkSize_MIN) || (x > DemodChunkSize_MAX)) {
            x = DemodChunkSize_DEF;
            s.setValue("IO/DemodChunkSize", x);
            fixed = true;
        }

        DemodChunkSizeCustomRB->setChecked(true);
        DemodChunkSizeSB->setValue(x);
    }

    /* Decoder chunk size */
    x = s.value("IO/DecoderChunkSize", DecoderChunkSize_DEF).toInt();

    if ((x < DecoderChunkSize_MIN) || (x > DecoderChunkSize_MAX)) {
        x = DecoderChunkSize_DEF;
        s.setValue("IO/DecoderChunkSize", x);
        fixed = true;
    }

    DecoderChunkSizeSB->setValue(x);

    s.sync();
}

/**************************************************************************************************/

void SettingsDialog::saveSettings() {
    QSettings s("desolve", "qlrpt");

    s.setValue("IO/IQSrcFileMTU", IQSrcFileMTUSB->value());
    s.setValue("IO/QPSKSrcFileMTU", QPSKSrcFileMTUSB->value());

    s.setValue("IO/IQRBSize", IQRBSizeSB->value());
    s.setValue("IO/QPSKRBSize", QPSKRBSizeSB->value());

    s.setValue("IO/DemodChunkSize", (DemodChunkSizeDefaultRB->isChecked()) ?
                   0 :
                   DemodChunkSizeSB->value());

    s.setValue("IO/DecoderChunkSize", DecoderChunkSizeSB->value());

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

void SettingsDialog::restoreDefaults() {
    IQSrcFileMTUSB->setValue(IQSrcFileMTU_DEF);
    QPSKSrcFileMTUSB->setValue(QPSKSrcFileMTU_DEF);

    IQRBSizeSB->setValue(IQRBSize_DEF);
    QPSKRBSizeSB->setValue(QPSKRBSize_DEF);

    DemodChunkSizeDefaultRB->setChecked(true);
    DemodChunkSizeSB->setValue(DemodChunkSize_DEFINIT);

    DecoderChunkSizeSB->setValue(DecoderChunkSize_DEF);
}
