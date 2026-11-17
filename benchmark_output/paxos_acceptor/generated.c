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


void acceptor_tbl_169692();
void place_holder_table_169903();
void forward_0_169747();
void handle_1a_0_169534();
void parse_udp();
void _drop_2_169496();
void accept();
void start();
void parse_paxos();
void transport_tbl_169774();
void _drop_0_169480();
void NoAction_0_169893();
void reject();
void read_round_0_169503();
void parse_ipv4();
void handle_2a_0_169613();

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

typedef uint64_t EthernetAddress;

typedef uint32_t IPv4Address;

typedef uint32_t PortId;

typedef struct {
	uint8_t isValid : 1;
	EthernetAddress dstAddr: 48;
	EthernetAddress srcAddr: 48;
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
	IPv4Address srcAddr: 32;
	IPv4Address dstAddr: 32;
} ipv4_t;

typedef struct {
	uint8_t isValid : 1;
	uint32_t srcPort : 16;
	uint32_t dstPort : 16;
	uint32_t length_ : 16;
	uint32_t checksum : 16;
} udp_t;

typedef struct {
	uint8_t isValid : 1;
	uint32_t msgtype : 16;
	uint32_t inst : 32;
	uint32_t rnd : 16;
	uint32_t vrnd : 16;
	uint32_t acptid : 16;
	uint32_t paxoslen : 32;
	uint64_t paxosval : 64;
} paxos_t;

typedef struct {
	ethernet_t ethernet;
	ipv4_t ipv4;
	udp_t udp;
	paxos_t paxos;
} headers;

typedef struct {
	uint32_t round : 16;
	uint8_t set_drop : 1;
	uint8_t ack_count : 8;
	uint8_t ack_acceptors : 8;
} paxos_metadata_t;

typedef struct {
	paxos_metadata_t paxos_metadata;
} metadata;

headers hdr;
metadata meta;
standard_metadata_t standard_metadata;


