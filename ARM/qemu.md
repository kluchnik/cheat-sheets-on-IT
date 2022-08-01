# Виртуализация архитектуры ARM в qemu

## Сборка BIOS (TianoCore UEFI (EDK II))
#### Загрузка исходников
```
$ git clone https://github.com/tianocore/edk2.git
$ cd edk2
$ git submodule update --init
$ cd ..

$ git clone https://github.com/tianocore/edk2-platforms.git
$ cd edk2-platforms
$ git submodule update --init
$ cd ..

$ git clone https://github.com/tianocore/edk2-non-osi.git

$ git clone https://git.linaro.org/uefi/uefi-tools.git
```
#### Сборка TianoCore UEFI (EDK II) под qemu
```
$ GCC5_AARCH64_PREFIX=aarch64-linux-gnu-
$ ./uefi-tools/edk2-build.sh armvirtqemu64
$ ls -lh ./Build/ArmVirtQemu-AARCH64/RELEASE_GCC5/FV/QEMU_EFI.fd
-rw-r--r-- 1 root root 2,0M фев 21 16:48 ./Build/ArmVirtQemu-AARCH64/RELEASE_GCC5/FV/QEMU_EFI.fd
```
#### Пример параметров для запуска qemu
```
$ qemu-system-aarch64 -M virt -cpu cortex-a57 -smp 2 -m 2048 \
  -bios QEMU_EFI.fd -serial mon:stdio -nographic
```
или  
```
$ dd if=/dev/zero bs=1M count=64 of=TianoCore.img
$ dd if=QEMU_EFI.fd of=TianoCore.img bs=1M conv=notrunc

$ qemu-system-aarch64 -M virt -cpu cortex-a57 -smp 2 -m 2048 \
  -pflash TianoCore.img -serial mon:stdio -nographic
```

## Пример установки debian
#### Устанавливаем зависимости
```
$ sudo apt install -y qemu
$ sudo apt install -y qemu-system
$ sudo apt install -y qemu-system-aarch64
$ sudo apt install -y socat
```
#### Скачать дистрибутив
```
$ wget https://cdimage.debian.org/cdimage/archive/10.11.0/arm64/iso-cd/debian-10.11.0-arm64-netinst.iso
```
#### Создать образ для виртуальной машины
```
$ qemu-img create -f qcow2 test_disk.qcow2 24G
```
#### Установка debian с использованием TianoCore UEFI (EDK II) в qemu
```
$ qemu-system-aarch64 -M virt -cpu cortex-a57 -smp 8 -m 4096 \
  -bios QEMU_EFI.fd \
  -serial mon:stdio -nographic \
  -hda disk.qcow2 \
  -drive file=debian-10.11.0-arm64-netinst.iso,id=cdrom,if=none,media=cdrom \
    -device virtio-scsi-device \
    -device scsi-cd,drive=cdrom \
  -net nic,model=e1000 \
  -net user,hostfwd=tcp::2222-:22
```
## Настраиваем qemu как сервис
#### Разрешение ping из ВМ
```
$ sudo groupadd unpriv_ping
$ USERS='user root'
$ for USER in ${USERS}; do
  sudo usermod --append --groups unpriv_ping ${USER}
done
$ (
  GROUP_ID=$(getent group unpriv_ping | cut -f 3 -d :)
  printf 'net.ipv4.ping_group_range = %u %u\n' ${GROUP_ID} ${GROUP_ID} \
    >> /etc/sysctl.conf
)
$ sysctl -p
```
#### Сценарий запуска ВМ (файл /home/arm/run-qemu-arm64.sh)
```
$ cat /home/arm/run-qemu-arm64.sh
```
```
#!/bin/bash

CONSOLE_FILE=/tmp/console.sock
DISK_FILE=/home/arm/disk.qcow2
PID_FILE=/tmp/run-qemu-arm64.pid
# MONITOR=/tmp/monitor.sock

qemu-system-aarch64 -M virt -cpu cortex-a57 -smp 8 -m 4096 \
  -bios QEMU_EFI.fd \
  --chardev socket,id=serial0,path=${CONSOLE_FILE},server,nowait \
    -serial chardev:serial0 \
  -nographic \
  -hda ${DISK_FILE} \
  -net nic,model=e1000 \
  -net user,hostfwd=tcp::2222-:22 > /dev/null &>1 & echo $! > ${PID_FILE}
  # -monitor unix:${MONITOR},server,nowait
```
```
$ sudo chmod +x /home/arm/run-qemu-arm64.sh
```
#### Настройка init.d
```
$ cat /etc/init.d/qemu-arm64
```
```
#! /bin/sh

### BEGIN INIT INFO
# Provides:          qemu-arm64
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: qemu VM for arm64...
### END INIT INFO

PATH=/bin:/usr/bin:/sbin:/usr/sbin:/usr/local/bin:/usr/local/sbin
DAEMON=/home/arm/run-qemu-arm64.sh
PIDFILE=/tmp/run-qemu-arm64.pid
# MONITOR=/tmp/monitor.sock

test -x $DAEMON || exit 0

. /lib/lsb/init-functions

case "$1" in
  start)
     log_daemon_msg "Starting VM for arm64" 
     start_daemon -p $PIDFILE $DAEMON
     log_end_msg $?
   ;;
  stop)
     log_daemon_msg "Stopping VM for arm64" 
     #echo 'system_powerdown' | socat UNIX-CONNECT:${MONITOR} stdio > /dev/null 2>&1
     #echo 'q' | socat UNIX-CONNECT:${MONITOR} stdio > /dev/null 2>&1
     killproc -p $PIDFILE $DAEMON
     PID=`ps x |grep qemu-system-aarch64 | head -1 | awk '{print $1}'`
     kill -9 $PID
     log_end_msg $?
   ;;
  force-reload|restart)
     $0 stop
     $0 start
   ;;
  status)
     status_of_proc -p $PIDFILE $DAEMON atd && exit 0 || exit $?
   ;;
 *)
   echo "Usage: /etc/init.d/qemu-arm64 {start|stop|restart|force-reload|status}" 
   exit 1
  ;;
esac

exit 0
```
```
$ sudo chmod +x /etc/init.d/qemu-arm64
```
#### Настройка systemctl
```
$ cat /etc/systemd/system/qemu-arm64.service
```
```
[Unit]
Description=qemu-arm64

[Service]
Type=forking

User=root
ExecStart=/etc/init.d/qemu-arm64 start
ExecStop=/etc/init.d/qemu-arm64 stop
ExecReload=/etc/init.d/qemu-arm64 restart
Restart=on-failure

[Install]
WantedBy=multi-user.target
```
```
$ sudo chmod +x /etc/systemd/system/qemu-arm64.service
```
#### Управление сервисом
```
$ sudo systemctl status qemu-arm64.service
$ sudo systemctl enable qemu-arm64.service
$ sudo systemctl start qemu-arm64.service
$ sudo systemctl stop qemu-arm64.service
```
#### Доступ к ВМ
```
ssh -P 2222 <username>@<IP-адрес>
# socat - UNIX-CONNECT:/tmp/console.sock
# socat - UNIX-CONNECT:/tmp/monitor.sock
```
