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

#define ETH_ALEN    6       /* Octets in one ethernet addr   */
typedef unsigned short      u16;
typedef u16         __be16;

#define MAX_FILENAME_SIZE 128

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
  u_int16_t num_pkt;
  u_int16_t total_pkts;
  char filename[MAX_FILENAME_SIZE];
};

char errbuf[100];

u_int16_t num_pkt = 1;
FILE *fp;

static char my_ip_buff[20] =  { 0 };
/*pcap_t *pcap_open_live(const char *device, int snaplen,*/
       /*int promisc, int to_ms, char *errbuf);*/
char * get_my_ip(void);

void processer(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer);

int main(){
    fp = fopen("outfile.txt","a");

    char *my_ip = get_my_ip();
    strcpy(my_ip_buff, my_ip);

    pcap_if_t *devicepois;
    if(pcap_findalldevs(&devicepois, errbuf) == -1){
        fprintf(stderr,"Oh shoooot!!! No devicesss!");
        return -1;
    }
    pcap_if_t *dev =  devicepois;
    for(;dev!=NULL; dev=dev->next){
        if(!strcmp(dev->name, "dns0"))
            break;
    }
    pcap_t * handle = pcap_open_live(dev->name, 65000, 1, 0, errbuf);
    if(handle == NULL){
        fprintf(stderr,"Oops! Couldn't open the device %s\n", dev->name);
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

    char iface[] = "dns0";

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
    struct ouricmphdr *icmp = (struct ouricmphdr *)(buffer+20);
    fprintf(stderr, "packet %d of %d packets", icmp->num_pkt, icmp->total_pkts);
    if(num_pkt != icmp->num_pkt){
        fprintf(stderr, "got wrong seq packet! Got %d instead of %d!", icmp->num_pkt, num_pkt);
        exit(1);
    }
    num_pkt++;
    if(icmp->peanut && icmp->peanut == 2){
        fprintf(fp, "%s", &buffer[36 + MAX_FILENAME_SIZE]);
        buffer = NULL;
    }
    if(icmp->num_pkt == icmp->total_pkts){
        rename("outfile.txt", icmp->filename);
        printf("\a");
        fclose(fp);
        exit(0);
    }
}
