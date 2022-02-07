#!/bin/sh

if [ ! -d .env ]; then
    python3 -m venv .env
fi

if [ ! -s server/robotsystem.so ]; then
    ( 
        cd server; 
        ln -s ../robotsystem/build/robotsystem.so 
    )
fi

if [ ! -d ui/node_modules ]; then
    (
        cd ui
        npm install
    )
fi