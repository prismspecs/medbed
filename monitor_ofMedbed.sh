#!/bin/bash

while true; do
    # Change to the directory where your application is located
    cd /path/to/openFrameworks/apps/myApps/ofMedbed/bin/

    # Start your openFrameworks application
    ./ofMedbed

    # Wait for a few seconds before restarting
    sleep 5
done

