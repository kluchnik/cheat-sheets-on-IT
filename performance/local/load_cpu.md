# Мониторинг загрузки процессора

## Мониторинг при помощи утилиты top

### Скрипт запуска
```bash
$ sudo touch /usr/bin/load_cpu
$ sudo chmod 777 /usr/bin/load_cpu
$ sudo nano /usr/bin/load_cpu
```
```bash
#!/bin/bash

LOG_FILE='/var/log/load_cpu.log'
PIDFILE='/var/run/load_cpu.pid'

function run_log_cpu() {
  while :
  do
    cpuUsage=$(top -bn1 | grep load | head -1 | awk '{printf "%.2f%%\t\t\n", $(NF-2)}')
    echo "$(date +"%T") ${cpuUsage}"
    sleep 1
  done
}

run_log_cpu > ${LOG_FILE} & echo $! > ${PIDFILE}
```
### Сервис init.d
```bash
$ sudo touch /etc/init.d/log_cpu
$ sudo chmod 777 /etc/init.d/log_cpu
$ sudo nano /etc/init.d/log_cpu
```
```bash
#! /bin/sh

### BEGIN INIT INFO
# Provides:          Logging of load cpu
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Logging of load cpu
### END INIT INFO

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
PIDFILE='/var/run/load_cpu.pid'
DAEMON="/usr/bin/load_cpu"

#test -x ${DAEMON} || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting load_cpu"
     start_daemon -p ${PIDFILE} ${DAEMON}
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping load_cpu"
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
   echo "Usage: /etc/init.d/log_cpu {start|stop|restart|force-reload|status}"
   exit 1
  ;;
esac

exit 0
```
### Сервис systemctl
```bash
$ sudo touch /etc/systemd/system/log_cpu.service
$ sudo chmod +x /etc/systemd/system/log_cpu.service
$ sudo nano /etc/systemd/system/log_cpu.service
```
```bash
[Unit]
Description=Logging of load cpu

[Service]
Type=forking
User=root
ExecStart=/etc/init.d/log_cpu start
ExecStop=/etc/init.d/log_cpu stop
ExecReload=/etc/init.d/log_cpu restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```
### Запуск
```bash
$ sudo systemctl daemon-reload
$ sudo systemctl start log_cpu.service
$ sudo systemctl status log_cpu.service
$ sudo systemctl stop log_cpu.service
```
