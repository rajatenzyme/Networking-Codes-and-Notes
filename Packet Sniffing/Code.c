#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/ether.h>
#include <net/ethernet.h> 
#include <string.h>


void callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char*
        packet)
{
  static int count = 1;

  printf("\nPacket number [%d], length of this packet is: %d\n", count++, pkthdr->len);
  struct ether_header *eptr = (struct ether_header *)packet;
  fprintf(stdout,"ethernet header source: %s"
            ,ether_ntoa((const struct ether_addr *)&eptr->ether_shost));
            fprintf(stdout,"ethernet header destination: %s"
            ,ether_ntoa((const struct ether_addr *)&eptr->ether_dhost));
  struct ip *iphdr = (struct ip*)(packet + sizeof(struct ether_header));
  //printf("\n Packet Buffer [%s]", packet);
  
  //use struct iphdr and struct tcphdr to read TCP and IP headers
}

int main(int argc,char **argv)
{
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    struct bpf_program fp;        /* to hold compiled program */
    bpf_u_int32 pMask;            /* subnet mask */
    bpf_u_int32 pNet;             /* ip address*/
    pcap_if_t *alldevs, *d;
    char dev_buff[64] = {0};
    int i =0;

    // Check if sufficient arguments were supplied
    if(argc != 3)
    {
        printf("\nInsufficient Arguments \nUsage: %s [protocol][number-of-packets]\n",argv[0]);
        return 0;
    }

    // Prepare a list of all the devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
        exit(1);
    }

    // Print the list to user
    // so that a choice can be
    // made
    printf("\nHere is a list of available devices on your system:\n\n");
    for(d=alldevs; d; d=d->next)
    {
        printf("%d. %s", ++i, d->name);
        if (d->description || d->flags)
        {
            printf(" (%s), (FLag Value-%d) \n", d->description, d->flags);
            printf("%d, %d, %d, %d, %d\n", PCAP_IF_UP, PCAP_IF_CONNECTION_STATUS, PCAP_IF_LOOPBACK, PCAP_IF_RUNNING, PCAP_IF_WIRELESS);
            if(d->flags & PCAP_IF_UP)
            printf("Interface up \n");
            if(d->flags & PCAP_IF_RUNNING)
            printf("Interface running \n");
            if(d->flags & PCAP_IF_CONNECTION_STATUS_CONNECTED)
             printf("Connected \n");
            if(d->flags & PCAP_IF_LOOPBACK)
            printf("lOOPBACK \n");
            if(d->flags & PCAP_IF_WIRELESS)
            printf("WIRELESS \n");
             if(d->flags & PCAP_IF_CONNECTION_STATUS_DISCONNECTED)
            printf("DISCONNECTED \n");
            if(d->flags & PCAP_IF_CONNECTION_STATUS_UNKNOWN)
            printf("Unknown \n");
            
            
        }
            
        else
        {
            printf(" (Sorry, No description available for this device)\n");
            printf(" (%d) \n", d->flags);
        }
    }

    // Ask user to provide the interface name
    printf("\nEnter the interface name on which you want to run the packet sniffer : ");
    fgets(dev_buff, sizeof(dev_buff)-1, stdin);

    // Clear off the trailing newline that
    // fgets sets
    dev_buff[strlen(dev_buff)-1] = '\0';

    // Check if something was provided
    // by user
    if(strlen(dev_buff))
    {
        dev = dev_buff;
        printf("\n ---You opted for device [%s] to capture [%d] packets---\n\n Starting capture...",dev, (atoi)(argv[2]));
    }     

    // If something was not provided
    // return error.
    if(dev == NULL)
    {
        printf("\n[%s]\n", errbuf);
        return -1;
    }

    // fetch the network address and network mask
    pcap_lookupnet(dev, &pNet, &pMask, errbuf);
    int mask[4];
    mask[0]=(pMask & 0xff000000)>>24;    
    mask[1]=(pMask & 0x00ff0000)>>16;
    mask[2]=(pMask & 0x0000ff00)>>8;
    mask[3]=(pMask & 0x000000ff);
    
    int octet[4]; 
    octet[0]=(pNet & 0xff000000)>>24;
    octet[1]=(pNet & 0x00ff0000)>>16;
    octet[2]=(pNet & 0x0000ff00)>>8;
    octet[3]=(pNet & 0x000000ff);
    
    printf("Network [ %d.%d.%d.%d ], Mask [ %d.%d.%d.%d ]", octet[3], octet[2], octet[1], octet[0], mask[3], mask[2], mask[1], mask[0]);

    // Now, open device for sniffing
    descr = pcap_open_live(dev, BUFSIZ, 1,-1, errbuf);
    if(descr == NULL)
    {
        printf("pcap_open_live() failed due to [%s]\n", errbuf);
        return -1;
    }

    // Compile the filter expression
    if(pcap_compile(descr, &fp, argv[1], 0, pNet) == -1)
    {
        printf("\npcap_compile() failed\n");
        return -1;
    }

    // Set the filter compiled above
    if(pcap_setfilter(descr, &fp) == -1)
    {
        printf("\npcap_setfilter() failed\n");
        exit(1);
    }

    // For every packet received, call the callback function
    // For now, maximum limit on number of packets is specified
    // by user.
    pcap_loop(descr,atoi(argv[2]), callback, NULL);

    printf("\nDone with packet sniffing!\n");
    return 0;
}
