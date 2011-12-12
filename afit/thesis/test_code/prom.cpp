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
#define IPV4_HEADER_SIZE 20
#define IPV6_HEADER_SIZE 40
#define ETH_MIN_FRAME_SIZE 60
#define USE_ETHERNET_II
#ifdef USE_ETHERNET_II
	#define ETH_HEADER_SIZE 14
#else
	#define ETH_HEADER_SIZE 17
#endif

typedef unsigned char byte;

/*
 * Returns a socket in promiscuous mode for the given interface
 */
int get_promisc_socket(int iface) {
	int sockfd = -1;
	int err = -1;
	struct packet_mreq	mr;
	
	// Make socket and set to promisc
	sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
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
			fprintf(stderr, "\n[%3d]: ", (i / 10 + 1) * 10);
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
	byte *move_buf = NULL;

	// Check params
	if(!buf) {
		fprintf(stderr, "Unable to read packet with null buffer\n");
		return NULL;
	}
	if(buf_size < IPV6_HEADER_SIZE) {
		fprintf(stderr, "Not enough space given for packet header, must be at least %d\n",
			IPV6_HEADER_SIZE);
		return NULL;
	}

	// Get frame
	len = recv(sockfd, buf, buf_size, 0);
	if(len == 0)
		return 0;
	else if(len == -1)
		return NULL;
	
	// Drop out the ethernet frame
	len -= ETH_HEADER_SIZE;
	memmove(buf, buf + ETH_HEADER_SIZE, len);

	// Determine the actual length of the packet, in case it's longer
	// due to the minimum frame size. Only apply if we're a short packet
	if(len + ETH_HEADER_SIZE > ETH_MIN_FRAME_SIZE) {
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
			ip_len = IPV6_HEADER_SIZE + payload_len;
		}
		else {
			// If this version isn't IP 4/6, then just pass it on as-is
			ip_len = len;
		}
	}

	// Debug print
	print_buf("Received Packet contents", buf, ip_len);

	// Number of actual bytes in buffer
	return ip_len;
}

/*
 * Constructs packet and sends it out
 */
int send_packet(int sockfd, bool is_ipv6,
		const byte *src_addr, const byte *dest_addr,
		bool is_tcp_payload, const byte *payload, int payload_size) {
	// Allocate enough space to build packet
	byte *buf = NULL;
	int buf_size = 0;
	
	if(is_ipv6)
		buf_size = IPV6_HEADER_SIZE + payload_size;
	else
		buf_size = IPV4_HEADER_SIZE + payload_size;
	
	buf = (byte*)malloc(buf_size);
	if(!buf) {
		fprintf(stderr, "Unable to allocate memory to create packet\n");
		return -1;
	}

	// Create packet based on type
	if(is_ipv6) {
		// IPv6
		// Version, traffic class, QoS
		buf[0] = 0x60;
		buf[1] = 0x00;
		buf[2] = 0x00;
		buf[3] = 0x00;
		
		// Payload length
		buf[4] = payload_size >> 8;
		buf[5] = payload_size & 0xFF;

		// Payload type. TBD, allow adding EHs
		if(is_tcp_payload)
			buf[6] = 0x06; // TCP
		else
			buf[6] = 0x11; // UDP

		// Hop limit
		buf[7] = 64;
		
		// Addresses
		memcpy(buf + 8, src_addr, 16);
		memcpy(buf + 24, src_addr, 16);
		
		// Finally, the payload itself
		memcpy(buf + IPV6_HEADER_SIZE, payload, payload_size);
	}
	else {
		// IPv4
		// Version, Header len, TOS
		buf[0] = 0x45;
		buf[1] = 0x00;

		// Total len
		buf[2] = buf_size >> 8;
		buf[3] = buf_size & 0xFF;

		// Fragment ID
		buf[4] = 0x00;
		buf[5] = 0x00;

		// Flags, fragment offset
		buf[6] = 0xDD;
		buf[7] = 0xDD;

		// TTL
		buf[8] = 64;

		// Type
		if(is_tcp_payload)
			buf[9] = 0x06; // TCP
		else
			buf[9] = 0x11; // UDP

		// Addresses
		memcpy(buf + 12, src_addr, 4);
		memcpy(buf + 16, dest_addr, 4);
		
		// Checksum... TBD
		buf[10] = 0x00;
		buf[11] = 0x00;

		int total = 0;
		for(byte *check_ptr = buf; check_ptr < buf + IPV4_HEADER_SIZE; check_ptr += 2) {
			//printf("%x = %x %x\n", total, (*check_ptr << 8), *(check_ptr + 1));
			total ^= (*check_ptr << 8) | *(check_ptr + 1);
		}

		total = ~total & 0xFFFF;
		buf[10] = total >> 8;
		buf[11] = total;
		
		// Finally, the payload itself
		memcpy(buf + IPV4_HEADER_SIZE, payload, payload_size);
	}

	print_buf("Packet to send", buf, buf_size);

	// And send
	if(send(sockfd, buf, buf_size, 0) == -1) {
		fprintf(stderr, "Failed to send packet (errno %d)\n", errno);
		return -1;
	}

	return buf_size;
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

	// Look for packets and echo their readable contents back to sender
	int cnt = 0;
	int rcnt = 0;
	byte *readable = (byte*)malloc(50);

	while(cnt = receive_packet(sockfd, buf, MAX_MTU)) {
		rcnt = 0;
		for(int i = 0; i < cnt; i++) {
			if(rcnt < 49 && isalnum(buf[i])) {
				readable[rcnt] = buf[i];
				rcnt++;
			}
		}
		readable[rcnt] = '\0';

		//send_packet(sockfd, 0, buf + 16, buf + 12, 1, readable, rcnt);
		//send_packet(sockfd, 0, buf + 16, buf + 12, 1, buf + IPV4_HEADER_SIZE, cnt - IPV4_HEADER_SIZE);
	}

	// All done
	free(buf);
	close(sockfd);
	
	return 0;
}

