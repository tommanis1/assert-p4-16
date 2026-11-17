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


 int standard_metadata_ingress_port_eq_1_136593;

 int hdr_ipv4_dstAddr_eq_2_136593;

void NoAction_1_136315();
void core_pass_through_0_136510();
void set_source_color_0_136333();
void get_source_color_136351();
void forward_136527();
void accept();
void set_remote_dest_0_136432();
void NoAction_0_136305();
void start();
void NoAction_6_136316();
void parse_ipv4_option();
void reject();
void set_local_dest_0_136399();
void NoAction_7_136709();
void parse_ipv4();
void color_check_136588();
void place_holder_table_136710();
void drop_0_136317();
void parse_stag();

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

typedef uint64_t macAddr_t;

typedef uint32_t ip4Addr_t;

typedef struct {
	uint8_t isValid : 1;
	macAddr_t dstAddr: 48;
	macAddr_t srcAddr: 48;
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
	ip4Addr_t srcAddr: 32;
	ip4Addr_t dstAddr: 32;
} ipv4_t;

typedef struct {
	uint8_t isValid : 1;
	uint8_t copyFlag : 1;
	uint8_t optClass : 2;
	uint8_t option : 5;
	uint8_t optionLength : 8;
} ipv4_option_t;

typedef struct {
	uint8_t isValid : 1;
	uint8_t source_color : 8;
} stag_t;

typedef struct {
	uint8_t isValid : 1;
	uint8_t src_port_color : 8;
	uint8_t dst_port_color : 8;
	uint8_t toLocal : 1;
} local_md_t;

typedef struct {
	local_md_t local_md;
} metadata;

typedef struct {
	ethernet_t ethernet;
	ipv4_t ipv4;
	ipv4_option_t ipv4_option;
	stag_t stag;
} headers;

headers hdr;
metadata meta;
standard_metadata_t standard_metadata;


void start() {
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
	if(hdr.ipv4.ihl >= 5) { exit(1); }
	if((hdr.ipv4.ihl == 5)){
		accept();
	} else {
		parse_ipv4_option();
	}
}


void parse_ipv4_option() {
	//Extract hdr.ipv4_option
	hdr.ipv4_option.isValid = 1;
	if((hdr.ipv4_option.option == 31)){
		parse_stag();
	} else {
		accept();
	}
}


void parse_stag() {
	//Extract hdr.stag
	hdr.stag.isValid = 1;
	meta.local_md.src_port_color = hdr.stag.source_color;
	accept();
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

void ingress() {
	if(!hdr.stag.isValid) {
	get_source_color_136351();
}
	forward_136527();
	if(action_run == 136399) {
		 	color_check_136588();

	}
}

// Action
void NoAction_0_136305() {
	action_run = 136305;
	
}


// Action
void NoAction_1_136315() {
	action_run = 136315;
	
}


// Action
void NoAction_6_136316() {
	action_run = 136316;
	
}


// Action
void drop_0_136317() {
	action_run = 136317;
		mark_to_drop();

}


// Action
void set_source_color_0_136333() {
	action_run = 136333;
	uint8_t color;
	klee_make_symbolic(&color, sizeof(color), "color");
	meta.local_md.src_port_color = color;

}


// Action
void set_local_dest_0_136399() {
	action_run = 136399;
	uint32_t egr_port;
	klee_make_symbolic(&egr_port, sizeof(egr_port), "egr_port");
uint8_t color;
	klee_make_symbolic(&color, sizeof(color), "color");
	standard_metadata.egress_spec = egr_port;
	meta.local_md.dst_port_color = color;
	hdr.stag.isValid = 0;

}


// Action
void set_remote_dest_0_136432() {
	action_run = 136432;
	uint32_t egr_port;
	klee_make_symbolic(&egr_port, sizeof(egr_port), "egr_port");
	standard_metadata.egress_spec = egr_port;
	hdr.ipv4_option.isValid = 1;
	hdr.ipv4_option.copyFlag = 1;
	hdr.ipv4_option.optClass = 2;
	hdr.ipv4_option.option = 31;
	hdr.ipv4_option.optionLength = 4;
	hdr.ipv4.ihl = hdr.ipv4.ihl + 1;
	hdr.stag.isValid = 1;
	hdr.stag.source_color = meta.local_md.src_port_color;

}


// Action
void core_pass_through_0_136510() {
	action_run = 136510;
	uint32_t egr_port;
	klee_make_symbolic(&egr_port, sizeof(egr_port), "egr_port");
	standard_metadata.egress_spec = egr_port;

}


//Table
void get_source_color_136351() {
	// keys: standard_metadata.ingress_port:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: set_source_color_0_136333(); break;
		default: NoAction_0_136305(); break;
	}
	// default_action NoAction_0_136305();

}


//Table
void forward_136527() {
	// keys: hdr.ipv4.dstAddr:ternary
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: set_local_dest_0_136399(); break;
		case 1: set_remote_dest_0_136432(); break;
		case 2: core_pass_through_0_136510(); break;
		default: NoAction_1_136315(); break;
	}
	// size 1024
	// default_action NoAction_1_136315();

}


//Table
void color_check_136588() {
standard_metadata_ingress_port_eq_1_136593 = (standard_metadata.ingress_port  ==  1);
	hdr_ipv4_dstAddr_eq_2_136593 = (hdr.ipv4.dstAddr  ==  167772162);
		// keys: meta.local_md.dst_port_color:exact, meta.local_md.src_port_color:exact
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		case 0: drop_0_136317(); break;
		default: NoAction_6_136316(); break;
	}
	// size 1024
	// default_action drop_0_136317();

}



//Control

void egress() {
	place_holder_table_136710();
}

// Action
void NoAction_7_136709() {
	action_run = 136709;
	
}


//Table
void place_holder_table_136710() {
	int symbol;
	klee_make_symbolic(&symbol, sizeof(symbol), "symbol");
	switch(symbol) {
		default: NoAction_7_136709(); break;
	}
	// size 2
	// default_action NoAction_7_136709();

}



//Control

void computeChecksum() {
	
}


//Control

void verifyChecksum() {
	
}


//Control

void DeparserImpl() {
	//Emit hdr.ethernet
	
	//Emit hdr.ipv4
	
	//Emit hdr.ipv4_option
	
	//Emit hdr.stag
	
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
	//if(!(!((standard_metadata_ingress_port_eq_1_136593) && (hdr_ipv4_dstAddr_eq_2_136593)) || (!(assert_forward)))) assert_error("!((standard_metadata_ingress_port_eq_1_136593) && (hdr_ipv4_dstAddr_eq_2_136593)) || (!(assert_forward))");
}



