# Мониторинг загрузки оборудования

## Мониторинг при помощи различных утилит

### Скрипт запуска
```bash
$ sudo touch /usr/bin/load_hw
$ sudo chmod 777 /usr/bin/load_hw
$ sudo nano /usr/bin/load_hw
```
```bash
#!/bin/bash

LOG_FILE='/var/log/load_hw.csv'
PIDFILE='/var/run/load_hw.pid'

echo 'time,cpu_%,mem_MB,disk_MB' > ${LOG_FILE}

function run_log_hw() {
  while :
  do
    cpuUsage=$(top -bn1 | grep load | head -1 | awk '{printf "%.2f", $(NF-2)}')
    memUsage=$(free -m | awk '/Mem/{print $3}')
    # memUsage=$(free -m | awk 'NR==2{printf "%.2f%%", $3*100/$2 }')
    diskUsage=$(df -h -BMB | awk '$NF=="/"{printf "%s", $3}' | sed s/[^0-9.]//g)
    echo "$(date +"%T"),${cpuUsage},${memUsage},${diskUsage}"
    sleep 1
  done
}

run_log_hw >> ${LOG_FILE} & echo $! > ${PIDFILE}
```

### Сервис init.d

```bash
$ sudo touch /etc/init.d/log_hw
$ sudo chmod 777 /etc/init.d/log_hw
$ sudo nano /etc/init.d/log_hw
```
```bash
#! /bin/sh

### BEGIN INIT INFO
# Provides:          Logging of load hw
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Logging of load hw
### END INIT INFO

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
PIDFILE='/var/run/load_hw.pid'
DAEMON="/usr/bin/load_hw"

#test -x ${DAEMON} || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting load_hw"
     start_daemon -p ${PIDFILE} ${DAEMON}
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping load_hw"
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
   echo "Usage: /etc/init.d/log_hw {start|stop|restart|force-reload|status}"
   exit 1
  ;;
esac

exit 0
```

### Сервис systemctl

```bash
$ sudo touch /etc/systemd/system/log_hw.service
$ sudo chmod +x /etc/systemd/system/log_hw.service
$ sudo nano /etc/systemd/system/log_hw.service
```
```bash
[Unit]
Description=Logging of load hw

[Service]
Type=forking
User=root
ExecStart=/etc/init.d/log_hw start
ExecStop=/etc/init.d/log_hw stop
ExecReload=/etc/init.d/log_hw restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

### Запуск

```bash
$ sudo systemctl daemon-reload
$ sudo systemctl start log_hw.service
$ sudo systemctl status log_hw.service
$ sudo systemctl stop log_hw.service
```
