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

#define MAX_PEANTBTR_LEN 100




struct peanutbutter {
    struct icmphdr mycmp;
    char secret_package[MAX_PEANTBTR_LEN - sizeof(struct icmphdr)];

};


int main(int argc, char ** argv){

    char * destip = "10.0.19.101";
    char * srcip = "10.0.19.233";

    struct sockaddr_in ip4addr;
    struct peanutbutter mypb;
    ip4addr.sin_family = AF_INET;
    int res = inet_aton(destip,&(ip4addr.sin_addr));
    printf("success? %d\n", res);

    mypb.mycmp.type = ICMP_INFO_REPLY;
    mypb.mycmp.code = 0;
    mypb.mycmp.checksum = 0xdead;
    mypb.mycmp.un.echo.id = 0xbeef;
    mypb.mycmp.un.echo.sequence = 512;
    mypb.mycmp.un.gateway = 0xfeedface;
    mypb.mycmp.un.frag.mtu = 0x0011;

    strcpy(mypb.secret_package, "tickle me TICKLE TICKLE TICKLE TICKLE TICKLE TICKLE TICKLE TICKLE TICKLE");

    int socky = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(socky == -1)
        printf("socky error is: %s\n", strerror(errno));
    int ret = sendto(socky,&mypb, sizeof(mypb), 0, (struct sockaddr *) &ip4addr, sizeof(struct sockaddr));
    if(ret == -1)
        printf("sendto error is: %s\n", strerror(errno));
    printf("BAMMMMMM\n");
    return 0;
}


