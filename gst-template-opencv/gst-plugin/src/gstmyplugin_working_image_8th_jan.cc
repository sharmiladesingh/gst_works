/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) YEAR AUTHOR_NAME AUTHOR_EMAIL
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-plugin
 *
 * FIXME:Describe plugin here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! plugin ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/app/app.h>
#include "opencv2/opencv.hpp"
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include "gstmyplugin.h"
using namespace cv;

GST_DEBUG_CATEGORY_STATIC (gst_my_plugin_template_debug);
#define GST_CAT_DEFAULT gst_my_plugin_template_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

#define gst_my_plugin_template_parent_class parent_class
G_DEFINE_TYPE (GstMyPluginTemplate, gst_my_plugin_template, GST_TYPE_ELEMENT);

static void gst_my_plugin_template_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_my_plugin_template_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_my_plugin_template_sink_event (GstPad * pad, GstObject * parent, GstEvent * event);
static GstFlowReturn gst_my_plugin_template_chain (GstPad * pad, GstObject * parent, GstBuffer * buf);

/* GObject vmethod implementations */

/* initialize the plugin's class */
static void
gst_my_plugin_template_class_init (GstMyPluginTemplateClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_my_plugin_template_set_property;
  gobject_class->get_property = gst_my_plugin_template_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple(gstelement_class,
    "myPlugin",
    "FIXME:Generic",
    "FIXME:Generic Template Element",
    "sharmila <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_my_plugin_template_init (GstMyPluginTemplate * filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_event_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_my_plugin_template_sink_event));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_my_plugin_template_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  filter->silent = FALSE;
}

static void
gst_my_plugin_template_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstMyPluginTemplate *filter = GST_MY_PLUGIN_TEMPLATE (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_my_plugin_template_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstMyPluginTemplate *filter = GST_MY_PLUGIN_TEMPLATE (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


/* GstElement vmethod implementations */

/* this function handles sink events */
static gboolean
gst_my_plugin_template_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstMyPluginTemplate *filter;
  gboolean ret;
  GstVideoInfo *viinfo;

  filter = GST_MY_PLUGIN_TEMPLATE (parent);

  GST_LOG_OBJECT (filter, "Received %s event: %" GST_PTR_FORMAT,
      GST_EVENT_TYPE_NAME (event), event);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_CAPS:
    {
      GstCaps * caps;
      GstStructure *str;
      gint width, height;

      gst_event_parse_caps (event, &caps);
      /* do something with the caps */
      #if 0
      if(gst_video_info_from_caps(viinfo, caps))
      {
        g_print("getting video caps \n");
        #if 0
        GST_VIDEO_INFO_IS_YUV(viinfo);
        GST_VIDEO_INFO_IS_RGB(viinfo);
        GST_VIDEO_INFO_IS_GRAY(viinfo);
        GST_VIDEO_INFO_HAS_ALPHA(viinfo);
        GST_VIDEO_INFO_WIDTH(viinfo);
        GST_VIDEO_INFO_HEIGHT(viinfo);
        #endif
        printf("GST_VIDEO_INFO_SIZE %d \n", GST_VIDEO_INFO_SIZE(viinfo));
      }
      #endif

      
            /* and forward */
      ret = gst_pad_event_default (pad, parent, event);
      
      #if 1
            str = gst_caps_get_structure (caps, 0);
            if (!gst_structure_get_int (str, "width", &width) ||
                !gst_structure_get_int (str, "height", &height)) 
            {
              g_print ("No width/height available\n");
              break;
            }
            g_print ("The video size of this set of capabilities is %dx%d\n",
            width, height);
      #endif
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_my_plugin_template_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstMyPluginTemplate *filter;
  GstBuffer *chainBuf;
  static int framecount = 0;
  bool isSuccess;

  filter = GST_MY_PLUGIN_TEMPLATE (parent);

  if (filter->silent == FALSE){
    g_print ("gst_app_sink_pull_sample.\n");
    //usleep(50000);
  }

  g_print ("Have data of size %" G_GSIZE_FORMAT" bytes!\n", gst_buffer_get_size (buf));

  chainBuf = gst_buffer_copy ((const GstBuffer *)(buf));
  g_print ("gst_buffer_copy to chainbuf\n");

    #if 0
    /* just push out the incoming buffer without touching it */
    return gst_pad_push (filter->srcpad, chainBuf);
    #endif

    //#if 1
    GstMapInfo map;
    gst_buffer_map (chainBuf, &map, GST_MAP_READ);
    //Mat frame0(720, 1280, CV_8UC3, (char*)map.data, Mat::AUTO_STEP);
    //isSuccess = imwrite("Image0.jpg", frame0); //write the image to a file as JPEG 

#if 1
    // isSuccess = imwrite("Image01.jpg", (char*)map.data); //write the image to a file as JPEG 
    // convert gstreamer data to OpenCV Mat, you could actually
    // resolve height / width from caps...
    Mat frame1 ;// = Mat::zeros(720, 1280, CV_8UC3);
    Mat frame2(1080,1280, CV_8UC1, (char*)map.data /*, cv::Mat::AUTO_STEP*/);
    //Mat matLeft = Mat(Size(1280, 720), CV_8UC3, (char*)map.data);
    //cvtColor(matLeft, matLeft, CV_RGB2BGR);

    cvtColor(frame2,frame1,/*CV_YCrCb2RGB*/CV_YUV2BGR_YV12,3);
    //CV_YUV2BGR_YV12
    // int frameSize = map.size;
    //g_print ("frameSize%" G_GSIZE_FORMAT" bytes!\n", frameSize);

    isSuccess = imwrite("Image1.jpg", frame1); //write the image to a file as JPEG 
    isSuccess = imwrite("Image2.jpg", frame2); //write the image to a file as JPEG 
#endif

    framecount++;
    g_print ("Image.jpg.%d \n", framecount);

    gst_buffer_map (chainBuf, &map, GST_MAP_WRITE);
    //buf = gst_buffer_copy ((const GstBuffer *)(chainBuf));

    /* just push out the incoming buffer without touching it */
    return gst_pad_push (filter->srcpad, chainBuf);
   // #endif

}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
my_plugin_init (GstPlugin * plugin)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template plugin' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_my_plugin_template_debug, "myplugin",
      0, "Template my plugin");

  return gst_element_register (plugin, "myplugin", GST_RANK_NONE,
      GST_TYPE_MY_PLUGIN_TEMPLATE);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmyplugin"
#endif

/* gstreamer looks for this structure to register plugins
 *
 * exchange the string 'Template plugin' with your plugin description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    myplugin,
    "Template my plugin",
    my_plugin_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://gstreamer.net/"
)
