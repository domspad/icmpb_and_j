# icmpb_and_j
Transfer files using ICMP packets. The world is your oyster.

Receiver:
```
./icmp_receive  # starts listerner... received files notified and put into directory
```

Sender:
```
base64 <file> | icmp_shooter --dest=xxx.xxx.xxx.xxx     # sends encoded file as series of icmp packets 
```
