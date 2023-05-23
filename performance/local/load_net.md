# Мониторинг загрузки сети

```bash
$ sudo nano /etc/default/sysstat
```
```bash
#
# Default settings for /etc/init.d/sysstat, /etc/cron.d/sysstat
# and /etc/cron.daily/sysstat files
#

# Should sadc collect system activity informations? Valid values
# are "true" and "false". Please do not put other values, they
# will be overwritten by debconf!
ENABLED="true"
```
```bash
$ sudo sar -d
```

## Мониторинг при помощи утилиты sar

### Скрипт запуска
```bash
$ sudo touch /usr/bin/load_net
$ sudo chmod 777 /usr/bin/load_net
$ sudo nano /usr/bin/load_net
```
```bash
#!/bin/bash

LOG_FILE='/var/log/load_net.csv'
PIDFILE='/var/run/load_net.pid'
#INTERFACE='none'

echo 'time,iface,rxpps,txpps,rxkBps,txkBps' > ${LOG_FILE}

if [[ -n $( date | grep -E "(AM|PM)" ) ]];
  then time_type=1
  else time_type=0
fi

function run_log_net() {
  while :
  do
    if [[ ${time_type} -eq 1 ]];
      then echo $( sar -n DEV 1 1 | awk '$3 == "enp8s0" { print $1","$3","$4","$5","$6","$7 }' )
      else echo $( sar -n DEV 1 1 | awk '$3 == "enp8s0" { print $1","$2","$3","$4","$5","$6 }' )
    fi
  done
}

IFS=$'\x10'
run_log_net >> ${LOG_FILE} & echo $! > ${PIDFILE}
```

### Сервис init.d

```bash
$ sudo touch /etc/init.d/log_net
$ sudo chmod 777 /etc/init.d/log_net
$ sudo nano /etc/init.d/log_net
```
```bash
#! /bin/sh

### BEGIN INIT INFO
# Provides:          Logging of load network
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: Logging of load network
### END INIT INFO

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin
PIDFILE='/var/run/load_net.pid'
DAEMON='/usr/bin/load_net'

#test -x ${DAEMON} || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting load_net"
     start_daemon -p ${PIDFILE} ${DAEMON}
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping load_net"
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
   echo "Usage: /etc/init.d/load_net {start|stop|restart|force-reload|status}"
   exit 1
  ;;
esac

exit 0
```

### Сервис systemctl

```bash
$ sudo touch /etc/systemd/system/log_net.service
$ sudo chmod +x /etc/systemd/system/log_net.service
$ sudo nano /etc/systemd/system/log_net.service
```
```bash
[Unit]
Description=Logging of load network

[Service]
Type=forking
User=root
ExecStart=/etc/init.d/log_net start
ExecStop=/etc/init.d/log_net stop
ExecReload=/etc/init.d/log_net restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

### Запуск

```bash
$ sudo systemctl daemon-reload
$ sudo systemctl start log_net.service
$ sudo systemctl status log_net.service
$ sudo systemctl stop log_net.service
```
