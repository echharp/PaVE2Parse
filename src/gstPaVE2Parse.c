/* GStreamer
 * Copyright (C) 2015 Christian Harper-Cyr <christian.harper-cyr@ericsson.com>
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
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstpave2parse
 *
 * The pave2parse parses PaVE video stream into H264 (and more later).
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v videotestsrc ! pave2parse ! decodebin ! autovideosink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/gst.h>
#include "gstpave2parse.h"

GST_DEBUG_CATEGORY_STATIC (gst_pave2parse_debug_category);
#define GST_CAT_DEFAULT gst_pave2parse_debug_category

/* prototypes */


static void gst_pave2parse_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_pave2parse_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_pave2parse_dispose (GObject * object);
static void gst_pave2parse_finalize (GObject * object);

static GstPad *gst_pave2parse_request_new_pad (GstElement * element,
    GstPadTemplate * templ, const gchar * name);
static void gst_pave2parse_release_pad (GstElement * element, GstPad * pad);
static GstStateChangeReturn
gst_pave2parse_change_state (GstElement * element, GstStateChange transition);
static GstClock *gst_pave2parse_provide_clock (GstElement * element);
static gboolean gst_pave2parse_set_clock (GstElement * element, GstClock * clock);
static GstIndex *gst_pave2parse_get_index (GstElement * element);
static void gst_pave2parse_set_index (GstElement * element, GstIndex * index);
static gboolean gst_pave2parse_send_event (GstElement * element, GstEvent * event);
static gboolean gst_pave2parse_query (GstElement * element, GstQuery * query);

static GstCaps* gst_pave2parse_sink_getcaps (GstPad *pad);
static gboolean gst_pave2parse_sink_setcaps (GstPad *pad, GstCaps *caps);
static gboolean gst_pave2parse_sink_acceptcaps (GstPad *pad, GstCaps *caps);
static void gst_pave2parse_sink_fixatecaps (GstPad *pad, GstCaps *caps);
static gboolean gst_pave2parse_sink_activate (GstPad *pad);
static gboolean gst_pave2parse_sink_activatepush (GstPad *pad, gboolean active);
static gboolean gst_pave2parse_sink_activatepull (GstPad *pad, gboolean active);
static GstPadLinkReturn gst_pave2parse_sink_link (GstPad *pad, GstPad *peer);
static void gst_pave2parse_sink_unlink (GstPad *pad);
static GstFlowReturn gst_pave2parse_sink_chain (GstPad *pad, GstBuffer *buffer);
static GstFlowReturn gst_pave2parse_sink_chainlist (GstPad *pad, GstBufferList *bufferlist);
static gboolean gst_pave2parse_sink_event (GstPad *pad, GstEvent *event);
static gboolean gst_pave2parse_sink_query (GstPad *pad, GstQuery *query);
static GstFlowReturn gst_pave2parse_sink_bufferalloc (GstPad *pad, guint64 offset, guint size,
    GstCaps *caps, GstBuffer **buf);
static GstIterator * gst_pave2parse_sink_iterintlink (GstPad *pad);


static GstCaps* gst_pave2parse_src_getcaps (GstPad *pad);
static gboolean gst_pave2parse_src_setcaps (GstPad *pad, GstCaps *caps);
static gboolean gst_pave2parse_src_acceptcaps (GstPad *pad, GstCaps *caps);
static void gst_pave2parse_src_fixatecaps (GstPad *pad, GstCaps *caps);
static gboolean gst_pave2parse_src_activate (GstPad *pad);
static gboolean gst_pave2parse_src_activatepush (GstPad *pad, gboolean active);
static gboolean gst_pave2parse_src_activatepull (GstPad *pad, gboolean active);
static GstPadLinkReturn gst_pave2parse_src_link (GstPad *pad, GstPad *peer);
static void gst_pave2parse_src_unlink (GstPad *pad);
static GstFlowReturn gst_pave2parse_src_getrange (GstPad *pad, guint64 offset, guint length,
    GstBuffer **buffer);
static gboolean gst_pave2parse_src_event (GstPad *pad, GstEvent *event);
static gboolean gst_pave2parse_src_query (GstPad *pad, GstQuery *query);
static GstIterator * gst_pave2parse_src_iterintlink (GstPad *pad);


enum
{
  PROP_0
};

/* pad templates */

static GstStaticPadTemplate gst_pave2parse_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/unknown")
    );

