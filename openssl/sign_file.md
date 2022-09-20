# Подпись файла с использованием собственного сервера

## 1. Создаем директорию для работы с файлами, которые необходимо подписать
```
$ mkdir ~/server_cert
$ cd ~/server_cert
```

## 2. Создаем файл конфигурации openssl.cfg

```
$ cat openssl.cfg

#
# OpenSSL example configuration file.
# This is mostly being used for generation of certificate requests.
#

# This definition stops the following lines choking if HOME isn't
# defined.
HOME                    = .
RANDFILE                = $ENV::HOME/.rnd

# Extra OBJECT IDENTIFIER info:
#oid_file               = $ENV::HOME/.oid
oid_section             = new_oids

# To use this configuration file with the "-extfile" option of the
# "openssl x509" utility, name here the section containing the
# X.509v3 extensions to use:
# extensions            =
# (Alternatively, use a configuration file that has only
# X.509v3 extensions in its main [= default] section.)

[ new_oids ]

# We can add new OIDs in here for use by 'ca', 'req' and 'ts'.
# Add a simple OID like this:
# testoid1=1.2.3.4
# Or use config file substitution like this:
# testoid2=${testoid1}.5.6

# Policies used by the TSA examples.
tsa_policy1 = 1.2.3.4.1
tsa_policy2 = 1.2.3.4.5.6
tsa_policy3 = 1.2.3.4.5.7

####################################################################
[ ca ]
default_ca      = CA_default            # The default ca section

####################################################################
[ CA_default ]

dir             = .                     # Where everything is kept
certs           = $dir/certs            # Where the issued certs are kept
crl_dir         = $dir/crl              # Where the issued crl are kept
database        = $dir/index.txt        # database index file.
#unique_subject = no                    # Set to 'no' to allow creation of
                                        # several ctificates with same subject.
new_certs_dir   = $dir/newcerts         # default place for new certs.

certificate     = $dir/myca.crt         # The CA certificate
serial          = $dir/serial           # The current serial number
crlnumber       = $dir/crlnumber        # the current crl number
                                        # must be commented out to leave a V1 CRL
crl             = $dir/crl.pem          # The current CRL
private_key     = $dir/myca.key         # The private key
RANDFILE        = $dir/.rand            # private random number file

x509_extensions = usr_cert              # The extentions to add to the cert

# Comment out the following two lines for the "traditional"
# (and highly broken) format.
name_opt        = ca_default            # Subject Name options
cert_opt        = ca_default            # Certificate field options

# Extension copying option: use with caution.
# copy_extensions = copy

# Extensions to add to a CRL. Note: Netscape communicator chokes on V2 CRLs
# so this is commented out by default to leave a V1 CRL.
# crlnumber must also be commented out to leave a V1 CRL.
# crl_extensions        = crl_ext

default_days    = 365                   # how long to certify for
default_crl_days= 30                    # how long before next CRL
default_md      = sha1                  # use public key default MD
preserve        = no                    # keep passed DN ordering

# A few difference way of specifying how similar the request should look
# For type CA, the listed attributes must be the same, and the optional
# and supplied fields are just that :-)
policy          = policy_match

# For the CA policy
[ policy_match ]
countryName             = match
stateOrProvinceName     = match
organizationName        = match
organizationalUnitName  = optional
commonName              = supplied
emailAddress            = optional

# For the 'anything' policy
# At this point in time, you must list all acceptable 'object'
# types.
[ policy_anything ]
countryName             = optional
stateOrProvinceName     = optional
localityName            = optional
organizationName        = optional
organizationalUnitName  = optional
commonName              = supplied
emailAddress            = optional

####################################################################
[ req ]
default_bits            = 2048
default_keyfile         = myca.ke
distinguished_name      = root_ca_distinguished_name
attributes              = req_attributes
x509_extensions = v3_ca # The extentions to add to the self signed cert

# Passwords for private keys if not present they will be prompted for
# input_password = secret
# output_password = secret

# This sets a mask for permitted string types. There are several options.
# default: PrintableString, T61String, BMPString.
# pkix   : PrintableString, BMPString (PKIX recommendation before 2004)
# utf8only: only UTF8Strings (PKIX recommendation after 2004).
# nombstr : PrintableString, T61String (no BMPStrings or UTF8Strings).
# MASK:XXXX a literal mask value.
# WARNING: ancient versions of Netscape crash on BMPStrings or UTF8Strings.
string_mask = nombstr

# req_extensions = v3_req # The extensions to add to a certificate request

[ root_ca_distinguished_name ]
countryName                     = Country Name (2 letter code)
countryName_default             = RU
countryName_min                 = 2
countryName_max                 = 2

stateOrProvinceName             = State or Province Name (full name)
stateOrProvinceName_default     = Moscow

localityName                    = Locality Name (eg, city)

0.organizationName              = Organization Name (eg, company)
0.organizationName_default      = Echelon

# we can do this but it is not needed normally :-)
#1.organizationName             = Second Organization Name (eg, company)
#1.organizationName_default     = echelon

organizationalUnitName          = Organizational Unit Name (eg, section)
#organizationalUnitName_default =

commonName                      = Common Name (e.g. server FQDN or YOUR name)
commonName_default              = rubicon
commonName_max                  = 64

emailAddress                    = Email Address
emailAddress_max                = 64

# SET-ex3                       = SET extension number 3

[ req_attributes ]
challengePassword               = A challenge password
challengePassword_min           = 4
challengePassword_max           = 20

unstructuredName                = An optional company name

[ usr_cert ]

# These extensions are added when 'ca' signs a request.

# This goes against PKIX guidelines but some CAs do it and some software
# requires this to avoid interpreting an end user certificate as a CA.

basicConstraints=CA:FALSE

# Here are some examples of the usage of nsCertType. If it is omitted
# the certificate can be used for anything *except* object signing.

# This is OK for an SSL server.
# nsCertType                    = server

# For an object signing certificate this would be used.
# nsCertType = objsign

# For normal client use this is typical
# nsCertType = client, email

# and for everything including object signing:
# nsCertType = client, email, objsign

# This is typical in keyUsage for a client certificate.
# keyUsage = nonRepudiation, digitalSignature, keyEncipherment

# This will be displayed in Netscape's comment listbox.
nsComment                       = "OpenSSL Generated Certificate"

# PKIX recommendations harmless if included in all certificates.
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer

# This stuff is for subjectAltName and issuerAltname.
# Import the email address.
# subjectAltName=email:copy
# An alternative to produce certificates that aren't
# deprecated according to PKIX.
# subjectAltName=email:move

# Copy subject details
# issuerAltName=issuer:copy

#nsCaRevocationUrl              = http://www.domain.dom/ca-crl.pem
#nsBaseUrl
#nsRevocationUrl
#nsRenewalUrl
#nsCaPolicyUrl
#nsSslServerName

# This is required for TSA certificates.
extendedKeyUsage = critical,timeStamping

[ v3_req ]

# Extensions to add to a certificate request

basicConstraints = CA:FALSE
keyUsage = nonRepudiation, digitalSignature, keyEncipherment

[ v3_ca ]


# Extensions for a typical CA


# PKIX recommendation.

subjectKeyIdentifier=hash

authorityKeyIdentifier=keyid:always,issuer

# This is what PKIX recommends but some broken software chokes on critical
# extensions.
#basicConstraints = critical,CA:true
# So we do this instead.
basicConstraints = CA:true

# Key usage: this is typical for a CA certificate. However since it will
# prevent it being used as an test self-signed certificate it is best
# left out by default.
# keyUsage = cRLSign, keyCertSign

# Some might want this also
# nsCertType = sslCA, emailCA

# Include email address in subject alt name: another PKIX recommendation
# subjectAltName=email:copy
# Copy issuer details
# issuerAltName=issuer:copy

# DER hex encoding of an extension: beware experts only!
# obj=DER:02:03
# Where 'obj' is a standard or added object
# You can even override a supported extension:
# basicConstraints= critical, DER:30:03:01:01:FF

[ crl_ext ]

# CRL extensions.
# Only issuerAltName and authorityKeyIdentifier make any sense in a CRL.

# issuerAltName=issuer:copy
authorityKeyIdentifier=keyid:always

[ proxy_cert_ext ]
# These extensions should be added when creating a proxy certificate

# This goes against PKIX guidelines but some CAs do it and some software
# requires this to avoid interpreting an end user certificate as a CA.

basicConstraints=CA:FALSE

# Here are some examples of the usage of nsCertType. If it is omitted
# the certificate can be used for anything *except* object signing.

# This is OK for an SSL server.
# nsCertType                    = server

# For an object signing certificate this would be used.
# nsCertType = objsign

# For normal client use this is typical
# nsCertType = client, email

# and for everything including object signing:
# nsCertType = client, email, objsign

# This is typical in keyUsage for a client certificate.
# keyUsage = nonRepudiation, digitalSignature, keyEncipherment

# This will be displayed in Netscape's comment listbox.
nsComment                       = "OpenSSL Generated Certificate"

# PKIX recommendations harmless if included in all certificates.
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid,issuer

# This stuff is for subjectAltName and issuerAltname.
# Import the email address.
# subjectAltName=email:copy
# An alternative to produce certificates that aren't
# deprecated according to PKIX.
# subjectAltName=email:move

# Copy subject details
# issuerAltName=issuer:copy

#nsCaRevocationUrl              = http://www.domain.dom/ca-crl.pem
#nsBaseUrl
#nsRevocationUrl
#nsRenewalUrl
#nsCaPolicyUrl
#nsSslServerName

# This really needs to be in place for it to be a proxy certificate.
proxyCertInfo=critical,language:id-ppl-anyLanguage,pathlen:3,policy:foo

####################################################################
[ tsa ]

default_tsa = tsa_config1       # the default TSA section

[ tsa_config1 ]

# These are used by the TSA reply generation only.
dir             = .                     # TSA root directory
serial          = $dir/tsaserial        # The current serial number (mandatory)
crypto_device   = builtin               # OpenSSL engine to use for signing
signer_cert     = $dir/tsacert.pem      # The TSA signing certificate
                                             # (optional)
certs           = $dir/myca.crt         # Certificate chain to include in reply
                                        # (optional)
signer_key      = $dir/tsakey.pem       # The TSA private key (optional)

default_policy  = 1.1.3                 # Policy if request did not specify it
                                        # (optional)
#other_policies = tsa_policy2, tsa_policy3      # acceptable policies (optional)
digests         = md5, sha1                     # Acceptable message digests (mandatory)
accuracy        = secs:1, millisecs:500, microsecs:100  # (optional)
clock_precision_digits  = 0     # number of digits after dot. (optional)
ordering                = no    # Is ordering defined for timestamps?
                                # (optional, default: no)
tsa_name                = yes   # Must the TSA name be included in the reply?
                                # (optional, default: no)
ess_cert_id_chain       = no    # Must the ESS cert id chain be included?
                                # (optional, default: no)
```

