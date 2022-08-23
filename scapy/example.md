# Примеры генерации сетевых пакетов

## eth/ip/udp/syslog

```
import scapy.all as scapy
import time

iface = 'ens3'
ip_src = '192.168.4.100'
port_src = 30000
ip_dst = '192.168.4.1'
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
