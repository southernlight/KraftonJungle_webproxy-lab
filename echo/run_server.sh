#!/bin/bash

PORT=$(./free-port.sh)
./echoserver $PORT

