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

#include "GlobalObjects.h"
#include "MainWindow.h"

#include <QApplication>

/**************************************************************************************************/

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    int initRes = initGlobalObjects();

    if (initRes != 0) {
        deinitGlobalObjects();

        return initRes;
    }

    MainWindow w;
    w.show();

    deinitGlobalObjects();

    return a.exec();
}
