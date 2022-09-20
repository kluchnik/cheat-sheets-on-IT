# Подпись файлов с использованием внешнего сервера

## 1. Сгенерировать tsq для файлов для подписи
```
$ openssl ts -query -data file.txt -no_nonce -sha1 -cert -out file.tsq
```
## 2. Подпись файла с использованием внешнего сервера
```
$ curl -H "Content-Type: application/timestamp-query" --data-binary '@file.tsq' https://freetsa.org/tsr > file.tsr
```
## 3. Скачивваем файл УЦ
```
$ wget https://www.freetsa.org/files/cacert.pem
```
## 4. Проверка подписи
```
$ openssl ts -verify -data file.txt -in file.tsr -CAfile cacert.pem
Verification: OK
```
