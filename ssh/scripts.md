# Удаленное выполнение команд по ssh

## 1. Выполнение команд SSH при авторизации через SSH-ключ
```
$ ssh-keygen -t rsa -b 2048
$ USERNAME='user'
$ HOST='10.0.4.144'
$ ssh-copy-id ${USERNAME}@${HOST}
$ cat ssh_cmd_1.sh
    #!/bin/bash
    USERNAME='user'
    HOST='10.0.4.144'
    SCRIPT='echo test_ssh_connect_1 > /tmp/test'
    ssh -l ${USERNAME} ${HOST} "${SCRIPT}"
```
## 2. Выполнеие команд SSH через обертку для интерактивных утилит
```
$ cat ssh_cmd_2.sh
    #!/usr/bin/expect -f
    set USERNAME [lrange $argv 0 0]
    set PASSWD [lrange $argv 1 1]
    set HOST [lrange $argv 2 2]
    spawn ssh ${USERNAME}@${HOST}
    match_max 100000
    expect "*?assword:*"
    send -- "${PASSWD}\r"
    send -- "\r"
    expect "*?user*"
    send -- "whoami\r"
    expect "*?user*"
    send -- "echo test_ssh_connect_2 > /tmp/test\r"
    expect "*?user*"
    send -- "exit\n"
    interact
$ USERNAME='user'
$ PASSWD='12345678'
$ HOST='10.0.4.144'
$ ./ssh_cmd_2.sh ${USERNAME} ${PASSWD} ${HOST}
```
## 3. Выполнеие команд SSH через утилиту sshpass
```
$ sudo apt-get install sshpass
$ cat ssh_cmd_3.sh
    #!/bin/bash
    USERNAME='user'
    PASSWD='12345678'
    HOST='10.0.4.144'
    SCRIPT='echo test_ssh_connect_3 > /tmp/test'
    sshpass -p ${PASSWD} ssh ${USERNAME}@${HOST} "${SCRIPT}"
```
## 4. Выполнеие команд SSH с использованием библиотек и кода на python, perl, c/c++, go и прочих ЯП
