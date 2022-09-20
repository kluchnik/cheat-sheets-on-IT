# Измерение поизводительности

## Схема  

PC-QA -> PC-TARGET
      <- PC-TARGET  

## Настройка сети  

PC-TARGET  
```
ip addr add 192.168.1.1/24 dev enp2s0f0
ip link set dev enp2s0f0 mtu 1500
ip link set enp2s0f0 up
ip addr show dev enp2s0f0

#ip addr add 192.168.2.1/24 dev enp2s0f1
#ip link set dev enp2s0f1 mtu 1500
ip link set enp2s0f1 up
ip addr show dev enp2s0f1
```

PC-QA  
```
#ip addr add 192.168.1.100/24 dev enp2s0f0
#ip link set dev enp2s0f0 mtu 1500
ip link set enp2s0f0 up
ip addr show dev enp2s0f0

#ip addr add 192.168.2.100/24 dev enp2s0f1
#ip link set dev enp2s0f1 mtu 1500
ip link set enp2s0f1 up
ip addr show dev enp2s0f1
```

## Генерация трафика при помощи Docker  

PC-TARGET  
```
echo 1 > /proc/sys/net/ipv4/ip_forward
/usr/sbin/sysctl -p
```

PC-QA  
```
echo 1 > /proc/sys/net/ipv4/ip_forward
/usr/sbin/sysctl -p

docker pull networkstatic/iperf3

docker network ls

docker network create -d macvlan \
--subnet=192.168.1.0/24 \
--gateway=192.168.1.1 \
-o parent=enp2s0f0 \
lan-1

docker network create -d macvlan \
--subnet=192.168.2.0/24 \
--gateway=192.168.2.1 \
-o parent=enp2s0f0 \
lan-1-2

docker network create -d macvlan \
--subnet=192.168.2.0/24 \
--gateway=192.168.2.1 \
-o parent=enp2s0f1 \
lan-2
```

PC-QA  
```
docker run -d --rm --net=lan-2 --ip=192.168.2.101 --name=server-1 networkstatic/iperf3 -s -p 1001
docker run -t --rm --net=lan-1 --ip=192.168.1.101 --name=client-1 networkstatic/iperf3 -c 192.168.2.101 -p 1001 -T s1 --time 10 --interval 1 -l 1460
```

```
docker run -d --rm --net=lan-2 --ip=192.168.2.101 --name=server-1 networkstatic/iperf3 -s -p 1001
docker run -d --rm --net=lan-2 --ip=192.168.2.102 --name=server-2 networkstatic/iperf3 -s -p 1002
docker run -d --rm --net=lan-2 --ip=192.168.2.103 --name=server-3 networkstatic/iperf3 -s -p 1003
docker run -d --rm --net=lan-2 --ip=192.168.2.104 --name=server-4 networkstatic/iperf3 -s -p 1004
docker run -d --rm --net=lan-2 --ip=192.168.2.105 --name=server-5 networkstatic/iperf3 -s -p 1005
docker run -d --rm --net=lan-2 --ip=192.168.2.106 --name=server-6 networkstatic/iperf3 -s -p 1006

echo > /tmp/iperf.log
docker run -t --rm --net=lan-1 --ip=192.168.1.101 --name=client-1 networkstatic/iperf3 -c 192.168.2.101 -p 1001 -T s1 --time 10 --interval 10 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.102 --name=client-2 networkstatic/iperf3 -c 192.168.2.102 -p 1002 -T s2 --time 10 --interval 10 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.103 --name=client-3 networkstatic/iperf3 -c 192.168.2.103 -p 1003 -T s3 --time 10 --interval 10 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.104 --name=client-4 networkstatic/iperf3 -c 192.168.2.104 -p 1004 -T s4 --time 10 --interval 10 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.105 --name=client-5 networkstatic/iperf3 -c 192.168.2.105 -p 1005 -T s5 --time 10 --interval 10 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.106 --name=client-6 networkstatic/iperf3 -c 192.168.2.106 -p 1006 -T s6 --time 10 --interval 10 -l 1460 >> /tmp/iperf.log &

```
