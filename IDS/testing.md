# Проверка срабатывания IDS

## Проверка срабатывания IDS по сигнатуре
Правила:  
```
alert ip any any -> any any (msg:"test rule 1"; content:"test_msg"; sid:10000001)
alert ip any any -> any any (msg:"test rule 2"; pcre:"/test_\d+/m"; sid:10000002)
```
Проверка:  
```
ip_address='192.168.6.100'
hping3 ${ip_address} --rawip --ipproto 254 --count 1 --sign " test_msg" --print
hping3 ${ip_address} --rawip --ipproto 254 --count 1 --sign " test_12345678" --print
```
## Проверка срабатывания IDS по ip-адресу
Правило:  
```
alert ip 192.168.4.100 any -> 192.168.6.100 any (msg:"test rule 3"; content:"test_msg"; sid:10000003)
```
Проверка:  
```
ip_address='192.168.6.100'
hping3 ${ip_address} --rawip --ipproto 254 --count 1 --sign " test_msg" --print
```
## Проверка срабатывания IDS по протоколу
Правила:  
```
alert ip any any -> any any (msg:"test rule 4 proto ip"; content:"test_msg"; sid:10000004)
alert icmp any any -> any any (msg:"test rule 5 proto icmp"; content:"test_msg"; sid:10000005)
alert udp any any -> any any (msg:"test rule 6 proto udp"; content:"test_msg"; sid:10000006)
alert tcp any any -> any any (msg:"test rule 7 proto tcp"; content:"test_msg"; sid:10000007)
```
Подготовка udp-сервера для ответа на запрос:
```
port_udp=2000
echo "udp response" > /tmp/udp_msg.txt
nc -ulp ${port_udp} < /tmp/udp_msg.txt > /dev/null 2>&1 &
```
Подготовка tcp-сервера для ответа на запрос:
```
port_tcp=3000
echo "tcp response" > /tmp/tcp_msg.txt
nc -lp ${port_tcp} < /tmp/tcp_msg.txt > /dev/null 2>&1 &
```
Проверка:
```
ip_address='192.168.6.100'
port_udp=2000
port_tcp=3000
hping3 ${ip_address} --rawip --ipproto 254 --count 1 --sign " test_msg" --print
echo "test_msg" > /dev/udp/${ip_address}/${port_udp}
timeout 1 bash -c "echo test_msg > /dev/tcp/${ip_address}/${port_udp}"
```
## Проверка срабатывания IDS по порту
Правила:  
```
alert udp any any -> any 2000 (msg:"test rule 8 proto udp port 2000"; content:"test_msg"; sid:10000008)
alert tcp any any -> any 3000 (msg:"test rule 9 proto tcp port 3000"; content:"test_msg"; sid:10000009)
```
Подготовка udp-сервера для ответа на запрос:
```
port_udp=2000
echo "udp response" > /tmp/udp_msg.txt
nc -ulp ${port_udp} < /tmp/udp_msg.txt > /dev/null 2>&1 &
```
Подготовка tcp-сервера для ответа на запрос:
```
port_tcp=3000
echo "tcp response" > /tmp/tcp_msg.txt
nc -lp ${port_tcp} < /tmp/tcp_msg.txt > /dev/null 2>&1 &
```
Проверка:  
```
ip_address='192.168.6.100'
port_udp=2000
port_tcp=3000
echo "test_msg" > /dev/udp/${ip_address}/${port_udp}
timeout 1 bash -c "echo test_msg > /dev/tcp/${ip_address}/${port_udp}"
```
## Проверка срабатывания IDS по правилу из стандартного набора malware-cnc.rules (sid: 19769)
Правило:  
```
alert icmp any any -> any any (msg:"MALWARE-CNC Win.Trojan.Yoddos outbound indicator"; \
itype:8; icode:0; content:"YYYYYYYYYYYYYYYYYYYYYYYYYYYY"; \
metadata:policy balanced-ips drop, policy security-ips drop; \
reference:url,www.virustotal.com/en/file/a7f97ed5c064b038279dbd02554c7e555d97f67b601b94bfc556a50a41dae137/analysis/; \
classtype:trojan-activity; sid:19769; rev:7;)
```
Проверка:  
```
ip_address='192.168.6.100'
hping3 ${ip_address} --icmp --icmptype 8 --icmpcode 0 --count 1 --print --sign 'YYYYYYYYYYYYYYYYYYYYYYYYYYYY'
```
## Проверка срабатывания IDS по правилу из стандартного набора malware-cnc.rules (sid: 24088)
Правило:  
```
alert icmp any any -> any any (msg:"MALWARE-CNC Win.Trojan.Bledoor TCP tunnel in ICMP"; \
content:"|08 00 45|"; depth:3; offset:12; content:"|00 00|"; within:2; distance:5; \
content:"|06|"; within:1; distance:1; content:"|00 00 00 00|"; within:4; distance:18; \
metadata:impact_flag red, policy balanced-ips drop, policy security-ips drop; \
reference:url,www.virustotal.com/file/7dde04222d364b6becbc2f36d30ce59a5ec25bf4c3577d0979bb1b874c06d5dc/analysis/; \
classtype:trojan-activity; sid:24088; rev:5;)
```
Проверка:  
```
echo -n -e '\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' > /tmp/sig_24088.txt
echo -n -e '\x08\x00\x45\x00\x00\x00\x00\x00\x00\x00\x00\x06' >> /tmp/sig_24088.txt

ip_address='192.168.6.100'
hping3 ${ip_address} --icmp --data 100 --count 1 --print --file /tmp/sig_24088.txt
```
## Проверка срабатывания IDS по правилу из стандартного набора server-webapp.rules (sid: 28289)
Правило:  
```
alert udp any any -> any 7329 (msg:"SERVER-WEBAPP Tenda W302R root remote code execution attempt"; \
flow:to_server; content:"w302r_mfg|00|x"; fast_pattern:only; \
metadata:policy balanced-ips drop, policy max-detect-ips drop, policy security-ips drop, service http; \
reference:url,www.medialinkproducts.com/wirelessRouter.php; classtype:attempted-admin; sid:28289; rev:4;)
```
Проверка:  
```
echo -n 'w302r_mfg' > /tmp/sid_28289.txt
echo -n -e '\x00' >> /tmp/sid_28289.txt
echo -n 'x' >> /tmp/sid_28289.txt

ip_address='192.168.6.1'
cat /tmp/sid_28289.txt > /dev/udp/${ip_address}/7329
```
## Проверка срабатывания IDS по правилу из стандартного набора server-webapp.rules (sid: 28290)
Правило:  
```
alert udp any any -> any 7329 (msg:"SERVER-WEBAPP Tenda W302R iwpriv remote code execution attempt"; \
flow:to_server; content:"w302r_mfg|00|1"; fast_pattern:only; \
metadata:policy balanced-ips drop, policy max-detect-ips drop, policy security-ips drop, service http; \
reference:url,www.medialinkproducts.com/wirelessRouter.php; classtype:attempted-admin; sid:28290; rev:4;)
```
Проверка:  
```
echo -n 'w302r_mfg' > /tmp/sid_28290.txt
echo -n -e '\x00' >> /tmp/sid_28290.txt
echo -n '1' >> /tmp/sid_28290.txt

ip_address='192.168.6.1'
cat /tmp/sid_28290.txt > /dev/udp/${ip_address}/7329
```
## Проверка срабатывания IDS по правилу из стандартного набора malware-cnc.rules (sid: 32624)
Правило:  
```
alert icmp any any -> any any (msg:"MALWARE-CNC Win.Trojan.Regin outbound connection"; \
itype:0; content:"shit"; content:"shit"; distance:0; \
metadata:impact_flag red, policy balanced-ips drop, policy security-ips drop; \
reference:url,www.virustotal.com/en/file/c0cf8e008fbfa0cb2c61d968057b4a077d62f64d7320769982d28107db370513/analysis/; \
classtype:trojan-activity; sid:32624; rev:3;)
```
Проверка:  
```
ip_address='192.168.6.100'
hping3 ${ip_address} --icmp --icmptype 0 --count 1 --print --sign 'shitshit'
```
Альтернативная проверка:  
```
ip_address='192.168.6.100'
ping -c 1 -p '7368697473686974' ${ip_address}
```
## Проверка срабатывания IDS по правилу из стандартного набора malware-cnc.rules (sid: 50764)
Правило:  
```
alert udp any any -> any 53 (msg:"MALWARE-CNC Win.Trojan.Helminth outbound DNS tunnel"; \
flow:to_server,no_stream; content:"|01 00|"; depth:2; offset:2; content:"00000"; within:8; distance:12; \
content:"232A"; within:4; distance:3; \
metadata:impact_flag red, policy balanced-ips drop, policy max-detect-ips drop, policy security-ips drop, service dns; \
reference:url,virustotal.com/gui/file/662c53e69b66d62a4822e666031fd441bbdfa741e20d4511c6741ec3cb02475f; \
classtype:trojan-activity; sid:50764; rev:1;)
```
Проверка:  
```
echo -n -e '\xFF\xFF' > /tmp/sid_50764.txt
echo -n -e '\x01\x00' >> /tmp/sid_50764.txt
echo -n -e '\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF' >> /tmp/sid_50764.txt
echo -n '00000' >> /tmp/sid_50764.txt
echo -n -e '\xFF\xFF\xFF' >> /tmp/sid_50764.txt
echo -n '232A' >> /tmp/sid_50764.txt

ip_address='192.168.6.100'
cat /tmp/sid_50764.txt > /dev/udp/${ip_address}/53
```
## Проверка срабатывания IDS по правилу из стандартного набора malware-cnc.rules (sid: 50765)
Правило:  
```
alert udp any any -> any 53 (msg:"MALWARE-CNC Win.Trojan.ISMAgent outbound DNS tunnel"; \
flow:to_server,no_stream; content:"|01 00|"; depth:2; offset:2; \
content:"|01|n|01|n|01|c"; within:6; distance:8; \
metadata:impact_flag red, policy balanced-ips drop, policy maxdetect-ips drop, policy security-ips drop, service dns; \
reference:url,virustotal.com/gui/file/a9f1375da973b229eb649dc3c07484ae7513032b79665efe78c0e55a6e716821; \
classtype:trojan-activity; sid:50765; rev:1;)
```
Проверка:  
```
echo -n -e '\xFF\xFF' > /tmp/sid_50765.txt
echo -n -e '\x01\x00' >> /tmp/sid_50765.txt
echo -n -e '\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF' >> /tmp/sid_50765.txt
echo -n -e '\x01' >> /tmp/sid_50765.txt
echo -n 'n' >> /tmp/sid_50765.txt
echo -n -e '\x01' >> /tmp/sid_50765.txt
echo -n 'n' >> /tmp/sid_50765.txt
echo -n -e '\x01' >> /tmp/sid_50765.txt
echo -n 'c' >> /tmp/sid_50765.txt

ip_address='192.168.6.100'
cat /tmp/sid_50765.txt > /dev/udp/${ip_address}/53
```
## Проверка срабатывания IDS по правилу из стандартного набора malware-cnc.rules (sid: 50768)
Правило:  
```
alert udp any any -> any 53 (msg:"MALWARE-CNC Win.Trojan.BONDUPDATER outbound DNS tunnel"; \
flow:to_server,no_stream; content:"|01 00|"; depth:2; offset:2; content:"0"; within:11; distance:11; \
content:"B007"; within:15; distance:4; content:"|00 01|"; distance:0; \
isdataat:1,relative; isdataat:!3,relative; \
metadata:impact_flag red, policy balanced-ips drop, policy max-detect-ips drop, policy security-ips drop, service dns; \
reference:url,virustotal.com/gui/file/de620a0511d14a2fbc9b225ebfda550973d956ab4dec7e460a42e9d2d3cf0588; \
classtype:trojan-activity; sid:50768; rev:2;)
```
Проверка:  
```
echo -n -e '\xFF\xFF' > /tmp/sid_50768.txt
echo -n -e '\x01\x00' >> /tmp/sid_50768.txt
echo -n -e '\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF' >> /tmp/sid_50768.txt
echo -n '0' >> /tmp/sid_50768.txt
echo -n -e '\xFF\xFF\xFF\xFF' >> /tmp/sid_50768.txt
echo -n 'B007' >> /tmp/sid_50768.txt
echo -n -e '\x00\x01\x00\x01' >> /tmp/sid_50768.txt

ip_address='192.168.6.100'
cat /tmp/sid_50768.txt > /dev/udp/${ip_address}/53
```
## Проверка срабатывания IDS по правилу из стандартного набора indicator-scan.rules (sid: 41793)
Правило:  
```
alert udp any any -> any 69 (msg:"INDICATOR-SCAN Cisco Smart Install Protocol scan TFTP response"; \
flow:to_server; dsize:20; content:"|00 01|random_file|00|octet|00|"; depth:20; \
metadata:policy balanced-ips drop, policy connectivity-ips drop, policy max-detect-ips drop,policy security-ips drop, service tftp; \
reference:url,attack.mitre.org/techniques/T1018; \
reference:url,attack.mitre.org/techniques/T1040; \
reference:url,attack.mitre.org/techniques/T1046; \
reference:url,tools.cisco.com/security/center/content/CiscoSecurityResponse/cisco-sr-20170214-smi; \
classtype:attempted-recon; sid:41793; rev:3;)
```
Проверка:  
```
echo -n -e '\x00\x01' > /tmp/sid_41793.txt
echo -n 'random_file' >> /tmp/sid_41793.txt
echo -n -e '\x00' >> /tmp/sid_41793.txt
echo -n 'octet' >> /tmp/sid_41793.txt
echo -n -e '\x00' >> /tmp/sid_41793.txt

ip_address='192.168.6.100'
cat /tmp/sid_41793.txt > /dev/udp/${ip_address}/69
```
## Проверка срабатывания IDS в часте эвристического анализа
Сканирование пртоколов:
```
ip_address='192.168.6.100'
for ((item=1; item<100; item++)); do
  echo "protocol scan ${item}"
  hping3 --rawip ${ip_address} --ipproto ${item} --count 1 2>&1
done
```
Сканирование хостов по протоколу icmp
```
net_address='192.168.6.0'
for ((item=1; item<255; item++)); do
  hping3 --icmp ${net_address%%0}${item} --count 1 2>&1
done
```
Сканирование хостов по протоколу udp
```
net_address='192.168.6.0'
for ((item=1; item<255; item++)); do
  hping3 --udp ${net_address%%0}${item} --destport 22 --count 1 2>&1
done
```
Сканирование хостов по протоколу tcp
```
net_address='192.168.6.0'
for ((item=1; item<255; item++)); do
  hping3 --syn ${net_address%%0}${item} --destport 22 --count 1 2>&1
done
```
Сканирование портов по протоколу udp
```
ip_address='192.168.6.100'
port_list='21 22 23 80 443 8080'
for port in ${port_list}; do
  hping3 --udp ${ip_address} --destport ${port} --count 1 2>&1
done
```
Сканирование портов по протоколу tcp
```
ip_address='192.168.6.100'
port_list='21 22 23 80 443 8080'
for port in ${port_list}; do
  hping3 --syn ${ip_address} --destport ${port} --count 1 2>&1
done
```
Перебор паролей
```
ip_address='192.168.6.100'

users_list='admin radmin root user tester'
file_users='/tmp/users.list'
echo -n > ${file_users}
for item in ${users_list}; do
  echo ${item} >> ${file_users}
done

passwd_list='admin radmin root user tester toor 12345678 q12345678 1234 1 qwerty'
file_passwd='/tmp/passwd.list'
echo -n > ${passwd_list}
for item in ${users_list}; do
  echo ${item} >> ${file_passwd}
done

hydra -V -t 30 -L ${file_users} -P ${file_passwd} -I ssh://${ip_address}
```
