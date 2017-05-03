#include<net/ethernet.h>
#include <pcap/pcap.h>
#include <sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include<netinet/ip_icmp.h>
char errbuf[100];

static char my_ip_buff[20] =  { NULL };
/*pcap_t *pcap_open_live(const char *device, int snaplen,*/
       /*int promisc, int to_ms, char *errbuf);*/
char * get_my_ip(void);

void processer(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer);

int main(){

    char *my_ip = get_my_ip();
    strcpy(my_ip_buff, my_ip);

    pcap_if_t *devicepois;
    if(pcap_findalldevs(&devicepois, errbuf) == -1){
        printf("Oh shoooot!!! No devicesss!");
        return -1;
    }
    pcap_if_t *dev =  devicepois;
    for(;dev!=NULL; dev=dev->next){
        if(!strcmp(dev->name, "wlan0"))
            break;
    }
    printf("\nDevice is %s", dev->name);
    pcap_t * handle = pcap_open_live(dev->name, 65000, 1, 0, errbuf);
    if(handle == NULL){
        printf("Oops! Couldn't open the device %s\n", dev->name);
        return -1;
    }
    /*int header_type = pcap_datalink(handle);*/
    /*printf("the header type is -> %d\n", header_type);*/
    /*int header_type_setret = pcap_set_datalink(handle, header_type);*/
    /*printf("the header type set is -> %d\n", header_type_setret);*/
    pcap_loop(handle, -1, processer, NULL);
}

char * get_my_ip(void){
    int fd;
    struct ifreq ifr;

    char iface[] = "wlan0";

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;

    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    //display result
    return inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr);

}

void processer(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer){
    struct ip *iph = (struct ip *)(buffer + sizeof(struct ethhdr));
    int prot = iph->ip_p;
    char *dst_ip = inet_ntoa(iph->ip_dst);
    if(!strcmp(dst_ip, my_ip_buff) && prot == 1) {
        struct icmphdr *icmp = (struct icmphdr *)(buffer + 34);
        printf("headerlen %d caplen %d", header->len, header->caplen);
        printf("type:%d code:%d checksum:%x seq:%d\n", icmp->type, icmp->code, icmp->checksum, icmp->un.echo.sequence);
        for(int i = 50; i < header->caplen; i++){
            char c = buffer[i];
            /*if(c>=65 && c<=127)*/
            printf("%x", c & 0xff);
        }
        printf("\n\n");
    }
}