void start() {
	//Extract hdr.ethernet
	hdr.ethernet.isValid = 1;
	if((hdr.ethernet.etherType == 2048)){
		parse_ipv4();
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


void parse_udp() {
	//Extract hdr.udp
	hdr.udp.isValid = 1;
	if((hdr.udp.dstPort == 34952)){
		parse_paxos();
	} else {
		accept();
	}
}


void parse_paxos() {
	//Extract hdr.paxos
	hdr.paxos.isValid = 1;
	accept();
}


void accept() {
	
}


void reject() {
	assert_forward = 0;
	end_assertions();
	exit(0);
}


void TopParser() {
	klee_make_symbolic(&hdr, sizeof(hdr), "hdr");
	klee_make_symbolic(&meta, sizeof(meta), "meta");
	klee_make_symbolic(&standard_metadata, sizeof(standard_metadata), "standard_metadata");

	start();
}

//Control

void TopDeparser() {
	//Emit hdr.ethernet
	
	//Emit hdr.ipv4
	
	//Emit hdr.udp
	
	//Emit hdr.paxos
	
}


typedef struct {
	uint8_t field : 4;
	uint8_t field_0 : 4;
	uint8_t field_1 : 8;
	uint32_t field_2 : 16;
	uint32_t field_3 : 16;
	uint8_t field_4 : 3;
	uint32_t field_5 : 13;
	uint8_t field_6 : 8;
	uint8_t field_7 : 8;
	uint32_t field_8 : 32;
	uint32_t field_9 : 32;
} tuple_0;

//Control

void verifyChecksum() {
	
}


//Control

void computeChecksum() {
	
}


//Control

void ingress() {
	if(hdr.ipv4.isValid) {
	if(hdr.paxos.isValid) {
		read_round_0_169503();
	if(hdr.paxos.rnd >= meta.paxos_metadata.round) {
		acceptor_tbl_169692();
	transport_tbl_169774();

}

}
}
}

// Action
void _drop_0_169480() {
	action_run = 169480;
		mark_to_drop();

}


// Action
void _drop_2_169496() {
	action_run = 169496;
		mark_to_drop();

}


// Action
void read_round_0_169503() {
	action_run = 169503;
		
	uint64_t t39e87857_5cd6_40f5_aca8_42d19138561d;
	klee_make_symbolic(&t39e87857_5cd6_40f5_aca8_42d19138561d, sizeof(t39e87857_5cd6_40f5_aca8_42d19138561d), "t39e87857_5cd6_40f5_aca8_42d19138561d");
	meta.paxos_metadata.round = t39e87857_5cd6_40f5_aca8_42d19138561d;

	meta.paxos_metadata.set_drop = 1;

}


// Action
void handle_1a_0_169534() {
	action_run = 169534;
		hdr.paxos.msgtype = 1;
	
	uint64_t tbd9579f5_fa8f_4af1_864a_4159602403f5;
	klee_make_symbolic(&tbd9579f5_fa8f_4af1_864a_4159602403f5, sizeof(tbd9579f5_fa8f_4af1_864a_4159602403f5), "tbd9579f5_fa8f_4af1_864a_4159602403f5");
	hdr.paxos.vrnd = tbd9579f5_fa8f_4af1_864a_4159602403f5;

	
	uint64_t tbb7b863b_0796_4d67_8a0f_0d58f71546e8;
	klee_make_symbolic(&tbb7b863b_0796_4d67_8a0f_0d58f71546e8, sizeof(tbb7b863b_0796_4d67_8a0f_0d58f71546e8), "tbb7b863b_0796_4d67_8a0f_0d58f71546e8");
	hdr.paxos.paxosval = tbb7b863b_0796_4d67_8a0f_0d58f71546e8;

	
	uint64_t t48b0df8b_1c0f_4ff5_a3aa_98c15c54ac30;
	klee_make_symbolic(&t48b0df8b_1c0f_4ff5_a3aa_98c15c54ac30, sizeof(t48b0df8b_1c0f_4ff5_a3aa_98c15c54ac30), "t48b0df8b_1c0f_4ff5_a3aa_98c15c54ac30");
	hdr.paxos.acptid = t48b0df8b_1c0f_4ff5_a3aa_98c15c54ac30;

	meta.paxos_metadata.set_drop = 0;

}


// Action
void handle_2a_0_169613() {
	action_run = 169613;
		hdr.paxos.msgtype = 3;
	
	uint64_t t020b58e3_9967_4fe6_91a2_ecd343278bbe;
	klee_make_symbolic(&t020b58e3_9967_4fe6_91a2_ecd343278bbe, sizeof(t020b58e3_9967_4fe6_91a2_ecd343278bbe), "t020b58e3_9967_4fe6_91a2_ecd343278bbe");
	hdr.paxos.acptid = t020b58e3_9967_4fe6_91a2_ecd343278bbe;

	meta.paxos_metadata.set_drop = 0;

}


// Action
void forward_0_169747() {
	action_run = 169747;
	PortId port;
	klee_make_symbolic(&port, sizeof(port), "port");
uint32_t learnerPort;
	klee_make_symbolic(&learnerPort, sizeof(learnerPort), "learnerPort");
	standard_metadata.egress_spec = port;
	hdr.udp.dstPort = learnerPort;

}


//Table
void acceptor_tbl_169692() {
	// keys: hdr.paxos.msgtype:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: handle_1a_0_169534(); break;
		case 1: handle_2a_0_169613(); break;
		default: _drop_0_169480(); break;
	}
	// size 4
	// default_action _drop_0_169480();

}


//Table
void transport_tbl_169774() {
	// keys: meta.paxos_metadata.set_drop:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: _drop_2_169496(); break;
		default: forward_0_169747(); break;
	}
	// size 2
	// default_action _drop_2_169496();

}



//Control

void egress() {
	place_holder_table_169903();
}

// Action
void NoAction_0_169893() {
	action_run = 169893;
	
}


//Table
void place_holder_table_169903() {
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		default: NoAction_0_169893(); break;
	}
	// size 2
	// default_action NoAction_0_169893();

}



int main() {
	TopParser();
	ingress();
	egress();
	TopDeparser();
	end_assertions();
	return 0;
}

void assert_error(char msg[]) {
	printf("Assertion Error: %s", msg);
	klee_abort();
}

void end_assertions(){
}



