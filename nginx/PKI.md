# Развертывание Инфраструктуры открытых ключей (PKI) с нуля

Оригинал взят отсюда: http://habrahabr.ru/post/213741/  

## Создание своего центра сертификации (CA, Certificate authority)  
Создаём ключ для CA (ca.key) и самоподписанный доверенный сертификат (ca.crt)  
```
openssl req -new -newkey rsa:2048 -nodes -keyout ca.key -x509 -days 3650 \
-subj '/C=RU/ST=Moscow/L=Moscow/O=My/CN=etc/emailAddress=mail@my.ru' -out ca.crt
```

## Создание ключей и сертификата для веб-сервера  
Создаём пару RSA-ключей для nginx и запрос на сертификат:  
```
openssl genrsa -out server.key 2048
openssl req -new -key server.key -out server.csr
```
Подписываем сертификат ключом самодельного центра сертификации:  
```
openssl x509 -req -days 3650 -in server.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out server.crt
```
Чтобы nginx при перезагрузке не спрашивал пароль, сделаем для него беспарольную копию сертификата:  
```
openssl rsa -in server.key -out server.nopass.key
```
## Создание директорий для служебных файлов  
```
mkdir db
mkdir db/certs
mkdir db/newcerts
touch db/index.txt
echo "01" > db/serial
```
## Конфигурациия
Конфиг openssl /update/ca.config  
```
[ ca ]
default_ca = CA_CLIENT              # При подписи сертификатов использовать секцию CA_CLIENT

[ CA_CLIENT ]
dir = /updates/db                   # Каталог для служебных файлов
certs = $dir/certs                  # Каталог для сертификатов
new_certs_dir = $dir/newcerts       # Каталог для новых сертификатов

database = $dir/index.txt           # Файл с базой данных подписанных сертификатов
serial = $dir/serial                # Файл содержащий серийный номер сертификата (в шестнадцатиричном формате)
certificate = /updates/ca.crt       # Файл сертификата CA
private_key = /updates/ca.key       # Файл закрытого ключа CA
crl = $dir/crl.pem

default_days = 1                    # Срок действия подписываемого сертификата
default_crl_days = 7                # Срок действия CRL
default_md = md5                    # Алгоритм подписи

policy = policy_anything            # Название секции с описанием политики в отношении данных сертификата

[ policy_anything ]
countryName = optional              # Поля optional - не обязательны, supplied - обязательны
stateOrProvinceName = optional
localityName = optional
organizationName = optional
organizationalUnitName = optional
commonName = optional
emailAddress = optional
```
Конфиг nginx /etc/nginx/nginx.conf  
```
user  nginx;
worker_processes  1;
error_log  /var/log/nginx/error.log warn;
pid        /var/run/nginx.pid;
events {
    worker_connections  1024;
}
http {
    include       /etc/nginx/mime.types;
    default_type  application/octet-stream;
    log_format  main  '$remote_addr - $remote_user [$time_local] "$request" '
                      '$status $body_bytes_sent "$http_referer" '
                      '"$http_user_agent" "$http_x_forwarded_for"';
    access_log  /var/log/nginx/access.log  main;
    sendfile           on;
    #tcp_nopush        on;
    keepalive_timeout  65;
    #gzip              on;
    include /etc/nginx/conf.d/*.conf;
    server {
	server_name "sca35";
        autoindex on;
        
        location ^~ /sca35/ {
            root /var/www/sca35;
        }
        
        location ^~ /sca34/ {
            root /var/www/sca34;
        }
        listen *:443;
        ssl on;
        ssl_certificate /updates/server.crt;
        ssl_certificate_key /updates/server.nopass.key;
        ssl_client_certificate /updates/ca.crt;
        ssl_crl /updates/crl.pem;
        ssl_verify_client on;
        keepalive_timeout 70;
        fastcgi_param SSL_VERIFIED $ssl_client_verify;
        fastcgi_param SSL_CLIENT_SERIAL $ssl_client_serial;
        fastcgi_param SSL_CLIENT_CERT $ssl_client_cert;
        fastcgi_param SSL_DN $ssl_client_s_dn;
    }
}
```