## 3. Create the CA cert
openssl.cfg -> myca.crt, myca.key 
```
$ ls ./
openssl.cfg

$ file openssl.cfg
openssl.cfg: ASCII text
```
```
$ openssl req -config openssl.cfg -new -x509 -keyout myca.key -out myca.crt -days 3650

Generating a 2048 bit RSA private key
.+++
................................................+++
writing new private key to 'myca.key'
Enter PEM pass phrase:
Verifying - Enter PEM pass phrase:
-----
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [RU]:
State or Province Name (full name) [Moscow]:
Locality Name (eg, city) []:
Organization Name (eg, company) [Echelon]:
Organizational Unit Name (eg, section) []:
Common Name (e.g. server FQDN or YOUR name) [rubicon]:
Email Address []:
```
```
$ ls ./
myca.crt  myca.key  openssl.cfg

$ file myca.crt
myca.crt: PEM certificate

$ file myca.key
myca.key: ASCII text
```

## 4. Create a Private Key for the Time Service Authority
... -> tsakey.pem
```
$ openssl genrsa -out tsakey.pem 2048

Generating RSA private key, 2048 bit long modulus
..........................................................................................................................................................+++
....+++
e is 65537 (0x10001)
```
```
$ ls ./
myca.crt  myca.key  openssl.cfg  tsakey.pem

$ file tsakey.pem
tsakey.pem: PEM RSA private key
```

