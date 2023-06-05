# Пример сборки тестового стенда
```
PC-1 <-> eth0
PC-2 <-> eth1
PC-3 <-> eth2
PC-4 <-> eth3
```
## PC-1
host:
```
brctl addbr vmbr1
brctl addif vmbr1 eth0
brctl show
ip link set dev eth0 up
ip link set dev vmbr1 up
```
```
qemu-system-x86_64 \
  -name pc-1 \
  -enable-kvm \
  -cpu kvm64 \
  -m 2048 \
  -hda /home/qemu/pc-1.qcow2 \
  -device e1000,netdev=net1,mac=00:50:DA:82:8A:01 \
  -netdev user,id=net1,hostfwd=tcp::2201-:22 \
  -device e1000,netdev=net2,mac=00:50:DA:82:8A:02 \
  -netdev bridge,id=net2,br=vmbr1 \
  -display none \
  -serial telnet::4001,server,nowait \
  -daemonize \
  -pidfile /tmp/pc-1.pid
```
```
telnet localhost 4001
```
vm:
```
modprobe 8021q
ip link add link ens4 VLAN101 type vlan id 101
ip link set dev ens4 up
ip link set dev VLAN101 up
ip a add 10.10.101.11/24 dev VLAN101
ip route add 10.10.102.0/24 via 10.10.101.1
ip route add 10.10.103.0/24 via 10.10.101.1
ip route add 10.10.104.0/24 via 10.10.101.1
```
## PC-2
host:
```
brctl addbr vmbr2
brctl addif vmbr2 eth1
brctl show
ip link set dev eth1 up
ip link set dev vmbr2 up
```
```
qemu-system-x86_64 \
  -name pc-2 \
  -enable-kvm \
  -cpu kvm64 \
  -m 2048 \
  -hda /home/qemu/pc-2.qcow2 \
  -device e1000,netdev=net1,mac=00:50:DA:82:8B:01 \
  -netdev user,id=net1,hostfwd=tcp::2202-:22 \
  -device e1000,netdev=net2,mac=00:50:DA:82:8B:02 \
  -netdev bridge,id=net2,br=vmbr2 \
  -display none \
  -serial telnet::4002,server,nowait \
  -daemonize \
  -pidfile /tmp/pc-2.pid
```
```
telnet localhost 4002
```
vm:
```
modprobe 8021q
ip link add link ens4 VLAN102 type vlan id 102
ip link set dev ens4 up
ip link set dev VLAN102 up
ip a add 10.10.102.11/24 dev VLAN102
ip route add 10.10.101.0/24 via 10.10.102.1
ip route add 10.10.103.0/24 via 10.10.102.1
ip route add 10.10.104.0/24 via 10.10.102.1
```
## PC-3
host:
```
brctl addbr vmbr3
brctl addif vmbr3 eth2
brctl show
ip link set dev eth2 up
ip link set dev vmbr3 up
```
```
qemu-system-x86_64 \
  -name pc-3 \
  -enable-kvm \
  -cpu kvm64 \
  -m 2048 \
  -hda /home/qemu/pc-3.qcow2 \
  -device e1000,netdev=net1,mac=00:50:DA:82:8C:01 \
  -netdev user,id=net1,hostfwd=tcp::2203-:22 \
  -device e1000,netdev=net2,mac=00:50:DA:82:8C:02 \
  -netdev bridge,id=net2,br=vmbr3 \
  -display none \
  -serial telnet::4003,server,nowait \
  -daemonize \
  -pidfile /tmp/pc-3.pid
```
```
telnet localhost 4003
```
vm:
```
modprobe 8021q
ip link add link ens4 VLAN103 type vlan id 103
ip link set dev ens4 up
ip link set dev VLAN103 up
ip a add 10.10.103.11/24 dev VLAN102
ip route add 10.10.101.0/24 via 10.10.103.1
ip route add 10.10.102.0/24 via 10.10.103.1
ip route add 10.10.104.0/24 via 10.10.103.1
```
## PC-4
host:
```
brctl addbr vmbr4
brctl addif vmbr4 eth3
brctl show
ip link set dev eth3 up
ip link set dev vmbr4 up
```
```
qemu-system-x86_64 \
  -name pc-4 \
  -enable-kvm \
  -cpu kvm64 \
  -m 2048 \
  -hda /home/qemu/pc-4.qcow2 \
  -device e1000,netdev=net1,mac=00:50:DA:82:8D:01 \
  -netdev user,id=net1,hostfwd=tcp::2204-:22 \
  -device e1000,netdev=net2,mac=00:50:DA:82:8D:02 \
  -netdev bridge,id=net2,br=vmbr4 \
  -display none \
  -serial telnet::4004,server,nowait \
  -daemonize \
  -pidfile /tmp/pc-4.pid
```
```
telnet localhost 4004
```
vm:
```
modprobe 8021q
ip link add link ens4 VLAN104 type vlan id 104
ip link set dev ens4 up
ip link set dev VLAN104 up
ip a add 10.10.104.11/24 dev VLAN104
ip route add 10.10.101.0/24 via 10.10.104.1
ip route add 10.10.102.0/24 via 10.10.104.1
ip route add 10.10.103.0/24 via 10.10.104.1
```
