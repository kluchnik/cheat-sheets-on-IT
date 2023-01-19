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
  -boot once=c \
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

## Видеовыход через VNC
```
...
  -vga vmware \
  -display none \
  -vnc 0.0.0.0:2001
```

## Подключение monitor для управления ВМ
```
$ mkfifo vm_monitor.in
$ mkfifo vm_monitor.out

...
  -monitor pipe:vm_monitor
```
