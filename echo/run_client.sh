#!/bin/bash

PORT=$(./free-port.sh)
./echoclient $PORT

