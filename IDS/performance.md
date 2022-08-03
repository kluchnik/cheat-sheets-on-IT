# Создание нагрузки на IDS

## Нагрузка на сигнатурный анализ
Правило:
```
alert udp any any -> any any (msg:"test rule UDP all"; \
sid:10000001; rev:1; classtype:not suspicious; priority:1;)

alert udp 192.168.4.100 any -> 192.168.6.100 any (msg:"test rule UDP 65000"; content:"test_UDP"; \
sid:10000002; rev:1; classtype:not suspicious; priority:1;)

alert udp 192.168.4.100 any -> any any (msg:"test rule UDP 65000"; content:"test_UDP"; \
sid:10000003; rev:1; classtype:not suspicious; priority:1;)

alert udp 192.168.4.100 any -> 192.168.6.100 any (msg:"test rule UDP one session"; content:"|07d0|"; \
sid:10000004; rev:1; classtype:not suspicious; priority:1;)
```
Генерация трафика на один хост, но на разные порты:
```
ip_address='192.168.6.100'
timeout=0
for step in {1..100}; do
  let "pkg=65000*$step";
  echo -e "$(date '+%Y-%m-%d %H:%M:%S') step: ${step}\t ${pkg}";
  for port in {1..65000}; do
    echo -n "test_UDP" > /dev/udp/${ip_address}/${port}
  done
  sleep ${timeout}
done
```
Генерация трафика на разные хосты и порты:
```
net_address='192.168.6.0'
timeout=0
for item in {1..254}; do
  ip_address=${net_address%%0}${item}
  echo -e "$(date '+%Y-%m-%d %H:%M:%S') ip: ${ip_address}";
  for port in {1..65000}; do
    echo -n "test_UDP" > /dev/udp/${ip_address}/${port}
  done
  sleep ${timeout}
done
```
Генерация трафика в одной сессии:
```
ip_address='192.168.6.100'
iperf3 --client ${ip_address} -u --port 2000 -b 1G -l 500 --time 1 --interval 1
```