static GstStaticPadTemplate gst_pave2parse_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_SOMETIMES,
    GST_STATIC_CAPS ("application/unknown")
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstPaVE2Parse, gst_pave2parse, GST_TYPE_ELEMENT,
  GST_DEBUG_CATEGORY_INIT (gst_pave2parse_debug_category, "pave2parse", 0,
  "debug category for pave2parse element"));

static void
gst_pave2parse_class_init (GstPaVE2ParseClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_pave2parse_sink_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_pave2parse_src_template));

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
      "PaVE 2 Parse", "Parse/PaVE2", "Parses PaVE headers",
      "Christian Harper-Cyr <christian.harper-cyr@ericsson.com>");

  gobject_class->set_property = gst_pave2parse_set_property;
  gobject_class->get_property = gst_pave2parse_get_property;
  gobject_class->dispose = gst_pave2parse_dispose;
  gobject_class->finalize = gst_pave2parse_finalize;
  element_class->request_new_pad = GST_DEBUG_FUNCPTR (gst_pave2parse_request_new_pad);
  element_class->release_pad = GST_DEBUG_FUNCPTR (gst_pave2parse_release_pad);
  element_class->change_state = GST_DEBUG_FUNCPTR (gst_pave2parse_change_state);
  element_class->provide_clock = GST_DEBUG_FUNCPTR (gst_pave2parse_provide_clock);
  element_class->set_clock = GST_DEBUG_FUNCPTR (gst_pave2parse_set_clock);
  element_class->get_index = GST_DEBUG_FUNCPTR (gst_pave2parse_get_index);
  element_class->set_index = GST_DEBUG_FUNCPTR (gst_pave2parse_set_index);
  element_class->send_event = GST_DEBUG_FUNCPTR (gst_pave2parse_send_event);
  element_class->query = GST_DEBUG_FUNCPTR (gst_pave2parse_query);

}

static void
gst_pave2parse_init (GstPaVE2Parse *pave2parse)
{

  pave2parse->sinkpad = gst_pad_new_from_static_template (&gst_pave2parse_sink_template
      ,     
            "sink");
  gst_pad_set_getcaps_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_getcaps));
  gst_pad_set_setcaps_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_setcaps));
  gst_pad_set_acceptcaps_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_acceptcaps));
  gst_pad_set_fixatecaps_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_fixatecaps));
  gst_pad_set_activate_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_activate));
  gst_pad_set_activatepush_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_activatepush));
  gst_pad_set_activatepull_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_activatepull));
  gst_pad_set_link_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_link));
  gst_pad_set_unlink_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_unlink));
  gst_pad_set_chain_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_chain));
  gst_pad_set_chain_list_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_chainlist));
  gst_pad_set_event_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_event));
  gst_pad_set_query_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_query));
  gst_pad_set_bufferalloc_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_bufferalloc));
  gst_pad_set_iterate_internal_links_function (pave2parse->sinkpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_sink_iterintlink));
  gst_element_add_pad (GST_ELEMENT(pave2parse), pave2parse->sinkpad);



  pave2parse->srcpad = gst_pad_new_from_static_template (&gst_pave2parse_src_template
      ,     
            "src");
  gst_pad_set_getcaps_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_getcaps));
  gst_pad_set_setcaps_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_setcaps));
  gst_pad_set_acceptcaps_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_acceptcaps));
  gst_pad_set_fixatecaps_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_fixatecaps));
  gst_pad_set_activate_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_activate));
  gst_pad_set_activatepush_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_activatepush));
  gst_pad_set_activatepull_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_activatepull));
  gst_pad_set_link_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_link));
  gst_pad_set_unlink_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_unlink));
  gst_pad_set_getrange_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_getrange));
  gst_pad_set_event_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_event));
  gst_pad_set_query_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_query));
  gst_pad_set_iterate_internal_links_function (pave2parse->srcpad,
            GST_DEBUG_FUNCPTR(gst_pave2parse_src_iterintlink));
  gst_element_add_pad (GST_ELEMENT(pave2parse), pave2parse->srcpad);


}

