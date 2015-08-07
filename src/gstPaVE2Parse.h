#ifndef _GST_PAVE2PARSE_H_
#define _GST_PAVE2PARSE_H_

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_PAVE2PARSE   (gst_pave2parse_get_type())
#define GST_PAVE2PARSE(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_PAVE2PARSE,GstPaVE2Parse))
#define GST_PAVE2PARSE_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_PAVE2PARSE,GstPaVE2ParseClass))
#define GST_IS_PAVE2PARSE(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_PAVE2PARSE))
#define GST_IS_PAVE2PARSE_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_PAVE2PARSE))
typedef struct _GstPaVE2Parse GstPaVE2Parse;
typedef struct _GstPaVE2ParseClass GstPaVE2ParseClass;

struct _GstPaVE2Parse
{
	GstElement base_pave2parse;
	GstPad *sinkpad;
	GstPad *srcpad;

	GstBuffer* frameBuffer;
	gsize bytesLeft;
	gsize offset;
};

struct _GstPaVE2ParseClass
{
	GstElementClass base_pave2parse_class;
};

GType gst_pave2parse_get_type (void);

G_END_DECLS

#endif
