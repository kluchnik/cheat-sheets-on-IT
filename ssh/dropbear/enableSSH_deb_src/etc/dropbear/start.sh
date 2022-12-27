#!/bin/bash

BANNER_FILE='/etc/dropbear/banner'
LOG_FILE='/var/log/ssh-server'
PID_FILE='/var/run/dropbear.pid'

if [[ -f ${PID_FILE} ]]; then
  PID=$(cat ${PID_FILE})
  STATUS_SSH=$(ps aux | awk '{print $2}' | grep -E "^${PID}$")
else
  STATUS_SSH=''
fi

if [[ -z ${STATUS_SSH} ]]; then
  /usr/sbin/dropbear -ER -b ${BANNER_FILE} -p 0.0.0.0:22 &>${LOG_FILE}
fi
