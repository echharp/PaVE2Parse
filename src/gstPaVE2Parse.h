/* GStreamer
 * Copyright (C) 2015 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_PAVE2PARSE_H_
#define _GST_PAVE2PARSE_H_

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_PAVE2PARSE   (gst_pave2parse_get_type())
#define GST_PAVE2PARSE(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PAVE2PARSE,GstPaVE2Parse))
#define GST_PAVE2PARSE_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PAVE2PARSE,GstPaVE2ParseClass))
#define GST_IS_PAVE2PARSE(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PAVE2PARSE))
#define GST_IS_PAVE2PARSE_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PAVE2PARSE))

typedef struct _PaVE2Header PaVE2Header;
typedef struct _GstPaVE2Parse GstPaVE2Parse;
typedef struct _GstPaVE2ParseClass GstPaVE2ParseClass;

typedef struct _PaVE2Header
{
	guint8	signature[4];			// 00
	guint8	version;				// 04
	guint8	video_codec;			// 05
	guint16	header_size; // = 64	// 06
	guint32	payload_size;			// 08
	guint16	encoded_stream_width;	// 12
	guint16	encoded_stream_height;	// 14
	guint16	display_width;			// 16
	guint16	display_height;			// 18
	guint32	frame_number;			// 20
	guint32	timestamp;				// 24
	guint8	total_chunks;			// 28
	guint8	chunk_index;			// 29
	guint8	frame_type;				// 30
	guint8	control;				// 31
	guint32	stream_byte_position_l;	// 32
	guint32	stream_byte_position_h;	// 36
	guint16	stream_id;				// 40
	guint8	total_slices;			// 42
	guint8	slice_index;			// 43
	guint8	header1_size;			// 44
	guint8	header2_size;			// 45
	guint8	__1[2];		// Padding	// 46
	guint32	advertised_size;		// 48
	guint8	__2[12];	// Padding	// 52
									// 64
} __attribute__ ((packed)) PaVE2Header;

struct _GstPaVE2Parse
{
  GstElement base_pave2parse;

  GstPad *sinkpad;
  GstPad *srcpad;
};

struct _GstPaVE2ParseClass
{
  GstElementClass base_pave2parse_class;
};

GType gst_pave2parse_get_type (void);

G_END_DECLS

#endif
