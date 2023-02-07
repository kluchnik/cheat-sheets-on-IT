# Примеры генерации сетевых пакетов

## Основные команды
```
# Прочитать pcap-файл
pcap = scapy.rdpcap('example.pcap')
# Отправить сетевой пакет
scapy.sendp(package, iface=iface)
# Сохранить сетевой пакет
scapy.wrpcap('example.pcap', package)
```

## eth/ip/udp/syslog

```
import scapy.all as scapy
import time

iface = 'ens3'
ip_src = '192.168.4.100'
port_src = 30000
ip_dst = '192.168.6.100'
port_dst = 514
msg = 'test messange'

eth = scapy.Ether()
eth.type = 0x0800

ip = scapy.IP()
ip.proto = 17
ip.src = ip_src
ip.dst = ip_dst

udp = scapy.UDP()
udp.sport = port_src
udp.dport = port_dst

syslog = '<0>{}{}'.format(time.strftime("%b %d %H:%M:%S "), msg)
data = scapy.Raw(load=syslog)

package = eth/ip/udp/data
package.show2()

scapy.sendp(package, iface=iface)
```

## eth/v802_1Q/ip/udp/data
```
import scapy.all as scapy

iface = 'ens3'
vlan = 333
ip_src = '192.168.4.100'
port_src = 30000
ip_dst = '192.168.6.100'
port_dst = 514
msg = 'test messange'

eth = scapy.Ether()
eth.type = 0x8100

v802_1Q = scapy.Dot1Q()
v802_1Q.vlan = vlan
v802_1Q.type = 0x0800

ip = scapy.IP()
ip.proto = 17
ip.src = ip_src
ip.dst = ip_dst

udp = scapy.UDP()
udp.sport = port_src
udp.dport = port_dst

data = scapy.Raw(load=msg)

package = eth/v802_1Q/ip/udp/data
package.show2()

scapy.sendp(package, iface=iface)
```

## eth/ip[security]/udp/data
```
import scapy.all as scapy

iface = 'ens3'
ip_src = '192.168.4.100'
port_src = 30000
ip_dst = '192.168.6.100'
port_dst = 514
msg = 'test messange'
ml_first = 0b00000011 # level=1 (0000 0001)
ml_second = 0b00001000 # category=2 (0000 0010)

eth = scapy.Ether()
eth.type = 0x0800

ip = scapy.IP()
ip.proto = 17
ip.src = ip_src
ip.dst = ip_dst

security = scapy.IPOption_Security()
security.copy_flag = 0b1
security.optclass = 0b00
security.option = 0b00010
security.length = 0b00000101
security.security = 0b10101011 * 256 + ml_first
security.compartment = ml_second * 256
ip.options = security

udp = scapy.UDP()
udp.sport = port_src
udp.dport = port_dst

data = scapy.Raw(load=msg)

package = eth/ip/udp/data
package.show2()

scapy.sendp(package, iface=iface)
```
