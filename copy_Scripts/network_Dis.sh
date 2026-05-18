#!/bin/bash

INTERFACE="$1"
STATUS="$2"

if [ "$STATUS" = "up" ] && [[ "$INTERFACE" == wl* ]]; then
    /your/path &
fi