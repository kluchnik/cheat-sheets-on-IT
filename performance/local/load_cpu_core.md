# Мониторинг загрузки ядерер процессора

## Мониторинг при помощи утилиты mpstat

### Скрипт запуска
```bash
$ sudo touch /usr/bin/load_cpu_core
$ sudo chmod 777 /usr/bin/load_cpu_core
$ sudo nano /usr/bin/load_cpu_core
```
```bash
#!/bin/bash

LOG_FILE='/var/log/load_cpu_core.csv'
PIDFILE='/var/run/load_cpu_core.pid'

max_cpu_core=$( lscpu | grep '^CPU(s):' | awk '{ print $2 }' )
header='time,'
for (( item=1; item<=${max_cpu_core}; item++ )) do
  header="${header}${item},"
done
header=$( echo ${header} | sed 's/,$//' )
echo ${header} > ${LOG_FILE}

if [[ -n $( date | grep -E "(AM|PM)" ) ]];
  then time_type=1
  else time_type=0
fi

function run_log_cpu_core() {
  while :
  do
    result=''
    list_load_cpu=$( mpstat -P ALL | grep -i -E -w '^[0-9]+:[0-9]+:[0-9]+ (AM|PM|)[ ]+[0-9]+' )
    for (( item=0; item<${max_cpu_core}; item++ )); do
      if [[ ${time_type} -eq 1 ]];
      then
        load_cpu_core=$( echo ${list_load_cpu} | grep -i -E -w "^[0-9]+:[0-9]+:[0-9]+ (AM|PM)[ ]+${item} " | sed 's/,/./' | awk '{sum=$4+$6} END {print sum}' )
      else
        load_cpu_core=$( echo ${list_load_cpu} | grep -i -E -w "^[0-9]+:[0-9]+:[0-9]+[ ]+${item} " | sed 's/,/./' | awk '{sum=$3+$5} END {print sum}' )
      fi
      result="${result}${load_cpu_core},"
    done
    result=$( echo ${result} | sed 's/,$//' )
    echo "$(date +"%T"),${result}"
    sleep 1
  done
}

IFS=$'\x10'
run_log_cpu_core >> ${LOG_FILE} & echo $! > ${PIDFILE}
```

### Сервис init.d

```bash
$ sudo touch /etc/init.d/log_cpu_core
$ sudo chmod 777 /etc/init.d/log_cpu_core
$ sudo nano /etc/init.d/log_cpu_core
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
PIDFILE='/var/run/load_cpu_core.pid'
DAEMON="/usr/bin/load_cpu_core"

#test -x ${DAEMON} || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting load_cpu_core"
     start_daemon -p ${PIDFILE} ${DAEMON}
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping load_cpu_core"
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
   echo "Usage: /etc/init.d/log_cpu_core {start|stop|restart|force-reload|status}"
   exit 1
  ;;
esac

exit 0
```

### Сервис systemctl

```bash
$ sudo touch /etc/systemd/system/log_cpu_core.service
$ sudo chmod +x /etc/systemd/system/log_cpu_core.service
$ sudo nano /etc/systemd/system/log_cpu_core.service
```
```bash
[Unit]
Description=Logging of load cpu core

[Service]
Type=forking
User=root
ExecStart=/etc/init.d/log_cpu_core start
ExecStop=/etc/init.d/log_cpu_core stop
ExecReload=/etc/init.d/log_cpu_core restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

### Запуск

```bash
$ sudo systemctl daemon-reload
$ sudo systemctl start log_cpu_core.service
$ sudo systemctl status log_cpu_core.service
$ sudo systemctl stop log_cpu_core.service
```
