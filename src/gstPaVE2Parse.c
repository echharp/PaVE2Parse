#include "config.h"

#include <gst/gst.h>
#include "gstPaVE2Parse.h"
#include "pave.h"

static gboolean pave2parse_plugin_init(GstPlugin* plugin)
{
	return gst_element_register(plugin, "PaVE2Parse", GST_RANK_NONE, GST_TYPE_PAVE2PARSE);
}

GST_PLUGIN_DEFINE(
	GST_VERSION_MAJOR,
	GST_VERSION_MINOR,
	PaVE2Parse,
	"PaVE2Parse",
	pave2parse_plugin_init,
	VERSION,
	"LPGL",
	"Gstreamer",
	"http://gstreamer.net"
	)