## Генерация ключей для клиента:
```
scripts/genclientcert.sh <время_обновления_в_днях> <серийный номер>
```
```
#!/bin/sh
if [ $# -ne 2 ]; then
  echo "Need two arguments: number_of_days and client_id_number"
  exit 1
fi
BASEPATH=/updates
CLIENTSPATH=${BASEPATH}/clients

openssl req -new -newkey rsa:2048 -nodes -keyout ${CLIENTSPATH}/client$2.key -subj /C=RU/ST=Moscow/L=Moscow/O=$2/OU=User/CN=etc/emailAddress=client@example.ru -out ${CLIENTSPATH}/client$2.csr
openssl ca -config ${BASEPATH}/ca.config -days $1 -in ${CLIENTSPATH}/client$2.csr -out ${CLIENTSPATH}/client$2.crt -batch
```
| Получаем файлы | Описание |
| - | - |
| /updates/clients/client<SN>.key | приватный ключ клиента |
| /updates/clients/client<SN>.csr | запрос на получение сертификата. Содержит публичный ключ. Временный файл для получения сертификата, больше нигде не используется | 
| /updates/clients/client<SN>.crt | сертификат клиента (я, центр сертификации, удостоверяю, что публичный ключ XXXX принадлежит этому клиенту) |
| /updates/db/newcerts/<serial>.pem | совпадает с client<SN>.crt. Здесь <serial> -- порядковый номер сертификата, можно увидеть в третьем поле в ```grep --colour=auto /O=<SN> /updates/db/index.txt``` |
 
## Отзыв сертификата клиента:
```
scripts/revokecert.sh <серийный номер>
```
```
#!/bin/sh
if [ $# -ne 1 ]; then
  echo "Need one argument: client_id_number"
  exit 1
fi
BASEPATH=/updates
CLIENTSPATH=${BASEPATH}/clients

openssl ca -config ${BASEPATH}/ca.config -revoke ${CLIENTSPATH}/client$1.crt -crl_reason affiliationChanged
openssl ca -gencrl -config ${BASEPATH}/ca.config -out ${BASEPATH}/crl.pem

nginx -s reload
```
В CRL--файл (Certificate Revocation List) (какой?) вносится сертификат провинившегося клиента и автоматически вызывается nginx -s reload. Если сертификат был отозван пользователем без повышенных привелегий, nginx необходимо перезапустить вручную от рута.  

## Удаление сертификата клиента:
1. Если остался образ с этим сертификатом, он сможет соединяться с сервером до тех пор, пока не выйдет срок сертификата. Отозвать его будет невозможно  
2. Команды sed и rm стоит выполнять с осторожностью, чтобы по ошибке не удалить нужный сертификат. Восстановить его будет невозможно  
3. При удалении сертификата не от пользователя production меняются права и владелец файлов /updates/db/serial и /updates/db/index.txt, что приводит к невозможности генерации сертификатов от production  
 
Просмотр параметров сертификата в журнале:  
```
grep --colour=auto /O=<SN> /updates/db/index.txt
```
Если ничего лишнего не выводит, то запоминаем значение счётчика в третьем поле (<serial>).  
```
sed -i "\:/O=<SN>/:d" /updates/db/index.txt
rm /updates/clients/client<SN>.*
rm /updates/db/newcerts/<serial>.pem   # , где <serial> -- значение счётчика
```
Опционально: если после этого клиента не генерились новые сертификаты, можно уменьшить на 1 (в шестнадцатеричной системе) счётчик в /updates/db/serial.  
Теперь можно сгенерить новые ключи и сертификат для того же клиента.  

## Какие файлы в /updates за что отвечают?  
| Файл | Описание |
| - | - |
| /updates/server.crt | ssl-сертификат веб-сервера |
| /updates/server.nopass.key | ssl-ключ веб-сервера |
| /updates/ca.crt | самоподписанный сертификат CA |
| /updates/ca.key | приватный ключ CA для выдачи сертификатов клиентам |
| /updates/crl.pem | список отозванных сертификатов |
| /updates/scripts | наши скрипты, обёртки над openssl |
| /updates/clients/ | каталог для хранения клиентских ключей и сертификатов. В работе сервера никак не участвуют, лежат чтобы быть скопированными в iso-образ, а дальше просто на память |
| /updates/db/serial | шестнадцатеричный счётчик выданных сертификатов |
| /updates/db/index.txt | "база данных" клиентских сертификатов |
| /updates/db/certs/ | те же сертификаты клиентов, что и в /updates/clients/, только имена файлов другие. Openssl сохраняет на память |
