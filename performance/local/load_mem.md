# Мониторинг загрузки оперативной памяти

## Мониторинг при помощи утилиты free

### Скрипт запуска
```bash
$ sudo touch /usr/bin/load_mem
$ sudo chmod 777 /usr/bin/load_mem
$ sudo nano /usr/bin/load_mem
```
```bash
#!/bin/bash

LOG_FILE='/var/log/load_mem.log'
PIDFILE='/var/run/load_mem.pid'

function run_log_mem() {
  while :
  do
    memUsage=$(free -m | awk '/Mem/{print $3}')
	#memUsage=$(free -m | awk 'NR==2{printf "%.2f%%\t\t", $3*100/$2 }')
    echo "$(date +"%T") ${memUsage} MB"
    sleep 1
  done
}

run_log_mem > ${LOG_FILE} & echo $! > ${PIDFILE}
```
### Сервис init.d
```bash
$ sudo touch /etc/init.d/log_mem
$ sudo chmod 777 /etc/init.d/log_mem
$ sudo nano /etc/init.d/log_mem
```
```bash
#! /bin/sh

### BEGIN INIT INFO
# Provides:          Logging of load mem
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Logging of load mem
### END INIT INFO

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
PIDFILE='/var/run/load_mem.pid'
DAEMON="/usr/bin/load_mem"

#test -x ${DAEMON} || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting load_mem"
     start_daemon -p ${PIDFILE} ${DAEMON}
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping load_mem"
     killproc -p ${PIDFILE} ${DAEMON}
     PID=$(ps ax | grep ${DAEMON} | grep -v grep | awk '{print $1}')
     kill -9 ${PID}
     log_end_msg $?
   ;;
  force-reload|restart)
     $0 stop
     $0 start
   ;;
  status)
     status_of_proc -p ${PIDFILE} ${DAEMON} atd && exit 0 || exit $?
   ;;
 *)
   echo "Usage: /etc/init.d/log_mem {start|stop|restart|force-reload|status}"
   exit 1
  ;;
esac

exit 0
```
### Сервис systemctl
```bash
$ sudo touch /etc/systemd/system/log_mem.service
$ sudo chmod +x /etc/systemd/system/log_mem.service
$ sudo nano /etc/systemd/system/log_mem.service
```
```bash
[Unit]
Description=Logging of load disk

[Service]
Type=forking
User=root
ExecStart=/etc/init.d/log_mem start
ExecStop=/etc/init.d/log_mem stop
ExecReload=/etc/init.d/log_mem restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```
### Запуск
```bash
$ sudo systemctl daemon-reload
$ sudo systemctl start log_mem.service
$ sudo systemctl status log_mem.service
$ sudo systemctl stop log_mem.service
```
