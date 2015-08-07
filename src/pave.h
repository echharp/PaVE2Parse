#ifndef _GST_PAVE2PARSE_PAVE_H_
#define _GST_PAVE2PARSE_PAVE_H_

#include <glib.h>

typedef enum _PaVEVideoCodec
{
	PAVE_VIDEO_CODEC_UNKNOWN = 0,
	PAVE_VIDEO_CODEC_VLIB = 1,
	PAVE_VIDEO_CODEC_P264 = 2,
	PAVE_VIDEO_CODEC_MPEG4_VISUAL = 3,
	PAVE_VIDEO_CODEC_MPEG4_AVC = 4,
	PAVE_VIDEO_CODEC_COUNT = 5
}PaVEVideoCodec;

typedef enum _PaVEFrametype
{
	PAVE_FRAME_TYPE_UNKNOWN = 0,
	PAVE_FRAME_TYPE_IDR = 1,
	PAVE_FRAME_TYPE_I = 2,
	PAVE_FRAME_TYPE_P = 3,
	PAVE_FRAME_TYPE_HEADERS = 4,
	PAVE_FRAME_TYPE_COUNT = 5
}PaVEFrameType;

typedef enum _PaVEHeaderControl
{
	PAVE_HEADER_CONTROL_DATA = 0,
	PAVE_HEADER_CONTROL_ADVERTISEMENT = 1,
	PAVE_HEADER_CONTROL_LAST_FRAME = 2,
	PAVE_HEADER_CONTROL_COUNT = 3
}PaVEHeaderControl;

typedef enum _PaVEStreamId
{
	PAVE_STREAM_ID_MP4_360p = 0,
	PAVE_STREAM_ID_H264_360p = 1,
	PAVE_STREAM_ID_H264_720p = 2,
	PAVE_STREAM_ID_COUNT = 3
}PaVEStreamId;

typedef struct _PaVEHeader
{
	guint32		signature;
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
}PaVEHeader __attribute__((packed));

#define PAVE_HEADER_SIGNATURE_INT32 (0x45566150)

#endif