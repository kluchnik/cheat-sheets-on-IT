# iperf3

## TCP 

### Server
```
IFACE='eth0'
L_IP='192.168.2.100'
L_PORT='1000'

ip a add ${L_IP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

# iperf3 -s -p ${L_PORT} -D; ps aux | grep iperf3 | grep -v grep
iperf3 -s -p ${L_PORT}
```

### Client
```
IFACE='eth0'
L_IP='192.168.1.100'
D_IP='192.168.2.100'
D_PORT='1000'

ip a add ${L_IP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

iperf3 --client ${D_IP} --port ${D_PORT} -T s1 --time 60 --interval 5 --set-mss 1460
iperf3 --client ${D_IP} --port ${D_PORT} -T s1 --time 60 --interval 5 --set-mss 1460 --reverse
```

## UDP 

### Server
```
IFACE='eth0'
L_IP='192.168.2.100'
L_PORT='1000'

ip a add ${L_IP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

# iperf3 -s -p ${L_PORT} -D; ps aux | grep iperf3 | grep -v grep
iperf3 -s -p ${L_PORT}
```

### Client
```
IFACE='eth0'
L_IP='192.168.1.100'
D_IP='192.168.2.100'
D_PORT='1000'

ip a add ${L_IP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

iperf3 -u --client ${D_IP} --port ${D_PORT} -T s1 -b 1G -l 1448 --time 60 --interval 5
iperf3 -u --client ${D_IP} --port ${D_PORT} -T s1 -b 1G -l 1448 --time 60 --interval 5 --reverse
```
