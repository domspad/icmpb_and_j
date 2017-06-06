
CCFLAGS = -ggdb

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
	CCFLAGS += -lpcap
endif
ifeq ($(UNAME_S), Linux)
	CCFLAGS += -std=gnu99
endif


all:
	gcc $(CCFLAGS) icmp_chat.c -o icmp_chat -lpcap
	gcc $(CCFLAGS) icmp_shooter.c -o icmp_shooter

clean:
	rm icmp_chat icmp_shooter

