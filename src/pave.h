#ifndef _GST_PAVE2PARSE_PAVE_H_
#define _GST_PAVE2PARSE_PAVE_H_

#include <glib.h>

struct _PaVEHeader
{
	guint8		signature[4];
	guint8		version;
	guint8		video_codec;
	guint16		header_size;
	guint32		payload_size;
	guint16		encoded_stream_width;
	guint16		encoded_stream_height;
	guint16		display_width;
	guint16		display_height;
	guint32		frame_number;
	guint32		timestamp;
	guint8		total_chuncks;
	guint8		chunck_index ;
	guint8		frame_type;
	guint8		control;
	guint32		stream_byte_position_lw;
	guint32		stream_byte_position_uw;
	guint16		stream_id;
	guint8		total_slices;
	guint8		slice_index ;
	guint8		header1_size;
	guint8		header2_size;
	guint8		__1[2];
	guint32		advertised_size;
	guint8		__2[12];
} __attribute__((packed));

#endif