# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libgstmyplugin.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################


## Plugin 3 (my filter)

# sources used to compile this plug-in
libgstmyplugin_la_SOURCES = gstmyplugin.cc gstmyplugin.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstmyplugin_la_CXXFLAGS = $(GST_CFLAGS) `pkg-config --cflags opencv`
libgstmyplugin_la_LIBADD = $(GST_LIBS)
libgstmyplugin_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) `pkg-config --libs opencv`
libgstmyplugin_la_LIBTOOLFLAGS = --tag=disable-static
#libgstmyplugin_la_EXTRA_CXXFLAGS = `pkg-config --cflags gstreamer-plugins-base-1.0`
#libgstmyplugin_la_EXTRA_LDFLAGS = `pkg-config --libs gstreamer-plugins-base-1.0`

libgstmyplugin_la_EXTRA_CXXFLAGS = `-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -pthread -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2`

libgstmyplugin_la_EXTRA_LDFLAGS = `-Wl,--export-dynamic -pthread -lgstreamer-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lgthread-2.0 -lxml2 -lpthread -lz -lm -lglib-2.0`

# headers we need but don't want installed
noinst_HEADERS = gstmyplugin.h
