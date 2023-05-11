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

DISK='sdb'
LOG_FILE='/var/log/load_io_disk.log'
PIDFILE='/var/run/load_io_disk.pid'

iostat -x ${DISK} -d 1 -t -m > ${LOG_FILE} & echo $! > ${PIDFILE}

#function run_log_io_disk() {
#  while :
#  do
#    diskIO=$(iostat -x ${DISK} -d -m | grep ${DISK})
#    echo "$(date +"%T") ${diskIO}"
#    sleep 1
#  done
#}
#
#run_log_io_disk > ${LOG_FILE} & echo $! > ${PIDFILE}

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
### Параметры
| Параметр | Описание |
| - | - |
| Device | Имя устройства |
| tps | Указывает количество передач (запрос ввода-вывода) в секунду, которые были отправлены на устройство. Несколько логических запросов могут быть объединены в один запрос ввода-вывода к устройству |
| Blk_read/s | Укажите объем данных, считываемых с устройства, выраженный в количестве блоков (512 байт) в секунду |
| Blk_wrtn/s | Укажите объем данных, записываемых на устройство, выраженный в количестве блоков (512 байт) в секунду |
| Blk_read | Общее количество прочитанных блоков |
| Blk_wrtn | Общее количество записанных блоков |
| kB_read/s | Объем данных, считываемых с устройства, выраженный в килобайтах в секунду |
| kB_wrtn/s | Объем данных, записываемых на устройство, выраженный в килобайтах в секунду |
| kB_read | Общее количество прочитанных килобайт |
| kB_wrtn | Общее количество записанных килобайт |
| MB_read/s | Объем данных, считываемых с устройства, выраженный в мегабайтах в секунду |
| MB_wrtn/s | Объем данных, записываемых на устройство, выраженный в мегабайтах в секунду |
| MB_read | Общее количество прочитанных мегабайт |
| MB_wrtn | Общее количество записанных мегабайт |
| rrqm/s | Количество объединяемых запросов на чтение в секунду, которые были поставлены в очередь на устройство |
| wrqm/s | Количество объединяемых запросов на запись в секунду, которые были поставлены в очередь на устройство |
| r/s | Количество запросов на чтение, которые были отправлены устройству в секунду |
| w/s | Количество запросов на запись, которые были отправлены устройству в секунду |
| rsec/s | Количество секторов, считываемых с устройства в секунду |
| wsec/с | Количество секторов, записываемых на устройство в секунду |
| rkB/s | Количество килобайт, считываемых с устройства в секунду |
| wkB/s | Количество килобайт, записываемых на устройство в секунду |
| rMB/s | Количество мегабайт, считываемых с устройства в секунду |
| wMB/s | Количество мегабайт, записываемых на устройство в секунду |
| avgrq-sz | Средний размер (в секторах) запросов, которые были отправлены устройству |
| avgqu-sz | Средняя длина очереди запросов, которые были отправлены устройству |
| await | Среднее время (в миллисекундах) для обработки запросов ввода-вывода, отправляемых на устройство. Это включает в себя время, затраченное на запросы в очереди, и время, затраченное на их обслуживание |
| svctm | Среднее время обслуживания (в миллисекундах) запросов ввода-вывода, которые были отправлены устройству |
| %util | Процент процессорного времени, в течение которого устройству были отправлены запросы ввода-вывода (использование полосы пропускания устройства). Насыщение устройства происходит, когда это значение близко к 100% |
