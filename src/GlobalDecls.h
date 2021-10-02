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

/**************************************************************************************************/

#endif
