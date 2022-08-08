# Монтирование/размонтирование виртуального диска qcow2

## Монтирование
```
$ DISK_PATH='/etc/libvirt/qemu/disk/example.qcow2'
$ DISK_DEV='/dev/nbd0'
$ DISK_MOUNT='/mnt/'

$ modprobe nbd max_part=8
$ qemu-nbd --connect=${DISK_DEV} ${DISK_PATH}
$ fdisk /dev/nbd0 -l
$ partx -a /dev/nbd0

$ mount /dev/nbd0p1 ${DISK_MOUNT}
```
## Размонтирование
```
$ DISK_DEV='/dev/nbd0'
$ DISK_MOUNT='/mnt/'

$ umount ${DISK_MOUNT}
$ qemu-nbd --disconnect ${DISK_DEV}
$ rmmod nbd
```
