# Мониторинг загрузки жеского диска

## Мониторинг при помощи утилиты df

### Скрипт запуска
```bash
$ sudo touch /usr/bin/load_disk
$ sudo chmod 777 /usr/bin/load_disk
$ sudo nano /usr/bin/load_disk
```
```bash
#!/bin/bash

DISK='/dev/sdb2'
LOG_FILE='/var/log/load_disk.log'
PIDFILE='/var/run/load_disk.pid'

function run_log_disk() {
  while :
  do
    diskUsage=$(df -h ${DISK} -BMB | tail -1 | awk '{print $3}')
    #diskUsage=$(df -h | awk '$NF=="/"{printf "%s\t\t", $5}')
    echo "$(date +"%T") ${diskUsage}"
    sleep 1
  done
}

run_log_disk > ${LOG_FILE} & echo $! > ${PIDFILE}
```

### Сервис init.d

```bash
$ sudo touch /etc/init.d/log_disk
$ sudo chmod 777 /etc/init.d/log_disk
$ sudo nano /etc/init.d/log_disk
```
```bash
#! /bin/sh

### BEGIN INIT INFO
# Provides:          Logging of load disk
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Logging of load disk
### END INIT INFO

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
PIDFILE='/var/run/load_disk.pid'
DAEMON="/usr/bin/load_disk"

#test -x ${DAEMON} || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting load_disk"
     start_daemon -p ${PIDFILE} ${DAEMON}
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping load_disk"
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
   echo "Usage: /etc/init.d/log_disk {start|stop|restart|force-reload|status}"
   exit 1
  ;;
esac

exit 0
```

### Сервис systemctl

```bash
$ sudo touch /etc/systemd/system/log_disk.service
$ sudo chmod +x /etc/systemd/system/log_disk.service
$ sudo nano /etc/systemd/system/log_disk.service
```
```bash
[Unit]
Description=Logging of load disk

[Service]
Type=forking
User=root
ExecStart=/etc/init.d/log_disk start
ExecStop=/etc/init.d/log_disk stop
ExecReload=/etc/init.d/log_disk restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

### Запуск

```bash
$ sudo systemctl daemon-reload
$ sudo systemctl start log_disk.service
$ sudo systemctl status log_disk.service
$ sudo systemctl stop log_disk.service
```
