#! /bin/bash

source ../patch.sh

patch '/g_return_if_fail (previous_screen != NULL)/d' applet.c
patch '/g_return_if_fail (previous_parent != NULL)/d' switcher.c
patch '/stdlib.h/i#include <glib/gi18n-lib.h>' main.h
