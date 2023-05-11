# Мониторинг нагрузки на жесткий диск в linux

## Мониторинг при помощи утилиты iostat

### Скрипт запуска
```bash
$ sudo touch /usr/bin/load_io_disk
$ sudo chmod 777 /usr/bin/load_io_disk
$ sudo nano /usr/bin/load_io_disk
```
```bash
#!/bin/bash

DISK_NAME='sdb'
LOG_FILE='/var/log/load_io_disk.log'
PIDFILE='/var/run/load_io_disk.pid'

iostat -x ${DISK_NAME} -d 1 -t -m > ${LOG_FILE} & echo $! > ${PIDFILE}
```
### Сервис init.d
```bash
$ sudo touch /etc/init.d/log_io_disk
$ sudo chmod 777 /etc/init.d/log_io_disk
$ sudo nano /etc/init.d/log_io_disk
```
```bash
#! /bin/sh

### BEGIN INIT INFO
# Provides:          Logging of load io disk
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Logging of load io disk
### END INIT INFO

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
PIDFILE='/var/run/load_io_disk.pid'
DAEMON="/usr/bin/load_io_disk"

#test -x ${DAEMON} || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting load_io_disk"
     start_daemon -p ${PIDFILE} ${DAEMON}
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping load_io_disk"
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
   echo "Usage: /etc/init.d/log_io_disk {start|stop|restart|force-reload|status}"
   exit 1
  ;;
esac

exit 0
```
### Сервис systemctl
```bash
$ sudo touch /etc/systemd/system/log_io_disk.service
$ sudo chmod +x /etc/systemd/system/log_io_disk.service
$ sudo nano /etc/systemd/system/log_io_disk.service
```
```bash
[Unit]
Description=Logging of load io disk

[Service]
Type=forking
User=root
ExecStart=/etc/init.d/log_io_disk start
ExecStop=/etc/init.d/log_io_disk stop
ExecReload=/etc/init.d/log_io_disk restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```
### Запуск
```bash
$ sudo systemctl daemon-reload
$ sudo systemctl start log_io_disk.service
$ sudo systemctl status log_io_disk.service
$ sudo systemctl stop log_io_disk.service
```
