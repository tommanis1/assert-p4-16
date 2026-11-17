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

int traverse_154662 = 0;

 int hdr_paxos_msgtype_eq_2_154662;
int traverse_154735 = 0;

void _drop_0_154623();
void _drop_2_154639();
void reset_instance_0_154704();
void parse_udp();
void leader_tbl_154736();
void NoAction_0_154919();
void accept();
void start();
void parse_paxos();
void forward_0_154791();
void transport_tbl_154818();
void reject();
void increase_instance_0_154646();
void parse_ipv4();
void place_holder_table_154929();

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
		leader_tbl_154736();
	transport_tbl_154818();

}
}
}

// Action
void _drop_0_154623() {
	action_run = 154623;
		mark_to_drop();

}


// Action
void _drop_2_154639() {
	action_run = 154639;
		mark_to_drop();

}


// Action
void increase_instance_0_154646() {
	action_run = 154646;
		traverse_154662 = 1;hdr_paxos_msgtype_eq_2_154662 = (hdr.paxos.msgtype  ==  2);
	
	
	uint64_t t14933575_866d_4b08_9b07_5e84fa4b53e3;
	klee_make_symbolic(&t14933575_866d_4b08_9b07_5e84fa4b53e3, sizeof(t14933575_866d_4b08_9b07_5e84fa4b53e3), "t14933575_866d_4b08_9b07_5e84fa4b53e3");
	hdr.paxos.inst = t14933575_866d_4b08_9b07_5e84fa4b53e3;

	hdr.paxos.inst = hdr.paxos.inst + 1;
	meta.paxos_metadata.set_drop = 0;

}


// Action
void reset_instance_0_154704() {
	action_run = 154704;
		meta.paxos_metadata.set_drop = 1;
	traverse_154735 = 1;

}


// Action
void forward_0_154791() {
	action_run = 154791;
	PortId port;
	klee_make_symbolic(&port, sizeof(port), "port");
uint32_t acceptorPort;
	klee_make_symbolic(&acceptorPort, sizeof(acceptorPort), "acceptorPort");
	standard_metadata.egress_spec = port;
	hdr.udp.dstPort = acceptorPort;

}


//Table
void leader_tbl_154736() {
	// keys: hdr.paxos.msgtype:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: increase_instance_0_154646(); break;
		case 1: reset_instance_0_154704(); break;
		default: _drop_0_154623(); break;
	}
	// size 4
	// default_action _drop_0_154623();

}


//Table
void transport_tbl_154818() {
	// keys: meta.paxos_metadata.set_drop:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: _drop_2_154639(); break;
		default: forward_0_154791(); break;
	}
	// size 2
	// default_action _drop_2_154639();

}



//Control

void egress() {
	place_holder_table_154929();
}

// Action
void NoAction_0_154919() {
	action_run = 154919;
	
}


//Table
void place_holder_table_154929() {
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		default: NoAction_0_154919(); break;
	}
	// size 2
	// default_action NoAction_0_154919();

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
	//if(!(!(traverse_154662) || (hdr_paxos_msgtype_eq_2_154662))) assert_error("!(traverse_154662) || (hdr_paxos_msgtype_eq_2_154662)");
	//if(!(!(traverse_154735) || (!(assert_forward)))) assert_error("!(traverse_154735) || (!(assert_forward))");
}



