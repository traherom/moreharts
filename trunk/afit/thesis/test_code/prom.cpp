// Networking headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/socket.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <netinet/in.h>

//#include <pcap.h>

#define DEBUG 

#define MAX_MTU 2300
#define IP_HEADER_SIZE 40
#define ETH_MIN_FRAME_SIZE_CHECK 46
typedef unsigned char byte;

/*
 * Returns a socket in monitor mode
 */
int get_promisc_socket(int iface) {
	int sockfd = -1;
	int err = -1;
	struct packet_mreq	mr;
	
	// Make socket and set to promisc
	sockfd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	if(!sockfd) {
		// Unable to open socket. We suck
		return 0;
	}
	
	memset(&mr, 0, sizeof(mr));
	mr.mr_ifindex = iface;
	mr.mr_type = PACKET_MR_PROMISC;
	
	if (setsockopt(sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
		// Unable to set. Preserve errno across close
		err = errno;
		close(sockfd);
		errno = err;
		return 0;
	}
	
	return sockfd;
}

/*
 * Loops until cnt bytes are received and places them in
 * buf. Returns -1 on error or the true number of bytes read
 */
int recv_count(int sockfd, byte *buf, int cnt) {
	int total = 0;
	int received = 0;
	
	while(total < cnt) {
		received = recv(sockfd, buf + total, cnt - total, 0);
		if(received > 0)
			total += received;
		else if(received == 0)
			break;
		else
			return received;
	}

	return total;
}

/*
 * Displays the given buffer's contents
 */
void print_buf(const char *label, byte *buf, int size) {
	// Show packet contents
	#ifdef DEBUG
	if(label)
		fprintf(stderr, "%s\n", label);

	fprintf(stderr, "[  0]: ");
	for(int i = 0; i < size; i++) {
		fprintf(stderr, "%2x ", buf[i]);
		if(i % 10 == 9)
			fprintf(stderr, "\n[%3d]: ", i / 10 + 1);
	}
	fprintf(stderr, "\n");
	#endif
}

/*
 * Receive packet from socket. Assumes Ethernet II frames,
 * only gets IPv4 and v6 packets.
 * Returns actual size of received packet.
 */
int receive_packet(int sockfd, byte *buf, int buf_size) {
	int ip_len = 0;
	int len = 0;

	// Check params
	if(!buf) {
		fprintf(stderr, "Unable to read packet with null buffer\n");
		return NULL;
	}
	if(buf_size < IP_HEADER_SIZE) {
		fprintf(stderr, "Not enough space given for packet header, must be at least %d\n",
			IP_HEADER_SIZE);
		return NULL;
	}

	// Get frame
	len = recv(sockfd, buf, buf_size, 0);
	if(len == 0)
		return 0;
	else if(len == -1)
		return NULL;
	
	// Determine the actual length of the packet, in case it's longer
	// due to the minimum frame size. Only apply if we're a short packet
	if(len > ETH_MIN_FRAME_SIZE_CHECK) {
		ip_len = len;
	}
	else {
		int version = buf[0] >> 4;
		if(version == 4) {
			int payload_len = buf[2] << 8;
			payload_len |= (unsigned int)buf[3];
			ip_len = payload_len;
		}
		else if(version == 6) {
			int payload_len = buf[4] << 8;
			payload_len |= (unsigned int)buf[5];
			ip_len = IP_HEADER_SIZE + payload_len;
		}
		else {
			// If this version isn't IP 4/6, then just pass it on as-is
			ip_len = len;
		}
	}

	// Debug print
	print_buf("Packet contents", buf, ip_len);

	// Number of actual bytes in buffer
	return ip_len;
}

int main(int argc, char **argv) {
	int sockfd = -1;
	byte *buf = NULL;
	int rdcount = -1;
	
	// Open external and internal cards
	sockfd = get_promisc_socket(1);
	if(!sockfd) {
		switch(errno) {
			case EPERM:
				fprintf(stderr, "Permission denied opening listener\n");
				break;
		
			case EACCES:
				fprintf(stderr, "Access error opening listener\n");
				break;
		
			case ENODEV:
				fprintf(stderr, "Network device not found\n");
				break;
		
			default:
				fprintf(stderr, "Unable to open socket for listening, error %d\n", errno);
		}
		
		return 1;
	}

	buf = (byte*)malloc(MAX_MTU);
	if(!buf) {
		fprintf(stderr, "Unable to allocate memory\n");
		return 2;
	}

	printf("BEGIN\n");
	int cnt = 0;
	while(cnt = receive_packet(sockfd, buf, MAX_MTU)) {
		printf("Readable:\n");
		for(int i = 0; i < cnt; i++) {
			if(isalnum(buf[i]))
				printf("%c", buf[i]);
		}
		printf("\n");
	}
	printf("END\n");

	// All done
	free(buf);
	close(sockfd);
	
	return 0;
}

