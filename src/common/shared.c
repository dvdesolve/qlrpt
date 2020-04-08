/*
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details:
 *
 *  http://www.gnu.org/copyleft/gpl.txt
 */

/*****************************************************************************/

#include "shared.h"

#include "../glrpt/rc_config.h"
#include "../lrpt_decode/huffman.h"
#include "../lrpt_decode/met_to_data.h"
#include "../sdr/filters.h"
#include "common.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <gtk/gtk.h>

#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>

/*****************************************************************************/

/* Runtime config data */
rc_data_t rc_data;

/* QPSK Constellation drawingarea pixbuf */
GdkPixbuf *qpsk_pixbuf = NULL;
guchar    *qpsk_pixels = NULL;
gint
  qpsk_rowstride,
  qpsk_n_channels,
  qpsk_width,
  qpsk_height,
  qpsk_center_x,
  qpsk_center_y;

/* Waterfall drawingarea pixbuf */
GdkPixbuf *wfall_pixbuf = NULL;
guchar    *wfall_pixels = NULL;
gint
  wfall_rowstride,
  wfall_n_channels,
  wfall_width,
  wfall_height;

/* Global widgets */
GtkWidget
  *qpsk_drawingarea   = NULL,   /* QPSK Constellation drawingarea */
  *ifft_drawingarea   = NULL,   /* IFFT Spectrum drawingarea */
  *main_window        = NULL,   /* glrpt's top window */
  *start_togglebutton = NULL,   /* Start Receive and Decode toggle button */
  *text_scroller      = NULL,   /* Text view scroller */
  *lrpt_image         = NULL,   /* Image to be displayed */
  *pll_lock_icon      = NULL,   /* PLL Lock indicator icon */
  *pll_ave_entry      = NULL,   /* PLL lock detect level */
  *pll_freq_entry     = NULL,   /* PLL frequency indicator */
  *sig_level_entry    = NULL,   /* Average Signal level in AGC */
  *agc_gain_entry     = NULL,   /* AGC gain level */
  *frame_icon         = NULL,   /* Frame Status indicator icon */
  *status_icon        = NULL,   /* Receiver Status indicator icon */
  *sig_quality_entry  = NULL,   /* Signal Quality as given by Packet Decoder */
  *packet_cnt_entry   = NULL,   /* OK and Total count of Packets */
  *ob_time_entry      = NULL,   /* On Board Time indicator */
  *sig_level_drawingarea = NULL,    /* Signal level drawingarea */
  *sig_qual_drawingarea  = NULL,    /* Signal quality drawingarea */
  *agc_gain_drawingarea  = NULL,    /* Agc gain drawingarea */
  *pll_ave_drawingarea   = NULL;    /* PLL average drawingarea */

GtkBuilder
  *decode_timer_dialog_builder = NULL,
  *auto_timer_dialog_builder   = NULL,
  *main_window_builder = NULL,
  *popup_menu_builder  = NULL;

/* Text buffer for text view */
GtkTextBuffer *text_buffer = NULL;

/* Pixbuf for scaled images display */
GdkPixbuf *scaled_image_pixbuf = NULL;

/* Pixbuf rowstride and num of channels */
gint
  scaled_image_width,
  scaled_image_height,
  scaled_image_rowstride,
  scaled_image_n_channels;

/* Pixel buffer for scaled images display */
guchar *scaled_image_pixel_buf;

/* Common between callbacks.c and callback_func.c */
GtkWidget
  *quit_dialog  = NULL,
  *error_dialog = NULL,
  *popup_menu   = NULL,
  *decode_timer_dialog = NULL,
  *auto_timer_dialog   = NULL;

/* IFFT data buffer*/
int16_t *ifft_data        = NULL;
uint16_t ifft_data_length = 0;

/* Chebyshev filter data I/Q */
filter_data_t filter_data_i;
filter_data_t filter_data_q;

/* Demodulation control semaphore */
sem_t demod_semaphore;

/* Meteor decoder variables */
ac_table_rec_t *ac_table = NULL;
size_t ac_table_len;
mtd_rec_t mtd_record;

/* Channel images and sizes */
uint8_t *channel_image[CHANNEL_IMAGE_NUM];
size_t   channel_image_size;
uint32_t channel_image_width, channel_image_height;
