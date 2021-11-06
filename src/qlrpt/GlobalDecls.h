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

#ifndef GLOBALDECLS_H
#define GLOBALDECLS_H

/**************************************************************************************************/

#define IQSrcFileMTU_DEF        128
#define IQSrcFileMTU_MIN        1
#define IQSrcFileMTU_MAX        102400

#define QPSKSrcFileMTU_DEF      128
#define QPSKSrcFileMTU_MIN      1
#define QPSKSrcFileMTU_MAX      102400

#define IQRBSize_DEF            1024
#define IQRBSize_MIN            1
#define IQRBSize_MAX            102400

#define QPSKRBSize_DEF          1024
#define QPSKRBSize_MIN          1
#define QPSKRBSize_MAX          102400

#define DemodChunkSize_DEF      0
#define DemodChunkSize_DEFINIT  128
#define DemodChunkSize_MIN      1
#define DemodChunkSize_MAX      102400

#define DecoderChunkSize_DEF    3
#define DecoderChunkSize_MIN    1
#define DecoderChunkSize_MAX    100

#define SDRFrequency_DEF        137100.0
#define SDRFrequency_MIN        130000.0
#define SDRFrequency_MAX        140000.0

#define SDRBandwidth_DEF        120.0
#define SDRBandwidth_MIN        90.0
#define SDRBandwidth_MAX        200.0

#define DemodFilterRipple_DEF    5.0
#define DemodFilterRipple_MIN    0.0
#define DemodFilterRipple_MAX    100.0

#define DemodFilterPoles_DEF     6
#define DemodFilterPoles_MIN     2
#define DemodFilterPoles_MAX     252

#define DemodPLLLocked_DEF       0.80
#define DemodPLLLocked_MIN       0.1
#define DemodPLLLocked_MAX       2.0

#define DemodPLLUnlocked_DEF     0.83
#define DemodPLLUnlocked_MIN     0.1
#define DemodPLLUnlocked_MAX     2.0

#define DemodRRCOrder_DEF       32
#define DemodRRCOrder_MIN       0
#define DemodRRCOrder_MAX       256

#define DemodRRCAlpha_DEF       0.6
#define DemodRRCAlpha_MIN       0.0
#define DemodRRCAlpha_MAX       1.0

#define DemodInterpFactor_DEF   4
#define DemodInterpFactor_MIN   1
#define DemodInterpFactor_MAX   32

/* TODO use custom settings */
#define QPSKConstPoints         512

/**************************************************************************************************/

#endif
