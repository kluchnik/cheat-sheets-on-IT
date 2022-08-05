# Подпись файла


## Create the CA cert
openssl.cfg -> myca.crt, myca.key 
```
$ ls ./
openssl.cfg

$ file openssl.cfg
openssl.cfg: ASCII text
```
```
$ openssl req -config openssl.cfg -new -x509 -keyout myca.key -out myca.crt -days 3650
```
```
$ ls ./
myca.crt  myca.key  openssl.cfg

$ file myca.crt
myca.crt: PEM certificate

$ file myca.key
myca.key: ASCII text
```

## Create a Private Key for the Time Service Authority
... -> tsakey.pem
```
$ openssl genrsa -out tsakey.pem 2048
```
```
$ ls ./
myca.crt  myca.key  openssl.cfg  tsakey.pem

$ file tsakey.pem
tsakey.pem: PEM RSA private key
```

## Generate a request for a Certificate from the CA
openssl.cfg, tsakey.pem -> tsareq.csr
```
$ openssl req -config openssl.cfg -new -key tsakey.pem -out tsareq.csr
```
```
$ ls ./
myca.crt  myca.key  openssl.cfg  tsakey.pem  tsareq.csr

$ file tsareq.csr
tsareq.csr: PEM certificate request
```

## Generate and sign the certificate from a request signed by the CA
openssl.cfg, tsareq.csr -> tsacert.pem
```
$ mkdir newcerts
$ touch index.txt
$ echo 01 > serial
$ openssl ca -config openssl.cfg -in tsareq.csr -out tsacert.pem
```
```
$ ls ./
index.txt       index.txt.old  myca.key  openssl.cfg  serial.old   tsakey.pem
index.txt.attr  myca.crt       newcerts  serial       tsacert.pem  tsareq.csr

$ ls ./newcerts/
01.pem

$ cat index.txt
V       230805131322Z           01      unknown /C=RU/ST=Moscow/O=company/CN=example

$ cat index.txt.attr
unique_subject = yes

$ cat serial.old
01

$ cat serial
02

$ file ./newcerts/01.pem
./newcerts/01.pem: ASCII text
```

## Generate a TSA sign request
openssl.cfg, data -> file.tsq
```
$ echo test > file.txt
```
```
$ openssl ts -query -config openssl.cfg -data file.txt -cert -out file.tsq
$ # openssl ts -query -config openssl.cfg -data file.txt -no_nonce -out file.tsq
```
```
$ ls ./
file.tsq  index.txt       index.txt.old  myca.key  openssl.cfg  serial.old   tsakey.pem
file.txt  index.txt.attr  myca.crt       newcerts  serial       tsacert.pem  tsareq.csr

$ file file.tsq
file.tsq: data
```
```
$ openssl ts -query -in file.tsq -text
Version: 1
Hash Algorithm: sha1
Message data:
    0000 - 4e 12 43 bd 22 c6 6e 76-c2 ba 9e dd c1 f9 13 94   N.C.".nv........
    0010 - e5 7f 9f 83                                       ....
Policy OID: unspecified
Nonce: 0x306DD09C509FC616
Certificate required: yes
Extensions:
```

## Generate a TSA response from a request file
openssl.cfg, file.tsq, tsakey.pem, tsacert.pem -> file.tsr
```
$ echo 01 > tsaserial
$ openssl ts -reply -config openssl.cfg -queryfile file.tsq -out file.tsr -inkey tsakey.pem -signer tsacert.pem
```
```
$ ls ./
file.tsq  file.txt   index.txt.attr  myca.crt  newcerts     serial      tsacert.pem  tsareq.csr
file.tsr  index.txt  index.txt.old   myca.key  openssl.cfg  serial.old  tsakey.pem   tsaserial

$ cat tsaserial
02

$ file file.tsr
file.tsr: data
```
```
$ openssl ts -reply -in file.tsr -text
Status info:
Status: Granted.
Status description: unspecified
Failure info: unspecified

TST info:
Version: 1
Policy OID: 1.1.3
Hash Algorithm: sha1
Message data:
    0000 - 4e 12 43 bd 22 c6 6e 76-c2 ba 9e dd c1 f9 13 94   N.C.".nv........
    0010 - e5 7f 9f 83                                       ....
Serial number: 0x02
Time stamp: Aug  5 13:27:38 2022 GMT
Accuracy: 0x01 seconds, 0x01F4 millis, 0x64 micros
Ordering: no
Nonce: 0x306DD09C509FC616
TSA: DirName:/C=RU/ST=Moscow/O=company/CN=example
Extensions:
```

## Generate the TS token (not in TS response format) from a request file
openssl.cfg, file.tsq -> file.der
```
openssl ts -reply -config openssl.cfg -queryfile file.tsq -out file.der -token_out
```
```
$ ls ./
file.der  file.tsr  index.txt       index.txt.old  myca.key  openssl.cfg  serial.old   tsakey.pem  tsaserial
file.tsq  file.txt  index.txt.attr  myca.crt       newcerts  serial       tsacert.pem  tsareq.csr

$ file file.der
file.der: data
```
```
$ openssl asn1parse -in file.der -inform DER
    0:d=0  hl=4 l=2512 cons: SEQUENCE
    4:d=1  hl=2 l=   9 prim: OBJECT            :pkcs7-signedData
   15:d=1  hl=4 l=2497 cons: cont [ 0 ]
   19:d=2  hl=4 l=2493 cons: SEQUENCE
   23:d=3  hl=2 l=   1 prim: INTEGER           :03
   ...
```

## Validate the TS token
openssl.cfg, file.der, myca.crt, data -> Verification
```
$ openssl ts -verify -config openssl.cfg -data file.txt -in file.der -token_in -CAfile myca.crt
Verification: OK
```

## Validate the TS response
file.tsr, myca.crt, data -> Verification
```
$ openssl ts -verify -data file.txt -in file.tsr -CAfile myca.crt
Verification: OK
```
## Other validate
file.tsr, file.tsq, myca.crt, tsacert.pem -> Verification
```
$ openssl ts -verify -in file.tsr -queryfile file.tsq -CAfile myca.crt -untrusted tsacert.pem
Verification: OK
```
file.tsr, myca.crt, tsacert.pem, data -> Verification
```
$ openssl ts -verify -data file.txt -in file.tsr -CAfile myca.crt -untrusted tsacert.pem
Verification: OK
```
file.tsr, myca.crt, file.tsq -> Verification
```
$ openssl ts -verify -in file.tsr -queryfile file.tsq -CAfile myca.crt
Verification: OK
```
