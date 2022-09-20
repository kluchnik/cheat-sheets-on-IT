# docker

## Пример создания контейнера

1. Скачать образ debian нужной версии с docker.hub:  
```
docker pull debian:9.0
docker images
```

2. Запустить контейнер:  
```
docker run --name debian -it -d debian:9.0
docker ps
```

3. Подключится к контейнеру:
```
docker exec -it debian /bin/bash
cat /etc/issue
apt list –installed
```

## Пример ping

1. Создаем контейнер для команды ping  
```
#: mkdir ping
#: cd ./ping

#: nano Dockerfile
> FROM debian
> ENTRYPOINT ["/bin/ping"]

#: docker build -t qa/ping .
```

2. Проверить наличие контейнеров  
```
docker images
```

3. Создаем сетевые адаптеры  
```
docker network ls

docker network create -d macvlan \
--subnet=192.168.1.0/24 \
--gateway=192.168.1.1 \
-o parent=enp5s0 \
lan-1-enp5s0

docker network create -d macvlan \
--subnet=192.168.2.0/24 \
--gateway=192.168.2.1 \
-o parent=enp6s0 \
lan-2-enp6s0
```
4. Проверяем наличие сети  
```
docker network ls
docker network inspect lan-1-enp5s0
docker network inspect lan-2-enp6s0
```
5. Включаем неразборчивый режим  
```
ip link set dev enp5s0 up
ip link set dev enp6s0 up

ip link set dev enp5s0 promisc on
ip link set dev enp6s0 promisc on
```
6. Запускаем контейнер  
```
mkdir /tmp/tcpdump
docker run -d --net=lan-2-enp6s0 --ip=192.168.2.2 --name=tcpdump -v /tmp/tcpdump:/data corfr/tcpdump -i any -w /data/dump.pcap "icmp"
docker run -it --rm --net=lan-1-enp5s0 --ip=192.168.1.2 --name=ping qa/ping -c 1 192.168.2.2
```
7. Остановка контейнера (pcap-файл будет записан, только после остановки)  
```
docker ps -a
docker stop tcpdump
docker rm tcpdump
docker ps -a
```
8. Проверка результатов  
```
tcpdump -ttttnnr /tmp/tcpdump/dump.pcap
```

