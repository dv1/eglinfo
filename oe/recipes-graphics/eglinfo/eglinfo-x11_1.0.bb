EGLINFO_PLATFORM ?= "x11"
EGLINFO_DEVICE ?= "generic"
EGLINFO_BINARY_NAME ?= "eglinfo-x11"

include eglinfo.inc

SUMMARY += "(X11 version)"

PR = "r0"