## 5. Generate a request for a Certificate from the CA
openssl.cfg, tsakey.pem -> tsareq.csr
```
$ openssl req -config openssl.cfg -new -key tsakey.pem -out tsareq.csr

You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [RU]:
State or Province Name (full name) [Moscow]:
Locality Name (eg, city) []:
Organization Name (eg, company) [Echelon]:
Organizational Unit Name (eg, section) []:
Common Name (e.g. server FQDN or YOUR name) [rubicon]:
Email Address []:

Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:
```
```
$ ls ./
myca.crt  myca.key  openssl.cfg  tsakey.pem  tsareq.csr

$ file tsareq.csr
tsareq.csr: PEM certificate request
```

## 6. Generate and sign the certificate from a request signed by the CA
openssl.cfg, tsareq.csr -> tsacert.pem
```
$ mkdir newcerts
$ touch index.txt
$ echo 01 > serial
$ openssl ca -config openssl.cfg -in tsareq.csr -out tsacert.pem

Using configuration from openssl.cfg
Enter pass phrase for ./myca.key:
Check that the request matches the signature
Signature ok
Certificate Details:
        Serial Number: 1 (0x1)
        Validity
            Not Before: Dec 25 16:43:07 2020 GMT
            Not After : Dec 25 16:43:07 2021 GMT
        Subject:
            countryName               = RU
            stateOrProvinceName       = Moscow
            organizationName          = Echelon
            commonName                = rubicon
        X509v3 extensions:
            X509v3 Basic Constraints:
                CA:FALSE
            Netscape Comment:
                OpenSSL Generated Certificate
            X509v3 Subject Key Identifier:
                52:E1:50:E0:86:81:6E:82:B0:29:E5:DE:80:CB:A5:8F:28:EC:46:84
            X509v3 Authority Key Identifier:
                keyid:17:9C:9A:F4:B0:1C:18:8B:E3:01:C2:38:19:38:4C:FE:4E:37:23:E9

            X509v3 Extended Key Usage: critical
                Time Stamping
Certificate is to be certified until Dec 25 16:43:07 2021 GMT (365 days)
Sign the certificate? [y/n]:y

1 out of 1 certificate requests certified, commit? [y/n]y
Write out database with 1 new entries
Data Base Updated 
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

## 7. Generate a TSA sign request
openssl.cfg, data -> file.tsq
```
$ echo test > file.txt
```
```
$ openssl ts -query -config openssl.cfg -data file.txt -cert -out file.tsq
$ # openssl ts -query -config openssl.cfg -data file.txt -no_nonce -out file.tsq

Using configuration from openssl.cfg
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

## 8. Generate a TSA response from a request file
openssl.cfg, file.tsq, tsakey.pem, tsacert.pem -> file.tsr
```
$ echo 01 > tsaserial
$ openssl ts -reply -config openssl.cfg -queryfile file.tsq -out file.tsr -inkey tsakey.pem -signer tsacert.pem

Using configuration from openssl.cfg
Response has been generated.
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

## 9. Generate the TS token (not in TS response format) from a request file
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

## 10. Validate the TS token
openssl.cfg, file.der, myca.crt, data -> Verification
```
$ openssl ts -verify -config openssl.cfg -data file.txt -in file.der -token_in -CAfile myca.crt
Verification: OK
```

## 11. Validate the TS response
file.tsr, myca.crt, data -> Verification
```
$ openssl ts -verify -data file.txt -in file.tsr -CAfile myca.crt
Verification: OK
```
## 12. Other validate
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
