#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "1.5.2"
#define PACKAGE_NAME "pave2parse"
#endif
#define PACKAGE_URL "http://gstreamer.net"

#include <gst/gst.h>
#include "gstPaVE2Parse.h"
#include "pave.h"

GST_DEBUG_CATEGORY_STATIC(category_pave2parse);
#define GST_CAT_DEFAULT category_pave2parse

G_DEFINE_TYPE(GstPaVE2Parse, gst_pave2parse, GST_TYPE_ELEMENT);

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE("sink", GST_PAD_SINK, GST_PAD_ALWAYS, GST_STATIC_CAPS("ANY"));
static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE("src", GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS("ANY"));

static void gst_pave2parse_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* spec);
static void gst_pave2parse_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* spec);

static GstFlowReturn gst_pave2parse_chain(GstPad* pad, GstObject* parent, GstBuffer* buffer);
static gboolean gst_pave2parse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event);
static gboolean gst_pave2parse_src_event(GstPad* pad, GstObject* parent, GstEvent* event);

static void gst_pave2parse_class_init(GstPaVE2ParseClass* klass)
{
	GObjectClass* gobject_class = (GObjectClass*)klass;
	GstElementClass* gst_element_class = (GstElementClass*)klass;

	gobject_class->set_property = gst_pave2parse_set_property;
	gobject_class->get_property = gst_pave2parse_get_property;

	gst_element_class_set_static_metadata(gst_element_class, "pave2parse", "Parser/pave2", "Parses PaVE 2 stream into H264 or MP4", "Christian Harper-Cyr <christian.harper-cyr@ericsson.com>");

	gst_element_class_add_pad_template(gst_element_class, gst_static_pad_template_get(&sink_template));
	gst_element_class_add_pad_template(gst_element_class, gst_static_pad_template_get(&src_template));
}

static void gst_pave2parse_init(GstPaVE2Parse* parser)
{
	GObject* gobject = G_OBJECT(parser);
	GstElement* element = GST_ELEMENT(parser);

	parser->sinkpad = gst_pad_new_from_static_template(&sink_template, NULL);
	gst_pad_set_chain_function(parser->sinkpad, gst_pave2parse_chain);
	gst_pad_set_event_function(parser->sinkpad, gst_pave2parse_sink_event);
	gst_element_add_pad(GST_ELEMENT(parser), parser->sinkpad);
	parser->srcpad = gst_pad_new_from_static_template(&src_template, NULL);
	gst_pad_set_event_function(parser->sinkpad, gst_pave2parse_src_event);
	gst_element_add_pad(GST_ELEMENT(parser), parser->srcpad);

	parser->frameBuffer = NULL;
	parser->bytesLeft = 0;
	parser->offset = 0;
}

static void gst_pave2parse_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* spec)
{
	GstPaVE2Parse* parse = GST_PAVE2PARSE(object);
	switch(prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, spec);
		break;
	}
}

static void gst_pave2parse_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* spec)
{
	GstPaVE2Parse* parse = GST_PAVE2PARSE(object);
	switch(prop_id) {
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, spec);
		break;
	}
}

#define GST_PAVE2PARSE_CHAIN_EXIT(buffer, info, type, error) gst_buffer_unmap(buffer, &info); gst_buffer_unref(buffer); GST_##type(error); return GST_FLOW_ERROR
#define GST_PAVE2PARSE_CHAIN_ERROR(buffer, info, error) GST_PAVE2PARSE_CHAIN_EXIT(buffer, info, ERROR, error)
#define GST_PAVE2PARSE_CHAIN_WARN(buffer, info, error) GST_PAVE2PARSE_CHAIN_EXIT(buffer, info, WARNING, error)

static GstFlowReturn gst_pave2parse_chain(GstPad* pad, GstObject* parent, GstBuffer* buffer)
{
	GstPaVE2Parse* parse = GST_PAVE2PARSE(parent);
	if(pad != parse->sinkpad)
	{
		GST_ERROR("Pad is invalid");
		return GST_FLOW_ERROR;
	}

	GstMapInfo info;
	if(!gst_buffer_map(buffer, &info, GST_MAP_READ))
	{
		GST_ERROR("Could not map buffer");
		return GST_FLOW_ERROR;
	}
	// If frameBuffer is NULL means that we are waiting for a header, else its a frame
	if(parse->frameBuffer == NULL)
	{
		// Check the size
		if(info.size < sizeof(PaVEHeader))
		{
			GST_PAVE2PARSE_CHAIN_ERROR(buffer, info, "Buffer is too small to be a header")
		}
		PaVEHeader* header = (PaVEHeader*)info.data;
		// Check the signature
		if(header.signature != PAVE_HEADER_SIGNATURE_INT32 || header->header_size != sizeof(PaVEHeader))
 		{
			GST_PAVE2PARSE_CHAIN_ERROR(buffer, info, "PaVE Header is invalid, will be ignored");
		}
		parse->frameBuffer = gst_buffer_new_allocate(NULL, header->payload_size, NULL);
		parse->bytesLeft = header->payload_size;

		// TODO Set Caps

		if(header->)

gst_pave2parse_chain_exit:
	gst_buffer_unmap(buffer, &info);
	gst_buffer_unref(buffer);
}

#undef GST_PAVE2PARSE_CHAIN_ERROR

static gboolean gst_pave2parse_sink_event(GstPad* pad, GstObject* parent, GstEvent* event)
{
	return TRUE;
}

static gboolean gst_pave2parse_src_event(GstPad* pad, GstObject* parent, GstEvent* event)
{
	return TRUE;
}


static gboolean pave2parse_plugin_init(GstPlugin* plugin)
{
	GST_DEBUG_CATEGORY_INIT(category_pave2parse, "pave2parse", 0, "pave2parse");
	return gst_element_register(plugin, "pave2parse", 0, GST_TYPE_PAVE2PARSE);
}

GST_PLUGIN_DEFINE(
	GST_VERSION_MAJOR,
	GST_VERSION_MINOR,
	pave2parse,
	"pave2parse",
	pave2parse_plugin_init,
	VERSION,
	"LGPL",
	PACKAGE_NAME,
	PACKAGE_URL
	)