void
gst_pave2parse_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstPaVE2Parse *pave2parse = GST_PAVE2PARSE (object);

  GST_DEBUG_OBJECT (pave2parse, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_pave2parse_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstPaVE2Parse *pave2parse = GST_PAVE2PARSE (object);

  GST_DEBUG_OBJECT (pave2parse, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_pave2parse_dispose (GObject * object)
{
  GstPaVE2Parse *pave2parse = GST_PAVE2PARSE (object);

  GST_DEBUG_OBJECT (pave2parse, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_pave2parse_parent_class)->dispose (object);
}

void
gst_pave2parse_finalize (GObject * object)
{
  GstPaVE2Parse *pave2parse = GST_PAVE2PARSE (object);

  GST_DEBUG_OBJECT (pave2parse, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_pave2parse_parent_class)->finalize (object);
}



static GstPad *
gst_pave2parse_request_new_pad (GstElement * element, GstPadTemplate * templ,
    const gchar * name)
{

  return NULL;
}

static void
gst_pave2parse_release_pad (GstElement * element, GstPad * pad)
{

}

static GstStateChangeReturn
gst_pave2parse_change_state (GstElement * element, GstStateChange transition)
{
  GstPaVE2Parse *pave2parse;
  GstStateChangeReturn ret;

  g_return_val_if_fail (GST_IS_PAVE2PARSE (element), GST_STATE_CHANGE_FAILURE);
  pave2parse = GST_PAVE2PARSE (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }

  return ret;
}

static GstClock *
gst_pave2parse_provide_clock (GstElement * element)
{

  return NULL;
}

static gboolean
gst_pave2parse_set_clock (GstElement * element, GstClock * clock)
{

  return GST_ELEMENT_CLASS (parent_class)->set_clock (element, clock);
}

static GstIndex *
gst_pave2parse_get_index (GstElement * element)
{

  return NULL;
}

static void
gst_pave2parse_set_index (GstElement * element, GstIndex * index)
{

}

static gboolean
gst_pave2parse_send_event (GstElement * element, GstEvent * event)
{

  return TRUE;
}

static gboolean
gst_pave2parse_query (GstElement * element, GstQuery * query)
{
  GstPaVE2Parse *pave2parse = GST_PAVE2PARSE (element);
  gboolean ret;

  GST_DEBUG_OBJECT (pave2parse, "query");

  switch (GST_QUERY_TYPE (query)) {
    default:
      ret = GST_ELEMENT_CLASS (parent_class)->query (element, query);
      break;
  }

  return ret;
}

static GstCaps*
gst_pave2parse_sink_getcaps (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;
  GstCaps *caps;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "getcaps");

  caps = gst_caps_copy (gst_pad_get_pad_template_caps (pad));

  gst_object_unref (pave2parse);
  return caps;
}

static gboolean
gst_pave2parse_sink_setcaps (GstPad *pad, GstCaps *caps)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "setcaps");


  gst_object_unref (pave2parse);
  return TRUE;
}

static gboolean
gst_pave2parse_sink_acceptcaps (GstPad *pad, GstCaps *caps)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "acceptcaps");


  gst_object_unref (pave2parse);
  return TRUE;
}

static void
gst_pave2parse_sink_fixatecaps (GstPad *pad, GstCaps *caps)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "fixatecaps");


  gst_object_unref (pave2parse);
}

static gboolean
gst_pave2parse_sink_activate (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;
  gboolean ret;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "activate");

  if (gst_pad_check_pull_range (pad)) {
    GST_DEBUG_OBJECT (pad, "activating pull");
    ret = gst_pad_activate_pull (pad, TRUE);
  } else {
    GST_DEBUG_OBJECT (pad, "activating push");
    ret = gst_pad_activate_push (pad, TRUE);
  }

  gst_object_unref (pave2parse);
  return ret;
}

static gboolean
gst_pave2parse_sink_activatepush (GstPad *pad, gboolean active)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "activatepush");


  gst_object_unref (pave2parse);
  return TRUE;
}

static gboolean
gst_pave2parse_sink_activatepull (GstPad *pad, gboolean active)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "activatepull");


  gst_object_unref (pave2parse);
  return TRUE;
}

static GstPadLinkReturn
gst_pave2parse_sink_link (GstPad *pad, GstPad *peer)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "link");


  gst_object_unref (pave2parse);
  return GST_PAD_LINK_OK;
}

static void
gst_pave2parse_sink_unlink (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "unlink");


  gst_object_unref (pave2parse);
}

static GstFlowReturn
gst_pave2parse_sink_chain (GstPad *pad, GstBuffer *buffer)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "chain");


  gst_object_unref (pave2parse);
  return GST_FLOW_OK;
}

static GstFlowReturn
gst_pave2parse_sink_chainlist (GstPad *pad, GstBufferList *bufferlist)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "chainlist");


  gst_object_unref (pave2parse);
  return GST_FLOW_OK;
}

static gboolean
gst_pave2parse_sink_event (GstPad *pad, GstEvent *event)
{
  gboolean res;
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "event");

  switch (GST_EVENT_TYPE (event)) {
    default:
      res = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (pave2parse);
  return res;
}

