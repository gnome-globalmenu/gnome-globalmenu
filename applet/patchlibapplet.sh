#! /bin/bash

source ../patch.sh

patch '/g_return_if_fail (previous_window != NULL)/d' monitor.c

