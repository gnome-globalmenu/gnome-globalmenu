#! /bin/bash

source ../../patch.sh

patch '/g_mem_set_vtable/d' test-leak.c
patch '/g_type_init/i g_mem_set_vtable (glib_mem_profiler_table);' test-leak.c

