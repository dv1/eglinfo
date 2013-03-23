
EGLINFO
=======

This is a utility for displaying information about a platform's EGL support. In addition, it prints
information about the following client APIs (if the platform supports them):

* OpenGL
* OpenGL ES 1.x
* OpenGL ES 2.x
* OpenVG

This includes the list of extensions, supported shader binary formats, supported compressed texture formats, etc.

There are various utilities also called eglinfo. This one came to be because the others lacked many details
(such as what APIs are supported by an EGL configuration, and whether or not they are conformant), and
did not output any information about client APIs. In addition, this utility has the ability to output all of the
information in JSON form.


Building and installing
-----------------------

eglinfo uses the [waf meta build system](https://code.google.com/p/waf/). To configure , run:

    ./waf configure --platform=PLATFORM --device=DEVICE --prefix=PREFIX


PLATFORM is the platform to build for. Currently, the valid values are:

* x11
* fb (framebuffer)


DEVICE is the device to build for. Currently, the valid values are:

* generic (this is the default)
* imx6 (Freescale i.MX6)
* beagleboard
* raspberrypi


If the platform is set to x11, the default generic device is usually enough. If however the platform is fb,
you must set the appropiate DEVICE parameter, because EGL initialization on top of the framebuffer is
device specific.


PREFIX is the installation prefix.


Once configuration is complete, run:

    ./waf

This builds eglinfo.

Finally, to install, run:

    ./waf install

The eglinfo binary will then be installed to ${PREFIX}/bin .


Running
-------

Running eglinfo without parameters outputs information about the current display, using the text writer,
which outputs information in a human-readable fashion. eglinfo accepts the following arguments:

* -h        : Output help, then quit
* -w WRITER : Use specified writer for output. Valid values are: json text
* -d NAME   : Output information for the native display with specified name
              (depends on the platform: on X11, this is an X display name like :0)
* -r        : If using the JSON writer, do a raw print instead of a pretty one


Example:

    eglinfo -d :1 -w json

outputs EGL information about the X11 display ":1", as JSON data.



Extending eglinfo by a device
-----------------------------

To add support for another device, you must:

1. Add a platform\_NAME.cpp file in the src/ directory, and write an implementation of the
   native\_display and native\_window classes in there
2. Add a configuration block to the wscript file
3. Add device NAME to the valid\_devices list in the wscript


License
-------

This program is licensed under the zlib license (see the LICENSE file).

