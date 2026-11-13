#!/bin/sh
# Motorola Moto G7 Play
# SPDX-License-Identifier: AGPL-3.0-only

export SXMO_MONITOR="DSI-1"
export SXMO_POWER_BUTTON="0:0:pm8941_pwrkey"
export SXMO_VOLUME_BUTTON="1:1:gpio-keys 0:0:pm8941_resin"
export SXMO_VIBRATE_DEV="/dev/input/by-path/platform-78b5000.i2c-event"
