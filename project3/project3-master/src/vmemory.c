//Implement the API, modeling the translation of virtual page address to a
//physical frame address. We assume a 32 bit virtual memory and physical memory.
//Access to the page table is only via the CR3 register.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "vmemory.h"

#define OUT_TLB "../bin/tlb_out.txt"
#define PT_1 "../bin/first_pt.txt"
#define PT_2 "../bin/second_pt.txt"

#define TLB_COLUMMNS 2
#define TLB_ENTRIES 8

#define BUFSIZE 1024
#define CR3_ROWS 1024
#define CR3_COLUMNS 1024


bool FIFO_policy = true;
int **cr3;

//
// More static functions can be implemented here
//

// tlb : the in memory modelling of the tlb
static int tlb[TLB_ENTRIES][TLB_COLUMMNS] = {
	{-1,-1}, {-1,-1}, {-1,-1}, {-1,-1},
	{-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}
};



// number of tlb requests so far
static int num_tlb_requests = 0;

// number of tlb hits so far
static int num_tlb_hits = 0;

// lru index to keep track of lru tlb slots used
static int lru_tlb_index = 0;

// The implementation of get_vpage_cr3 is provided in
// an object file, so no need to re-implement it
void initialize_vmanager(int policy)
{
	// Set LRU policy when passsed as a parameter
	if (policy)
		FIFO_policy = false;
	cr3 = get_vpage_cr3();
}

int fetch_physical_frame(unsigned int v_addr)
{
	if(v_addr < 0) return -1; //overflow

	//extract correct bits from v_addr for each index/offset

	unsigned int firstPTIndex = v_addr>>22;
	unsigned int secondPTIndex =  (v_addr<<10)>>22;

	// overflow check
	if(firstPTIndex >= CR3_ROWS || secondPTIndex >= CR3_COLUMNS) return -1;

	// check first level
	if(!cr3[firstPTIndex]) return -1; //page fault

	// check second level
	if(cr3[firstPTIndex][secondPTIndex] == -1) return -1; // page fault

	return cr3[firstPTIndex][secondPTIndex];
}

void print_physical_address(int frame, int offset)
{
	if(frame == -1) printf("-1\n");
	else printf("0x%08x\n", (frame << 12) + offset);
	fflush(stdout);
	return;
}

int get_tlb_entry(int n)
{
	++num_tlb_requests;
	for(int i = 0; i < TLB_ENTRIES; ++i) {
		if(tlb[i][0] == n) {
			++num_tlb_hits;
			return tlb[i][1];
		}
	}
	return -1;
}

void populate_tlb(int v_addr, int p_addr)
{
	// rear of the queue
	static int rear = 0;

	if(rear == TLB_ENTRIES) rear = 0;

	tlb[rear][0] = v_addr;
	tlb[rear][1] = p_addr;
	++rear;


	return;
}


float get_hit_ratio()
{
	// divide by zero check
	return num_tlb_requests != 0 ? (double)num_tlb_hits / (double)num_tlb_requests : 0.0;
}

//Write to the file in OUT_TLB
void print_tlb()
{
	// append mode
	const char* mode = "a";
	FILE *fp = fopen(OUT_TLB, mode);
	if(!fp) return;


	for(int i = 0; i < TLB_ENTRIES; ++i)
		fprintf(fp, "0x%05x 0x%05x\n", 0x000fffff & tlb[i][0], 0x000fffff & tlb[i][1]);

	fprintf(fp, "\n");

	fclose(fp);
	return;
}
