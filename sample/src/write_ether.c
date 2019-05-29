#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <error.h>
#include <dlfcn.h>
#include <linux/if.h>
#include <sys/ioctl.h>

#define TYPE 0x88b5


int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: write_ether <ifname> <msg>\n");
		exit(1);
	}
	
	struct ifreq if_req;
	struct sockaddr_ll daem_addr_ll;
	int daem_sock;
	
	if ((daem_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		perror("socket");
		return errno;
	}

	memset(&daem_addr_ll, 0, sizeof(struct sockaddr_ll));
	strncpy(if_req.ifr_name, argv[1], strlen(argv[1]));

	if (ioctl(daem_sock, SIOCGIFINDEX, &if_req) < 0) {
		perror("ioctl");
		close(daem_sock);
		return errno;
	}
	daem_addr_ll.sll_family = PF_PACKET;
	daem_addr_ll.sll_protocol = htons(ETH_P_ALL);
	daem_addr_ll.sll_ifindex = if_req.ifr_ifindex; 

	if (bind(daem_sock, (struct sockaddr *)&daem_addr_ll, sizeof(struct sockaddr_ll)) < 0) {
		perror("bind");
		return errno;
	}

	if (ioctl(daem_sock, SIOCGIFFLAGS, &if_req) < 0) {
		perror("ioctl");
		close(daem_sock);
		return errno;
	}
	
	if_req.ifr_flags = if_req.ifr_flags|IFF_PROMISC|IFF_UP;
	if (ioctl(daem_sock, SIOCSIFFLAGS, &if_req) < 0) {
		perror("ioctl");
		close(daem_sock);
		return errno;
	}

	int fd;
	struct ifreq ifr2;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr2.ifr_addr.sa_family = AF_INET;
	strncpy(ifr2.ifr_name, argv[1], IFNAMSIZ-1);

	ioctl(fd, SIOCGIFHWADDR, &ifr2);

	close(fd);

	//ifr2.ifr_hwaddr.sa_data[0];

	struct ether_header eh;
	uint8_t smac[6] = {0xa2,0x73,0x57,0x27,0xa7,0x14};
#if 1
	memcpy(eh.ether_shost, smac, 6);
	memcpy(eh.ether_dhost, ifr2.ifr_hwaddr.sa_data, 6);
#else
	uint8_t dmac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
	memcpy(eh.ether_dhost, dmac, 6);
#endif
	eh.ether_type = htons(TYPE); 
	char data[100] = {0};

	memcpy(data, &eh, sizeof(struct ether_header));
	memcpy(data+sizeof(struct ether_header), argv[2], strlen(argv[2]));

	printf("ifname: %s\n", argv[1]);
	printf("dhost: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			eh.ether_dhost[0], eh.ether_dhost[1], eh.ether_dhost[2],
			eh.ether_dhost[3], eh.ether_dhost[4], eh.ether_dhost[5]);
	printf("shost: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			eh.ether_shost[0], eh.ether_shost[1], eh.ether_shost[2],
			eh.ether_shost[3], eh.ether_shost[4], eh.ether_shost[5]);
	printf("type: nw-order %.4x (host-oder %.4x)\n",  
			htons(eh.ether_type), 
			ntohs(eh.ether_type));


	write(daem_sock, data, sizeof(data));
}
