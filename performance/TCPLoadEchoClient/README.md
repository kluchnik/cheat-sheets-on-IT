# TCP Client

```
Usage: /usr/bin/tcploadechoclient -i <IP> [-i <IP> ..] -D <IP> -p <port> -n <conns> -r <pushrate> -c <connrate> [-s]

 -r <pushrate>  : Pushrate equals the number of ping "pushes" per second. This is equal to the number "data pushes" to the TCP stack per second over all connections
 -c <connrate>  : Connrate equals the number of new connections that are made each second. (by default = pushrate)
 -s             : Disables the check on the number of open files limit.
 ```
