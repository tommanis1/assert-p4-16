#define BITSLICE(x, a, b) ((x) >> (b)) & ((1 << ((a)-(b)+1)) - 1)
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<assert.h>

// KLEE function stubs
void klee_make_symbolic(void *addr, unsigned long nbytes, const char *name);
void klee_abort(void);
void klee_assume(int condition);
void klee_print_once(int id, const char *msg);

// Checksum function stubs (no-op for verification)
void verify_checksum(void) {}
void update_checksum(void) {}

int assert_forward = 1;
int action_run;

void end_assertions();


 int hdr_ipv4_dstAddr_eq_7_114129;

 int hdr_rtp_timestamp_eq_3_114129;

 int hdr_rtp_timestamp_eq_4_114129;

void take_video_114071();
void parse_udp();
void schedule_table_114124();
void accept();
void start();
void parse_ethernet();
void reject();
void _drop_114101();
void parse_ipv4();
void NoAction_0_114047();
void parse_rtp();

typedef struct {
	uint32_t ingress_port : 9;
	uint32_t egress_spec : 9;
	uint32_t egress_port : 9;
	uint32_t clone_spec : 32;
	uint32_t instance_type : 32;
	uint8_t drop : 1;
	uint32_t recirculate_port : 16;
	uint32_t packet_length : 32;
	uint32_t enq_timestamp : 32;
	uint32_t enq_qdepth : 19;
	uint32_t deq_timedelta : 32;
	uint32_t deq_qdepth : 19;
	uint64_t ingress_global_timestamp : 48;
	uint32_t lf_field_list : 32;
	uint32_t mcast_grp : 16;
	uint8_t resubmit_flag : 1;
	uint32_t egress_rid : 16;
	uint8_t checksum_error : 1;
} standard_metadata_t;

void mark_to_drop() {
	assert_forward = 0;
	end_assertions();
	exit(0);
}

typedef struct {
	uint8_t isValid : 1;
	uint64_t dstAddr : 48;
	uint64_t srcAddr : 48;
	uint32_t etherType : 16;
} ethernet_t;

typedef struct {
	uint8_t isValid : 1;
	uint8_t version : 4;
	uint8_t ihl : 4;
	uint8_t diffserv : 8;
	uint32_t totalLen : 16;
	uint32_t identification : 16;
	uint8_t flags : 3;
	uint32_t fragOffset : 13;
	uint8_t ttl : 8;
	uint8_t protocol : 8;
	uint32_t hdrChecksum : 16;
	uint32_t srcAddr : 32;
	uint32_t dstAddr : 32;
} ipv4_t;

typedef struct {
	uint8_t isValid : 1;
	uint8_t version : 2;
	uint8_t padding : 1;
	uint8_t extension : 1;
	uint8_t CSRC_count : 4;
	uint8_t marker : 1;
	uint8_t payload_type : 7;
	uint32_t sequence_number : 16;
	uint32_t timestamp : 32;
	uint32_t SSRC : 32;
} rtp_t;

typedef struct {
	uint8_t isValid : 1;
	uint32_t srcPort : 16;
	uint32_t dstPort : 16;
	uint32_t hdr_length : 16;
	uint32_t checksum : 16;
} udp_t;

typedef struct {
} metadata;

typedef struct {
	ethernet_t ethernet;
	ipv4_t ipv4;
	rtp_t rtp;
	udp_t udp;
} headers;

headers hdr;
metadata meta;
standard_metadata_t standard_metadata;


void parse_ethernet() {
	//Extract hdr.ethernet
	hdr.ethernet.isValid = 1;
	if((hdr.ethernet.etherType == 2048)){
		parse_ipv4();
	} else {
		accept();
	}
}


void parse_ipv4() {
	//Extract hdr.ipv4
	hdr.ipv4.isValid = 1;
	if((hdr.ipv4.protocol == 17)){
		parse_udp();
	} else {
		accept();
	}
}


void parse_rtp() {
	//Extract hdr.rtp
	hdr.rtp.isValid = 1;
	accept();
}


void parse_udp() {
	//Extract hdr.udp
	hdr.udp.isValid = 1;
	parse_rtp();
}


void start() {
	parse_ethernet();
}


void accept() {
	
}


void reject() {
	assert_forward = 0;
	end_assertions();
	exit(0);
}


void ParserImpl() {
	klee_make_symbolic(&hdr, sizeof(hdr), "hdr");
	klee_make_symbolic(&meta, sizeof(meta), "meta");
	klee_make_symbolic(&standard_metadata, sizeof(standard_metadata), "standard_metadata");

	start();
}

//Control

void egress() {
	
}


//Control

void ingress() {
	schedule_table_114124();
}

// Action
void NoAction_0_114047() {
	action_run = 114047;
	
}


// Action
void take_video_114071() {
	action_run = 114071;
	uint32_t dst_ip;
	klee_make_symbolic(&dst_ip, sizeof(dst_ip), "dst_ip");
	standard_metadata.egress_spec = 1;
	hdr.ipv4.dstAddr = dst_ip;

}


// Action
void _drop_114101() {
	action_run = 114101;
		mark_to_drop();

}


//Table
void schedule_table_114124() {
hdr_ipv4_dstAddr_eq_7_114129 = (hdr.ipv4.dstAddr  ==  4009820417);
	hdr_rtp_timestamp_eq_3_114129 = (hdr.rtp.timestamp  ==  3);
	hdr_rtp_timestamp_eq_4_114129 = (hdr.rtp.timestamp  ==  4);
		int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: take_video_114071(); break;
		case 1: _drop_114101(); break;
		default: NoAction_0_114047(); break;
	}
	// keys: hdr.ipv4.dstAddr:exact, hdr.rtp.timestamp:range
	// size 16384
	// default_action NoAction_0_114047();

}



//Control

void DeparserImpl() {
	//Emit hdr.ethernet
	
	//Emit hdr.ipv4
	
	//Emit hdr.udp
	
	//Emit hdr.rtp
	
}


//Control

void verifyChecksum() {
	
}


//Control

void computeChecksum() {
	
}


int main() {
	ParserImpl();
	ingress();
	egress();
	DeparserImpl();
	end_assertions();
	return 0;
}

void assert_error(char msg[]) {
	printf("Assertion Error: %s", msg);
	klee_abort();
}

void end_assertions(){
	//if(!(!((assert_forward) && (hdr_ipv4_dstAddr_eq_7_114129)) || (!((hdr_rtp_timestamp_eq_3_114129) || (hdr_rtp_timestamp_eq_4_114129))))) assert_error("!((assert_forward) && (hdr_ipv4_dstAddr_eq_7_114129)) || (!((hdr_rtp_timestamp_eq_3_114129) || (hdr_rtp_timestamp_eq_4_114129)))");
}



