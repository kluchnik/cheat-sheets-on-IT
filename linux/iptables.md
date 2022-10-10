# IPTables


## Пример генератора трафика для проверки коммутатора L2

### Схема
```
┌───────────────────────────┐        ┌────────┐
| PC-client (192.168.1.100) | -40G-> │   L2   │
| PC-server (192.168.2.100) | <-40G- |        |
└───────────────────────────┘        └────────┘
```

### Сетевая настройка
```
declare -A NET

NET[0,0]=enp2s0f0             # имя интерфейса
NET[0,1]=192.168.1.100        # IP-адрес физического интерфейса
NET[0,2]=192.168.2.200        # Виртуальный IP-адрес
NET[0,3]=00:90:0b:8e:b4:bc    # MAC-адрес физического интерфйеса
NET[0,4]=192.168.1.200        # Удаленный IP-адрес
NET[0,5]=00:90:0b:8e:b4:bd    # Удаленный MAC-адрес
NET[1,0]=enp2s0f1
NET[1,1]=192.168.2.100
NET[1,2]=192.168.1.200
NET[1,3]=00:90:0b:8e:b4:bd
NET[1,4]=192.168.2.200
NET[1,5]=00:90:0b:8e:b4:bc
```
```
for i in {0..1}; do
  DEV_L=${NET[${i},0]}
  IP_L=${NET[${i},1]}
  echo ${DEV_L} - ${IP_L}
  ip addr add ${IP_L}/24 dev ${DEV_L}
  ip link set dev ${DEV_L} mtu 1500
  ip link set ${DEV_L} up
  ip addr show dev ${DEV_L}
done
```
### Создание виртуальных интерфейсов
```
┌───────────────────────────────────────────┐        ┌────────┐
| PC-client (192.168.1.100) (192.168.2.200) | -40G-> │   L2   │
| PC-server (192.168.2.100) (192.168.1.200) | <-40G- |        |
└───────────────────────────────────────────┘        └────────┘
```
#### Добавление адресов в ARP-таблицу
```
for i in {0..1}; do
  DEV_L=${NET[${i},0]}
  IP_R=${NET[${i},4]}
  MAC_R=${NET[${i},5]}
  ip neigh add ${IP_R} lladdr ${MAC_R} dev ${DEV_L} nud permanent
  ip neig show dev ${DEV_L} | grep ${IP_R}
done
```

### Создание правил IPTables
```
# CLEAR
/usr/sbin/iptables -t nat -F

for i in {0..1}; do
  DEV_L=${NET[${i},0]}
  IP_L=${NET[${i},1]}
  IP_V=${NET[${i},2]}
  # OUTPUT
  /usr/sbin/iptables -t nat -A POSTROUTING -s ${IP_L}/32 -j SNAT --to-source ${IP_V}
  # INPUT
  /usr/sbin/iptables -t nat -A PREROUTING -i ${DEV_L} -d ${IP_V} -j DNAT --to-destination ${IP_L}
```

### Проверка работы
```
ping -c 1 192.168.1.200
ping -c 1 192.168.2.200

iperf3 -s -p 1000 -D
iperf3 --client 192.168.1.200 --port 1000 -T s1 --time 60 --interval 60 --set-mss 1460
```