static gboolean
gst_pave2parse_sink_query (GstPad *pad, GstQuery *query)
{
  gboolean res;
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "query");

  switch (GST_QUERY_TYPE(query)) {
    default:
      res = gst_pad_query_default (pad, query);
      break;
  }

  gst_object_unref (pave2parse);
  return res;
}

static GstFlowReturn
gst_pave2parse_sink_bufferalloc (GstPad *pad, guint64 offset, guint size,
    GstCaps *caps, GstBuffer **buf)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "bufferalloc");


  *buf = gst_buffer_new_and_alloc (size);
  gst_buffer_set_caps (*buf, caps);

  gst_object_unref (pave2parse);
  return GST_FLOW_OK;
}

static GstIterator *
gst_pave2parse_sink_iterintlink (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;
  GstIterator *iter;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "iterintlink");

  iter = gst_pad_iterate_internal_links_default (pad);

  gst_object_unref (pave2parse);
  return iter;
}


static GstCaps*
gst_pave2parse_src_getcaps (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;
  GstCaps *caps;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "getcaps");

  caps = gst_pad_get_pad_template_caps (pad);

  gst_object_unref (pave2parse);
  return caps;
}

static gboolean
gst_pave2parse_src_setcaps (GstPad *pad, GstCaps *caps)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "setcaps");


  gst_object_unref (pave2parse);
  return TRUE;
}

static gboolean
gst_pave2parse_src_acceptcaps (GstPad *pad, GstCaps *caps)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "acceptcaps");


  gst_object_unref (pave2parse);
  return TRUE;
}

static void
gst_pave2parse_src_fixatecaps (GstPad *pad, GstCaps *caps)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "fixatecaps");


  gst_object_unref (pave2parse);
}

static gboolean
gst_pave2parse_src_activate (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;
  gboolean ret;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "activate");

  if (gst_pad_check_pull_range (pad)) {
    GST_DEBUG_OBJECT (pad, "activating pull");
    ret = gst_pad_activate_pull (pad, TRUE);
  } else {
    GST_DEBUG_OBJECT (pad, "activating push");
    ret = gst_pad_activate_push (pad, TRUE);
  }

  gst_object_unref (pave2parse);
  return ret;
}

static gboolean
gst_pave2parse_src_activatepush (GstPad *pad, gboolean active)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "activatepush");


  gst_object_unref (pave2parse);
  return TRUE;
}

static gboolean
gst_pave2parse_src_activatepull (GstPad *pad, gboolean active)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "activatepull");


  gst_object_unref (pave2parse);
  return TRUE;
}

static GstPadLinkReturn
gst_pave2parse_src_link (GstPad *pad, GstPad *peer)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "link");


  gst_object_unref (pave2parse);
  return GST_PAD_LINK_OK;
}

static void
gst_pave2parse_src_unlink (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "unlink");


  gst_object_unref (pave2parse);
}

static GstFlowReturn
gst_pave2parse_src_getrange (GstPad *pad, guint64 offset, guint length,
    GstBuffer **buffer)
{
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "getrange");


  gst_object_unref (pave2parse);
  return GST_FLOW_OK;
}

static gboolean
gst_pave2parse_src_event (GstPad *pad, GstEvent *event)
{
  gboolean res;
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "event");

  switch (GST_EVENT_TYPE (event)) {
    default:
      res = gst_pad_event_default (pad, event);
      break;
  }

  gst_object_unref (pave2parse);
  return res;
}

static gboolean
gst_pave2parse_src_query (GstPad *pad, GstQuery *query)
{
  gboolean res;
  GstPaVE2Parse *pave2parse;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "query");

  switch (GST_QUERY_TYPE(query)) {
    default:
      res = gst_pad_query_default (pad, query);
      break;
  }

  gst_object_unref (pave2parse);
  return res;
}

static GstIterator *
gst_pave2parse_src_iterintlink (GstPad *pad)
{
  GstPaVE2Parse *pave2parse;
  GstIterator *iter;

  pave2parse = GST_PAVE2PARSE (gst_pad_get_parent (pad));

  GST_DEBUG_OBJECT(pave2parse, "iterintlink");

  iter = gst_pad_iterate_internal_links_default (pad);

  gst_object_unref (pave2parse);
  return iter;
}


static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "pave2parse", GST_RANK_NONE,
      GST_TYPE_PAVE2PARSE);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    pave2parse,
    "Parses PaVE headers",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)

