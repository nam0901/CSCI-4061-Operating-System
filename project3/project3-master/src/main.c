//main.c program to translate the virtual addresses from the virtual.txt
//into the physical frame address. Project 3 - CSci 4061

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vmemory.h"

#define INPUT_FILE "../bin/virtual.txt"
#define OUT_LRU_TLB "../bin/tlb_out.txt"
#define SIZE 256

#define FIFO_POLICY 0
#define LRU_POLICY 1
#define LRU_TLB_ENTRIES 10
#define TLB_COLUMMNS 2
#define TLB_ENTRIES 8

//Static function for extra credits
static int lru_tlb[LRU_TLB_ENTRIES][TLB_COLUMMNS] = {
	{-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
	{-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}
};

static int get_tlb_entry_lru(int v_addr);
static void populate_tlb_lru(int v_addr, int p_addr);
static void print_tlb_lru();



// number of tlb requests so far
static int num_tlb_requests = 0;

// number of tlb hits so far
static int num_tlb_hits = 0;

// lru index to keep track of lru tlb slots used
static int lru_tlb_index = 0;


int main(int argc, char* argv[])
{
	if (argc > 2) {
		printf("Too many arguments, enter up to one argument\n");
		exit(-1);
	}

	int policy = FIFO_POLICY;
	if(argc == 2) {
		const char* LRUflag = "-lru";
		if(!strcmp(argv[1], LRUflag)) policy = LRU_POLICY;
	}

	initialize_vmanager(policy);

	printf("Physical address\n");
	FILE* vfile = fopen(INPUT_FILE, "r");
	if(!vfile) return -1;
	char buf[SIZE];
	while(fgets(buf, sizeof(buf), vfile)) {
		if(!strcmp(buf, "\n")) continue;
		unsigned int vaddress = (unsigned int)strtoul(buf, NULL, 0);
		int physc_frame = fetch_physical_frame(vaddress);
		int offset = (vaddress << 20) >> 20;
		print_physical_address(physc_frame, offset);
	}
	fclose(vfile);

	if(policy == FIFO_POLICY){
		//FIFO TLB implementation
			populate_tlb(0,0);
			populate_tlb(1,1);
			populate_tlb(2,2);
			populate_tlb(3,3);
			populate_tlb(4,4);
		  populate_tlb(5,5);
			populate_tlb(6,6);
			populate_tlb(7,7);
			//Collision expected. 0,0 will be overwritten
			populate_tlb(8,8);
			get_tlb_entry(5);
			print_tlb();
	}
	else{
		//TLB LRU implementation
			populate_tlb_lru(0,0);
			populate_tlb_lru(1,1);
			populate_tlb_lru(2,2);
			populate_tlb_lru(3,3);
			populate_tlb_lru(4,4);
			populate_tlb_lru(5,5);
			populate_tlb_lru(6,6);
		  populate_tlb_lru(7,7);
			populate_tlb_lru(8,8);
			populate_tlb_lru(9,9);
			//Collision expected. 0,0 will be overwritten
			populate_tlb_lru(10,10);
			//access 1, 1 is then brought to end of queue, 2 is next to be evicted
			get_tlb_entry_lru(3);
			print_tlb_lru();

	}

	free_resources();

	return 0;
}



int get_tlb_entry_lru(int n)
{
	++num_tlb_requests;
	for(int i = 0; i < lru_tlb_index; ++i) {
		if(lru_tlb[i][0] == n) {
			++num_tlb_hits;
			int result = lru_tlb[i][1];
			for(int j = i; j < lru_tlb_index - 1; j++){
				lru_tlb[j][0] = lru_tlb[j+1][0];
				lru_tlb[j][1] = lru_tlb[j+1][1];
			}
			lru_tlb[lru_tlb_index - 1][0] = n;
			lru_tlb[lru_tlb_index - 1][1] = result;
			return result;
		}
	}
	return -1;
}

void populate_tlb_lru(int v_addr, int p_addr)
{
	// if not full
	if(lru_tlb_index < LRU_TLB_ENTRIES) {
		lru_tlb[lru_tlb_index][0] = v_addr;
		lru_tlb[lru_tlb_index][1] = p_addr;
		++lru_tlb_index;
	}
	else {
		for(int i = 0; i < LRU_TLB_ENTRIES-1;i++){
			lru_tlb[i][0] = lru_tlb[i+1][0];
			lru_tlb[i][1] = lru_tlb[i+1][1];
		}
		lru_tlb[LRU_TLB_ENTRIES-1][0] = v_addr;
		lru_tlb[LRU_TLB_ENTRIES-1][1] = p_addr;
	}
}


void print_tlb_lru(){
	const char* mode = "a";
	FILE *fp = fopen(OUT_LRU_TLB, mode);
	if(!fp) return;


	for(int i = 0; i < LRU_TLB_ENTRIES; ++i)
		fprintf(fp, "0x%05x 0x%05x\n", 0x000fffff & lru_tlb[i][0], 0x000fffff & lru_tlb[i][1]);

	fprintf(fp, "\n");

	fclose(fp);
}
