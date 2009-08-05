// gs_capture.cpp

#include "stdafx.h"

/*
	and you say, whats mine?
	and somebody else says, well what is?
	and you say, oh my god!
	am I here all alone?
*/

CGsCapture::CGsCapture()
{
}

void CGsCapture::Destroy()
{
}

static void bus_callback(GstBus *bus, GstMessage *message, void *appdata)
{
	oexEcho( "bus_callback()" );

	gchar *message_str;
	const gchar *message_name;
	GError *error;

	/* Report errors to the console */
	if(GST_MESSAGE_TYPE(message) == GST_MESSAGE_ERROR)
	{
		gst_message_parse_error(message, &error, &message_str);
		g_error("GST error: %s\n", message_str);
		g_free(error);
		g_free(message_str);
	}

	/* Report warnings to the console */
	if(GST_MESSAGE_TYPE(message) == GST_MESSAGE_WARNING)
	{
		gst_message_parse_warning(message, &error, &message_str);
		g_warning("GST warning: %s\n", message_str);
		g_free(error);
		g_free(message_str);
	}

	/* See if the message type is GST_MESSAGE_APPLICATION which means
	 * thet the message is sent by the client code (this program) and
	 * not by gstreamer. */
	if(GST_MESSAGE_TYPE(message) == GST_MESSAGE_APPLICATION)
	{
		/* Get name of the message's structure */
		message_name = gst_structure_get_name(gst_message_get_structure(message));

		/* The hildon banner must be shown in here, because the bus callback is
		 * called in the main thread and calling GUI-functions in gstreamer threads
		 * usually leads to problems with X-server */

		/* "photo-taken" message means that the photo was succefully taken
		 * and saved and message is shown to user */
		if(!strcmp(message_name, "photo-taken"))
		{
		}

		/* "photo-failed" means that the photo couldn't be captured or saved */
		if(!strcmp(message_name, "photo-failed"))
		{
		}
	}

}

// gst_registry_find_feature

#define VIDEO_SRC "v4l2src"
#define VIDEO_SINK "xvimagesink"

//#define VIDEO_SRC "v4lsrc"
//#define VIDEO_SINK "ximagesink"

//#define VIDEO_SRC "gnomevfssrc"

