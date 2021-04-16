# Описание suricata  

# Структура правил  
```
^ <action> (действия) ^ <header> (protocol, IP, ports, направление) ^ (<rule> (тело пакета)) ^
```
- action - лействия  
```
[pass | drop | reject | alert]
```
- header - параметры заголовка сетевого пакета  
```
[ip | icmp | udp | tcp | (http | ftp | tls (this includes ssl) | smb | dns | dcerpc | ssh | smtp | imap | msn)]
[IP_src "x.x.x.x" | grouping "[..,..]" | IP ranges "../.." | exception "!" | variables "$variables"]
[port_src "xxx" | grouping "[..,..]" | port ranges ":" | exception "!" | variables "$variables"]
[-> | <>] 
[IP_dst "x.x.x.x" | grouping "[..,..]" | IP ranges "../.." | exception "!" | variables "$variables"]
[port_dst "xxx" | grouping "[..,..]" | port ranges ":" | exception "!" | variables "$variables"]
```
- rule - параметры для сигнатурного анализа  
```
 [<keyword>: <settings>; <keyword>;]
```
| Параметр | Описание |
| ------------ | ------------ |
| Meta Keywords (информация правила) |
| msg: "some description"; | Сообщение при срабатывании правила |
| sid: <number>; | Уникальный индентификатор правила |
| rev: <number>; | Версия правила |
| classtype: <web-application-attack>; | класс атаки |
| reference: <type>, <reference>; | ссылка на источник ["reference: url, www.info.com;" \| "reference: cve, CVE-2014-1234"] |
| priority: <number>; | Приоритет правила [1..255] |
| metadata: <key> <value>, <key> <value>; | Добавление дополнительной информации к правилу |
| target: [src_ip | dest_ip]; | Дополнительная информация о целе атаки |
| IP Keywords (данные IP-загаловка) |
| ttl: <number>; | ttl пакета |
| ipopts: <name>; | IP опция [rr \| eol \| nop \| ts \| sec \| esec \| lsrr \| ssrr \| satid \| any] |
| sameip; | Совпадение IP-адреса источника с IP-адресом получателя |
| ip_proto: <number>; | Номер IP-протокола https://en.wikipedia.org/wiki/List_of_IP_Protocol_numbers |
| id: <number>; | Индентификатор пакета |
| geoip | Сопоставление IP-адреса со страной через  GeoIP API of Maxmind |
| fragbits:[\*+!]<[MDR]>; | Проверить, установлен ли в заголовке IP-фрагментация 
| | option: * - совпадать, если установлен какой-либо из указанных битов | 
| | option: + - совпадать с указанными битами плюс любые другие |
| | option: ! - совпадать, если указанные биты не установлены |
| | option: M - фрагментированный |
| | option: D - отсутсвует фрагментация |
| | option: R - резервный бит |
| fragoffset: [!\|<\|>]<number>; | Фрагмент IP-пакета |
| tos: [!]<number>; | Параметр ToS |
| TCP keywords (данные TCP заголовка) |
| seq: <number>; | Проверка номера TCP |
| ack: <number>; | Проверка номера подтверждения получения данных |
| window: [!]<number>; | Размера окна TCP [2 .. 65535] bytes |
| ICMP keywords (данные ICMP заголовка) |
| itype: [<|>]<number>; \| itype:min<>max; | Тип ICMP (https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml) |
| icode: [<|>]<number>; \| icode:min<>max; | Код ICMP |
| icmp_id: <number>; | ICMP-идентификатор |
| icmp_seq: <number>; | Порядковый номер ICMP |
| Payload Keywords (содержимое пакета) |
| content:[!]“...”; \| content:“a\|0D\|bc”; \| content:”\|61 0D 62 63\|"; \| content:”a\|0D\|b\|63\|”; | Содержимое пакета |
| | option: nocase; - не учитывать регистр |
| | option: offset: <number>; - начало проверки в символах (если не указывать offset:0;) |
| | option: depth: <number>; - глубина проверки в символах начиная с offset |
| | option: startswith; - проверят с начала (offset:0;), глубина автоматически подстраиваеться под размер content |
| | option: isdataat: <number> - есть ли еще любые данные в определенной части |
| | option: isdataat: <number>, relative; - есть ли еще любые данные в определенной части, относительно предыдущего значения |
| | option: replace:"string"; - замена содержимого content на replace в режиме IPS |
| | option: fast_pattern; - установка приоритета проверки для ускарения |
| | option: HTTP Keywords - https://suricata.readthedocs.io/en/suricata-4.1.4/rules/http-keywords.html |
| content:""; content:""; | Указание нескольких content |
| | option: distance: <number>; - относительное содержание, растояние между контентом [-2 | 0 | 2] \(если не указывать distance:0;\) |
| | option: within: <number>; - глубина поиска отностительно distance |
| uricontent: "<uri>"; | Проверка uri http, пример: uricontent:"/frame.html?"; аналог content:"/frame.html?"; http_uri; | 
| | option: urilen: <number>[ < | > | <> ]<number>; - длина uri |
| pcre: "/<regex>/opts"; | Регулярные выражения (Perl Compatible Regular Expressions) opts = [i \| s \| m \| A \| E \| G] |
| | opts: R - Совпадение относительно последнего совпадения с образцом (distance:0;) |
| | opts: U - Cопоставляет pcre с нормализованным uri |
| | opts: [I \| P \| Q \| H \| D \| M \| C \| S \| Y \| B \| O \| V \| W] |
| dsize: [<\|>]<number>; | Проверка размера пакета |
| rpc: <application number>, [<version number> \| \*], [<procedure number> \| \*]>; | Работа с RPC |
| http_protocol; | Проверяет поле протокола из HTTP-запроса или строки ответа. Если строка запроса - «GET / HTTP / 1.0rn», то этот буфер будет содержать «HTTP / 1.0» |
| http_request_line; | Заставляет проверять всю строку HTTP-запроса |
| flowbits: <parametr>, <name>; | Проверка двух пакетов |
| | настройка (https://suricata.readthedocs.io/en/suricata-4.1.4/configuration/suricata-yaml.html#flow-and-stream-handling) |
| | flowbits: set, <name>; - установить имя в потоке, первый тригер |
| | flowbits: isset, <name>; - второй тригер, если сработоло set |
| | flowbits: isnotset, <name>; - второй тригер, если не сработал set |
| | flowbits: toggle, <name>; - переключатель, изменяет состояние set с on на off и наоборот |
| | flowbits: unset, <name>; - сброс условия |
| | flowbits: noalert; - не генерировать сообщение |
| | Пример: -> ^.. flowbits: set, test; flowbits: noalert; ..^ <- ^.. flowbits: isset, test; ..^ |
| flow: <option>, <option>; | Направление потока |
| | option: to_client - от сервера к клиенту |
| | option: from_server - от сервера к клиенту |
| | option: to_server - от клиента к серверу |
| | option: from_client - от клиента к серверу |
| | option: established - по установленному соединению для TCP (для других протоколов прсматривается с двух сторон) |
| | option: not_established - по не установленному соединению для TCP (для других протоколов прсматривается с двух сторон) |
| | option: stateless - по установленному и не установленному соединению для TCP |
| | option: only_stream - совпадение с пакетом собраные потоковым движком |
| | option: no_stream - не будет соответствовать пакетам, которые были повторно собраны потоковым движком |
| | option: only_frag - совпадение пакетов, которые были собраны из фрагментов |
| | option: no_frag - совпадение пакетов, которые не были повторно собраны из фрагментов |
| flowint: <name>, <modifier> [, <value>]; | Позволяет хранить математические операции с использованием переменных |
| | Пример: [flowint: <name>, [ + \| - \| = \| > \| < \| >= \| <= \| == \| != ], <value>; \| flowint: <name>, [isset \| isnotset];] |
| | alert tcp any any -> any any (msg:"More than Five Usernames!"; content:"login user"; flowint: usernamecount, isnotset; flowint:usernamecount, =, 1; noalert;) |
| | alert tcp any any -> any any (msg:"More than Five Usernames!"; content:"login user"; flowint: usernamecount, isset; flowint: usernamecount, +, 1; flowint:usernamecount, >, 5;) - считает количества name:user и записывает в переменную usernamecount, после выполнения условия производится срабатывание условия usernamecount > 5 |
| alert tcp any any -> any any (msg:"Username Logged out"; content:"logout user"; flowint: usernamecount, -, 1; noalert;) |
| stream_size: [server \| client \| both \| either], <modifier>, <number>; | Размера потока соответствует трафику |
| | Пример: stream_size: [server \| client \| both \| either], [ > \| < \| >= \| <= \| = \| != ], <number>; |
| Bypass Keyword |
| bypass; | Исключения трафика из дальнейшей оценки |
| File Keywords (извлечение файла из потока) |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/file-keywords.html |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/file-extraction/file-extraction.html |
| DNS Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/dns-keywords.html |
| SSL/TLS Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/tls-keywords.html |
| SSH Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/ssh-keywords.html |
| JA3 Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/ja3-keywords.html |
| Modbus Keyword |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/modbus-keyword.html |
| DNP3 Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/dnp3-keywords.html |
| ENIP/CIP Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/enip-keyword.html |
| FTP/FTP-DATA Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/ftp-keywords.html |
| Kerberos Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/kerberos-keywords.html |
| Generic App Layer Keywords |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/app-layer.html |
| Xbits Keyword (установить, сбросить, переключать и проверять биты) |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/xbits.html |
| Thresholding Keywords (управления частотой оповещения правила) |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/thresholding.html |
| IP Reputation Keyword |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/ip-reputation-rules.html |
| Lua Scripting |
| | https://suricata.readthedocs.io/en/suricata-4.1.4/rules/rule-lua-scripting.html |