# Пример 1 (Подключение виртуальных машин мостом к физическому интерфейсу)

## Схема
```
┌────────────┐
│ ┌──────┐   |
│ | VPC1 0---0--->
| └──────┘ | │
│ ┌──────┐ | |
│ | VPC2 0 ┘ |
| └──────┘   │
└────────────┘
```
## Создаем группу мостов
```
brctl show
brctl addbr vdev
brctl show
```
## В виртуальную группу мостов добавляем физический интерфейс
```
brctl addif vdev eth0
brctl show
```
## Устанавливаем первую виртуальную машину
```
virt-install \
--name VPC1 \
--hvm --virt-type kvm --arch x86_64 \
--memory 1024 --vcpus 1 --boot hd \
--os-variant debian9 \
--disk path=/etc/libvirt/qemu/disk/pc1.qcow2,bus=sata \
--network bridge=dev \
--serial tcp,host=0.0.0.0:4001,mode=bind,protocol=telnet \
--console pty,target_type=serial --graphics none
```
```
brctl show
```
## Устанавливаем вторую виртуальную машину
```
virt-install \
--name VPC2 \
--hvm --virt-type kvm --arch x86_64 \
--memory 1024 --vcpus 1 --boot hd \
--os-variant debian9 \
--disk path=/etc/libvirt/qemu/disk/pc2.qcow2,bus=sata \
--network bridge=dev \
--serial tcp,host=0.0.0.0:4002,mode=bind,protocol=telnet \
--console pty,target_type=serial --graphics none
```
```
brctl show
```

## Поднимаем все интерфейсы
```
ip link set dev vdev up
ip link set dev eth0 up
```
