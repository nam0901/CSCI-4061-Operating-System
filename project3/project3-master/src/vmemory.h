#ifndef __VMEMORY_H
#define __VMEMORY_H

//Page table related functions, filled out.
void populate_pt_1();
void populate_pt_2();
int** get_vpage_cr3();
void free_resources();

//Virtual address translation
int fetch_physical_frame(unsigned int);
void print_physical_address(int, int);

// TLB implementation
int get_tlb_entry(int v_addr);
void populate_tlb(int v_addr, int p_addr);
float get_hit_ratio();
void print_tlb();

void initialize_vmanager(int);
#endif
