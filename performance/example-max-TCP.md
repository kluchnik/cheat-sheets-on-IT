# Проверка макисамального количества одновременных TCP-сессий

## Утилиты
```
/usr/bin/tcploadechoserver
/usr/bin/tcploadechoclient
```

## Схема
```
TCP-client <-> TCP-server-1
               TCP-server-2
               ...
```

## Настройка TCP-сервера
При данных настройках на одной Linux (Debian) удалось поддерживать только 50 000 одновременных TCP-сессий.  

### Сервер-1
```
IFACE='eth0'
IP='192.168.2.101'
PORT='1101'

ulimit -s 20000500
ulimit -n 1048576
echo '20000500' > /proc/sys/fs/nr_open
echo '10000 65535' > /proc/sys/net/ipv4/ip_local_port_range
echo '1' > /proc/sys/net/ipv4/tcp_fin_timeout
#echo '383865 511820 2303190' > /proc/sys/net/ipv4/tcp_mem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_rmem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_wmem
echo '0' > /proc/sys/net/ipv4/tcp_moderate_rcvbuf

ip a add ${IP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

/usr/bin/tcploadechoserver -i ${IP} -p ${PORT} &>/dev/null &

watch -n 1 ss -s
```

### Сервер-2
```
IFACE='eth0'
IP='192.168.2.102'
PORT='1102'

ulimit -s 20000500
ulimit -n 1048576
echo '20000500' > /proc/sys/fs/nr_open
echo '10000 65535' > /proc/sys/net/ipv4/ip_local_port_range
echo '1' > /proc/sys/net/ipv4/tcp_fin_timeout
#echo '383865 511820 2303190' > /proc/sys/net/ipv4/tcp_mem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_rmem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_wmem
echo '0' > /proc/sys/net/ipv4/tcp_moderate_rcvbuf

ip a add ${IP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

/usr/bin/tcploadechoserver -i ${IP} -p ${PORT} &>/dev/null &

watch -n 1 ss -s
```

## Запуск генерации TCP-соединений
Клиентов TCP-сессий можно запускать на одной Linux (Debian) машине.  

### Клиент-1
```
IFACE='eth0'
LIP='192.168.1.101'
DIP='192.168.2.101'
DPORT='1101'

ulimit -s 20000500
ulimit -n 1048576
echo '20000500' > /proc/sys/fs/nr_open
echo '10000 65535' > /proc/sys/net/ipv4/ip_local_port_range
echo '1' > /proc/sys/net/ipv4/tcp_fin_timeout
#echo '383865 511820 2303190' > /proc/sys/net/ipv4/tcp_mem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_rmem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_wmem
echo '0' > /proc/sys/net/ipv4/tcp_moderate_rcvbuf

ip a add ${LIP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

/usr/bin/tcploadechoclient -i ${LIP} -D ${DIP} -p ${DPORT} -n 50000 -r 1000 &>/dev/null &
```

### Клиент-1
```
IFACE='eth0'
LIP='192.168.1.102'
DIP='192.168.2.102'
DPORT='1102'

ulimit -s 20000500
ulimit -n 1048576
echo '20000500' > /proc/sys/fs/nr_open
echo '10000 65535' > /proc/sys/net/ipv4/ip_local_port_range
echo '1' > /proc/sys/net/ipv4/tcp_fin_timeout
#echo '383865 511820 2303190' > /proc/sys/net/ipv4/tcp_mem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_rmem
#echo '1024 4096 16384' > /proc/sys/net/ipv4/tcp_wmem
echo '0' > /proc/sys/net/ipv4/tcp_moderate_rcvbuf

ip a add ${LIP}/24 dev ${IFACE}
ip link set dev ${IFACE} up

/usr/bin/tcploadechoclient -i ${LIP} -D ${DIP} -p ${DPORT} -n 50000 -r 1000 &>/dev/null &
```
