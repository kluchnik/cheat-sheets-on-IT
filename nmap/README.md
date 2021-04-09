# Применение nmap

## Использование скриптов:
```(bash)
nmap --script <name_script> -sV -p <port> <host>
```
Список скриптов расположен в /usr/share/nmap/scripts/

Пример проверки ssh-сертификата:  
```(bash)
nmap --script ssl-cert,ssl-enum-ciphers -sV -p <port> <host>
```
