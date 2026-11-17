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


void learner_tbl_173563();
void handle_2b_0_175808();
void reset_consensus_instance_173669();
void parse_udp();
void forward_0_173712();
void read_round_0_173440();
void accept();
void transport_tbl_173739();
void start();
void parse_paxos();
void NoAction_0_173894();
void reject();
void parse_ipv4();
void _drop_0_173424();
void handle_new_value_0_175926();
void place_holder_table_173904();

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
		read_round_0_173440();
	if(hdr.paxos.rnd > meta.paxos_metadata.round) {
	reset_consensus_instance_173669();
} else {
	if((hdr.paxos.rnd == meta.paxos_metadata.round)) {
	learner_tbl_173563();
}
}
	if((meta.paxos_metadata.ack_acceptors == 6) || (meta.paxos_metadata.ack_acceptors == 5) || (meta.paxos_metadata.ack_acceptors == 3)) {
	transport_tbl_173739();
}

}
}
}

// Action
void _drop_0_173424() {
	action_run = 173424;
		mark_to_drop();

}


// Action
void read_round_0_173440() {
	action_run = 173440;
		
	uint64_t t953771c6_4a2e_46ac_800d_09039efaba99;
	klee_make_symbolic(&t953771c6_4a2e_46ac_800d_09039efaba99, sizeof(t953771c6_4a2e_46ac_800d_09039efaba99), "t953771c6_4a2e_46ac_800d_09039efaba99");
	meta.paxos_metadata.round = t953771c6_4a2e_46ac_800d_09039efaba99;

	meta.paxos_metadata.set_drop = 1;
	
	uint64_t t23c3016d_df5e_45f2_831d_66647438d02c;
	klee_make_symbolic(&t23c3016d_df5e_45f2_831d_66647438d02c, sizeof(t23c3016d_df5e_45f2_831d_66647438d02c), "t23c3016d_df5e_45f2_831d_66647438d02c");
	meta.paxos_metadata.ack_acceptors = t23c3016d_df5e_45f2_831d_66647438d02c;


}


// Action
void handle_2b_0_175808() {
	action_run = 175808;
		meta.paxos_metadata.ack_acceptors = meta.paxos_metadata.ack_acceptors | 1 << (uint8_t) hdr.paxos.acptid;

}


// Action
void handle_new_value_0_175926() {
	action_run = 175926;
	
}


// Action
void forward_0_173712() {
	action_run = 173712;
	PortId port;
	klee_make_symbolic(&port, sizeof(port), "port");
uint32_t learnerPort;
	klee_make_symbolic(&learnerPort, sizeof(learnerPort), "learnerPort");
	standard_metadata.egress_spec = port;
	hdr.udp.dstPort = learnerPort;

}


//Table
void learner_tbl_173563() {
	// keys: hdr.paxos.msgtype:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		default: handle_2b_0_175808(); break;
	}
	// size 1
	// default_action handle_2b_0_175808();

}


//Table
void reset_consensus_instance_173669() {
	// keys: hdr.paxos.msgtype:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		default: handle_new_value_0_175926(); break;
	}
	// size 1
	// default_action handle_new_value_0_175926();

}


//Table
void transport_tbl_173739() {
	// keys: meta.paxos_metadata.set_drop:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: _drop_0_173424(); break;
		default: forward_0_173712(); break;
	}
	// size 2
	// default_action _drop_0_173424();

}



//Control

void egress() {
	place_holder_table_173904();
}

// Action
void NoAction_0_173894() {
	action_run = 173894;
	
}


//Table
void place_holder_table_173904() {
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		default: NoAction_0_173894(); break;
	}
	// size 2
	// default_action NoAction_0_173894();

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



