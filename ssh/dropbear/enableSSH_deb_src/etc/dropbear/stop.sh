#!/bin/bash

PID_FILE='/var/run/ssh-server.pid'
PID=${cat ${PID_FILE}}

kill -9 ${PID}
