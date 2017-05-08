# icmpb_and_j
Transfer files using ICMP packets. The world is your oyster.

Receiver:
```
./icmp_receive | base64 -d > outfile.txt  # starts listerner and outputs received messages to stdin
```

Sender:
```
base64 <file> | icmp_shooter --dest=xxx.xxx.xxx.xxx     # sends encoded file as series of icmp packets 
```
