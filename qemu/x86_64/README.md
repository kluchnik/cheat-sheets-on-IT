# Виртуализация x86_64 через qemu

## Пример запуска:
```
$ ISO_FILE='/tmp/test.iso'
$ VM_DISK='/home/user/qemu/disk.qcow2'
$ PID_FILE='/var/run/vm_test.pid'

$ qemu-system-x86_64 \
  -name vm_test \
  -enable-kvm \
  -cpu kvm64 \
  -m 2048 \
  -boot once=d \
  -cdrom ${ISO_FILE} \
  -hda ${VM_DISK} \
  -device e1000,netdev=net1,mac=00:50:DA:82:8A:01 \
  -netdev user,id=net1,net=192.168.1.0/24,hostfwd=tcp::8443-192.168.1.1:8443 \
  -device e1000,netdev=net2,mac=00:50:DA:82:8A:02 \
  -netdev user,id=net2 \
  -device e1000,netdev=net3,mac=00:50:DA:82:8A:03 \
  -netdev user,id=net3 \
  -device e1000,netdev=net4,mac=00:50:DA:82:8A:04 \
  -netdev user,id=net4 \
  -device e1000,netdev=net5,mac=00:50:DA:82:8A:05 \
  -netdev user,id=net5 \
  -device e1000,netdev=net6,mac=00:50:DA:82:8A:06 \
  -netdev user,id=net6 \
  -display none \
  -serial telnet::4001,server,nowait \
  -daemonize \
  -pidfile ${PID_FILE}
```
### Первая загрузка с CD-ROM:
```
...
  -boot once=d
```
### Загрузка с HDA:
```
...
  -boot c
```

### Сеть через проброс порта
```
...
  -device e1000,netdev=net1,mac=00:50:DA:82:8A:01 \
  -netdev user,id=net1,net=192.168.1.0/24,hostfwd=tcp::8443-192.168.1.1:8443 \
```
Способ перенаправления входящих внешних соединений на localhost
```
socat tcp-listen:8001,reuseaddr,fork tcp:localhost:8443
```
### Сеть: автоматическое создание tap интерфейса
```
$ cat /home/user/qemu/ifup

#!/bin/bash
ip link set dev tap1 up
ip a change dev tap1 192.168.1.100/24
exit 0
```
```
...
  -device e1000,netdev=net1,mac=00:50:DA:82:8A:01 \
  -netdev tap,id=net1,ifname=tap1,script=/home/user/qemu/ifup,downscript=no \
```
Настройка перенаправления через iptables
```
$ iptables -t nat -A PREROUTING -d 0.0.0.0/0 -p tcp --dport 8001 -j DNAT --to-destination 192.168.1.1:8443
$ iptables -t nat -A POSTROUTING -p tcp --dport 8443 -j MASQUERADE
```

### Сеть: подключение VM к bridge
```
$ brctl show
$ brctl add vmbr1

$ ip link set dev vmbr1 up
$ ip a change dev vmbr1 192.168.1.100/24
```
```
$ USER='user'
$ echo "allow all" | sudo tee /etc/qemu/${USER}.conf
$ echo "include /etc/qemu/${USER}.conf" | sudo tee --append /etc/qemu/bridge.conf
$ sudo chown root:${USER} /etc/qemu/${USER}.conf
$ sudo chmod 640 /etc/qemu/${USER}.conf
```
```
...
  -device e1000,netdev=net1,mac=00:50:DA:82:8A:01 \
  -netdev bridge,id=net1,br=vmbr1 \
```
Настройка перенаправления через iptables
```
$ iptables -t nat -A PREROUTING -d 0.0.0.0/0 -p tcp --dport 8001 -j DNAT --to-destination 192.168.1.1:8443
$ iptables -t nat -A POSTROUTING -p tcp --dport 8443 -j MASQUERADE
```

## Видеовыход через VNC
```
...
  -vga vmware \
  -display none \
  -vnc 0.0.0.0:2001
```

## Подключение monitor для управления ВМ
```
...
  -monitor stdio
```
или
```
$ mkfifo vm_monitor.in
$ mkfifo vm_monitor.out

...
  -monitor pipe:vm_monitor
 
$ echo 'help' > vm_monitor.in
$ cat vm_monitor.out | less

$ echo 'quit' > vm_monitor.in
```