int CGsCapture::Init()
{
	GstElement *pipeline, *camera_src, *screen_sink, *image_sink;
	GstElement *screen_queue, *image_queue;
	GstElement *csp_filter, *image_filter, *tee;
	GstCaps *caps;
	GstBus *bus;
	GMainLoop *loop;

	/* Initialize Gstreamer */
//	gst_init( 0, 0 );

oexM();
	GError *err = 0;
	if ( !gst_init_check( 0, 0, &err ) )
	{	oexSHOW( err->message );
		g_error_free( err );
		oexEcho( "gst_init_check() failed" );
		return -1;
	}
oexM();

	loop = g_main_loop_new (NULL, FALSE);
	oexSHOW( (int)loop );

	/* Create pipeline and attach a callback to it's
	 * message bus */
	pipeline = gst_pipeline_new("test-camera");

	bus = gst_pipeline_get_bus( GST_PIPELINE( pipeline ) );
	gst_bus_add_watch( bus, (GstBusFunc)bus_callback, loop );
	gst_object_unref( GST_OBJECT( bus ) );

/*
	GstElement *filesrc  = gst_element_factory_make ("filesrc", "my_filesource");
	if ( !filesrc )
	{
		oexEcho( "gst_element_factory_make() failed" );
		return -1;
	} // end if
*/

	/* Create elements */
	/* Camera video stream comes from a Video4Linux driver */
	camera_src = gst_element_factory_make("v4l2src", "camera_src");

//	gst_play_error_plugin (VIDEO_SRC, &err);

oexM();

	/* Colorspace filter is needed to make sure that sinks understands
	 * the stream coming from the camera */
	csp_filter = gst_element_factory_make("ffmpegcolorspace", "csp_filter");

oexM();

	/* Tee that copies the stream to multiple outputs */
	tee = gst_element_factory_make("tee", "tee");


	/* Queue creates new thread for the stream */
	screen_queue = gst_element_factory_make("queue", "screen_queue");


	/* Sink that shows the image on screen. Xephyr doesn't support XVideo
	 * extension, so it needs to use ximagesink, but the device uses
	 * xvimagesink */
	screen_sink = gst_element_factory_make(VIDEO_SINK, "screen_sink");
	/* Creates separate thread for the stream from which the image
	 * is captured */
	image_queue = gst_element_factory_make("queue", "image_queue");
	/* Filter to convert stream to use format that the gdkpixbuf library
	 * can use */
	image_filter = gst_element_factory_make("ffmpegcolorspace", "image_filter");

	/* A dummy sink for the image stream. Goes to bitheaven */
	image_sink = gst_element_factory_make("fakesink", "image_sink");

oexM();

	/* Check that elements are correctly initialized */
	if(!(pipeline && bus && camera_src && screen_sink && csp_filter && screen_queue
		&& image_queue && image_filter && image_sink))
	{
		oexSHOW( (long)pipeline );
		oexSHOW( (long)bus );
		oexSHOW( (long)camera_src );
		oexSHOW( (long)screen_sink );
		oexSHOW( (long)csp_filter );
		oexSHOW( (long)screen_queue );
		oexSHOW( (long)image_queue );
		oexSHOW( (long)image_filter );
		oexSHOW( (long)image_sink );

		oexEcho("Couldn't create pipeline elements");
		return -1;
	}

	/* Set image sink to emit handoff-signal before throwing away
	 * it's buffer */
	g_object_set(G_OBJECT(image_sink),
			"signal-handoffs", TRUE, NULL);

	/* Add elements to the pipeline. This has to be done prior to
	 * linking them */
	gst_bin_add_many(GST_BIN(pipeline), camera_src, csp_filter,
			tee, screen_queue, screen_sink, image_queue,
			image_filter, image_sink, NULL);

	/* Specify what kind of video is wanted from the camera */
	caps = gst_caps_new_simple("video/x-raw-rgb",
			"width", G_TYPE_INT, 640,
			"height", G_TYPE_INT, 480,
			NULL);


oexM();

	/* Link the camera source and colorspace filter using capabilities
	 * specified */
	if(!gst_element_link_filtered(camera_src, csp_filter, caps))
	{
		oexEcho( "gst_element_link_filtered() failed" );
		return -1;
	}
	gst_caps_unref(caps);

	/* Connect Colorspace Filter -> Tee -> Screen Queue -> Screen Sink
	 * This finalizes the initialization of the screen-part of the pipeline */
	if(!gst_element_link_many(csp_filter, tee, screen_queue, screen_sink, NULL))
	{
		oexEcho( "gst_element_link_many() failed" );
		return -1;
	}

	/* gdkpixbuf requires 8 bits per sample which is 24 bits per
	 * pixel */
	caps = gst_caps_new_simple("video/x-raw-rgb",
			"width", G_TYPE_INT, 640,
			"height", G_TYPE_INT, 480,
			"bpp", G_TYPE_INT, 24,
			"depth", G_TYPE_INT, 24,
			"framerate", GST_TYPE_FRACTION, 15, 1,
			NULL);

oexM();

	/* Link the image-branch of the pipeline. The pipeline is
	 * ready after this */
	if(!gst_element_link_many(tee, image_queue, image_filter, NULL))
	{	oexEcho( "gst_element_link_many() failed" );
		return -1;
	}

	if(!gst_element_link_filtered(image_filter, image_sink, caps))
	{	oexEcho( "gst_element_link_filtered() failed" );
		return -1;
	}

	gst_caps_unref(caps);

	/* As soon as screen is exposed, window ID will be advised to the sink */
//	g_signal_connect(appdata->screen, "expose-event", G_CALLBACK(expose_cb),
//			 screen_sink);

oexM();

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

oexM();

	{ // Take snap shot

		GstElement *image_sink;

		/* Get the image sink element from the pipeline */
		image_sink = gst_bin_get_by_name(GST_BIN(pipeline),
				"image_sink");

		if ( !image_sink )
		{	oexEcho( "image_sink is null" );
			return -1;
		}


		/* Display a note to the user */
//		hildon_banner_show_information(GTK_WIDGET(appdata->window),
	//		NULL, "Taking Photo");

		/* Connect the "handoff"-signal of the image sink to the
		 * callback. This gets called whenever the sink gets a
		 * buffer it's ready to pass forward on the pipeline */
//		appdata->buffer_cb_id = g_signal_connect(
//				G_OBJECT(image_sink), "handoff",
//				G_CALLBACK(buffer_probe_callback), appdata);
	}

	return 0;
}

