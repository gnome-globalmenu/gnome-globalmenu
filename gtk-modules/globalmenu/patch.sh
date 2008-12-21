#! /bin/bash

source ../../patch.sh

patch 's/g_log_default_handler_target/NULL/' module-main.c