## Пример iperf3
```
docker pull networkstatic/iperf3

docker network ls

echo 1 > /proc/sys/net/ipv4/ip_forward
/usr/sbin/sysctl -p

docker network create -d macvlan \
--subnet=192.168.1.0/24 \
--gateway=192.168.1.1 \
-o parent=enp2s0f0 \
lan-1

docker network create -d macvlan \
--subnet=192.168.2.0/24 \
--gateway=192.168.2.1 \
-o parent=enp2s0f1 \
lan-2


docker run -d --rm --net=lan-2 --ip=192.168.2.101 --name=server-1 networkstatic/iperf3 -s -p 1001
docker run -d --rm --net=lan-2 --ip=192.168.2.102 --name=server-2 networkstatic/iperf3 -s -p 1002
docker run -d --rm --net=lan-2 --ip=192.168.2.103 --name=server-3 networkstatic/iperf3 -s -p 1003
docker run -d --rm --net=lan-2 --ip=192.168.2.104 --name=server-4 networkstatic/iperf3 -s -p 1004
docker run -d --rm --net=lan-2 --ip=192.168.2.105 --name=server-5 networkstatic/iperf3 -s -p 1005
docker run -d --rm --net=lan-2 --ip=192.168.2.106 --name=server-6 networkstatic/iperf3 -s -p 1006
docker run -d --rm --net=lan-2 --ip=192.168.2.107 --name=server-7 networkstatic/iperf3 -s -p 1007
docker run -d --rm --net=lan-2 --ip=192.168.2.108 --name=server-8 networkstatic/iperf3 -s -p 1008
docker run -d --rm --net=lan-2 --ip=192.168.2.109 --name=server-9 networkstatic/iperf3 -s -p 1009
docker run -d --rm --net=lan-2 --ip=192.168.2.110 --name=server-10 networkstatic/iperf3 -s -p 1010
docker run -d --rm --net=lan-2 --ip=192.168.2.111 --name=server-11 networkstatic/iperf3 -s -p 1011
docker run -d --rm --net=lan-2 --ip=192.168.2.112 --name=server-12 networkstatic/iperf3 -s -p 1012
docker run -d --rm --net=lan-2 --ip=192.168.2.113 --name=server-13 networkstatic/iperf3 -s -p 1013
docker run -d --rm --net=lan-2 --ip=192.168.2.114 --name=server-14 networkstatic/iperf3 -s -p 1014
docker run -d --rm --net=lan-2 --ip=192.168.2.115 --name=server-15 networkstatic/iperf3 -s -p 1015
docker run -d --rm --net=lan-2 --ip=192.168.2.116 --name=server-16 networkstatic/iperf3 -s -p 1016
docker run -d --rm --net=lan-2 --ip=192.168.2.117 --name=server-17 networkstatic/iperf3 -s -p 1017
docker run -d --rm --net=lan-2 --ip=192.168.2.118 --name=server-18 networkstatic/iperf3 -s -p 1018
docker run -d --rm --net=lan-2 --ip=192.168.2.119 --name=server-19 networkstatic/iperf3 -s -p 1019
docker run -d --rm --net=lan-2 --ip=192.168.2.120 --name=server-20 networkstatic/iperf3 -s -p 1020
echo > /tmp/iperf.log
docker run -t --rm --net=lan-1 --ip=192.168.1.101 --name=client-1 networkstatic/iperf3 -c 192.168.2.101 -p 1001 -T s1 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.102 --name=client-2 networkstatic/iperf3 -c 192.168.2.102 -p 1002 -T s2 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.103 --name=client-3 networkstatic/iperf3 -c 192.168.2.103 -p 1003 -T s3 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.104 --name=client-4 networkstatic/iperf3 -c 192.168.2.104 -p 1004 -T s4 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.105 --name=client-5 networkstatic/iperf3 -c 192.168.2.105 -p 1005 -T s5 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.106 --name=client-6 networkstatic/iperf3 -c 192.168.2.106 -p 1006 -T s6 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.107 --name=client-7 networkstatic/iperf3 -c 192.168.2.107 -p 1007 -T s7 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.108 --name=client-8 networkstatic/iperf3 -c 192.168.2.108 -p 1008 -T s8 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.109 --name=client-9 networkstatic/iperf3 -c 192.168.2.109 -p 1009 -T s9 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.110 --name=client-10 networkstatic/iperf3 -c 192.168.2.110 -p 1010 -T s10 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.111 --name=client-11 networkstatic/iperf3 -c 192.168.2.111 -p 1011 -T s11 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.112 --name=client-12 networkstatic/iperf3 -c 192.168.2.112 -p 1012 -T s12 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.113 --name=client-13 networkstatic/iperf3 -c 192.168.2.113 -p 1013 -T s13 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.114 --name=client-14 networkstatic/iperf3 -c 192.168.2.114 -p 1014 -T s14 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.115 --name=client-15 networkstatic/iperf3 -c 192.168.2.115 -p 1015 -T s15 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.116 --name=client-16 networkstatic/iperf3 -c 192.168.2.116 -p 1016 -T s16 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.117 --name=client-17 networkstatic/iperf3 -c 192.168.2.117 -p 1017 -T s17 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.118 --name=client-18 networkstatic/iperf3 -c 192.168.2.118 -p 1018 -T s18 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.119 --name=client-19 networkstatic/iperf3 -c 192.168.2.119 -p 1019 -T s19 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &
docker run -t --rm --net=lan-1 --ip=192.168.1.120 --name=client-20 networkstatic/iperf3 -c 192.168.2.120 -p 1020 -T s20 --time 60 --interval 60 -l 1460 >> /tmp/iperf.log &

```

## Пример ftp

```
mkdir /tmp/ftp1

docker run -d -v /tmp/ftp1:/home/vsftpd \
--net=lan-1-enp5s0 --ip=192.168.1.2 \ьл
-e FTP_USER=user \
-e FTP_PASS=user \
-e PASV_ADDRESS=192.168.1.2 \
--name ftp1 \
--restart=always bogem/ftp
```
