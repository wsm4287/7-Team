#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <time.h>
#include "ftl.h"

using namespace std;

/*#ifdef COST_BENEFIT
static long Ftl::now() {
	return s.host_write + s.gc_write;
}
#endif
*/

int* l2ptable;
int* free_check;
long* age_check;
int* p_ptr;
int* valid_check;
int* gc_check;
int gc_blk;
int* free_blk;

Ftl::~Ftl(){

}

Ftl::Ftl(){

	int i;
	Set_Variable();
	l2ptable = (int*)malloc(sizeof(int)*N_LPNS);
	for(i=0;i<N_LPNS;i++)
		l2ptable[i] = N_PPNS;

	free_check = (int*)malloc(sizeof(char)*N_BANKS);
	for(i=0;i<N_BANKS;i++)
		free_check[i] = BLKS_PER_BANK + 1;
		
	age_check = (long*)malloc(sizeof(long)*N_BLOCKS);
	for(i=0;i<N_BLOCKS;i++)
		age_check[i] = 0;

	p_ptr = (int*)malloc(sizeof(int)*N_BANKS);
	for(i=0;i<N_BANKS;i++)
		p_ptr[i] = 0;

	valid_check = (int*)malloc(sizeof(int)*N_PPNS);
	for(i=0;i<N_PPNS;i++)
		valid_check[i] = 0; 

	gc_check = (int*)malloc(sizeof(int)*N_BANKS);
	for(i=0;i<N_BANKS;i++)	
		gc_check[i] = 0;

	free_blk = (int*)malloc(sizeof(int)*N_BANKS);
	for(i=0;i<N_BANKS;i++)
		free_blk[i] = BLKS_PER_BANK - 1;
}

//void Ftl::Ftl_Read(u32 lpn, u32 *read_buffer)
void Ftl::Ftl_Read(u32 lba, u32 num_sectors, u32 *read_buffer)
{	
	/*u32 bank, blk, p_page, ppn;
	u32 spare;
	
	ppn = l2ptable[lpn];
	bank = lpn % N_BANKS;
	blk = (ppn % N_PPNS_PB) / PAGES_PER_BLK;
	p_page = (ppn % N_PPNS_PB) % PAGES_PER_BLK;

	Nand_Read(bank, blk, p_page, read_buffer, &spare);	
	return;*/

	int bank, blk, p_page, i;
	int lpn, sect_offset, remain_sects, num_sectors_to_read, buf_cnt;
	u32 temp_page[SECTORS_PER_PAGE], spare;
	
	lpn = lba / SECTORS_PER_PAGE;
	sect_offset = lba % SECTORS_PER_PAGE;
	remain_sects = num_sectors;
	buf_cnt = 0;
	
	while(remain_sects != 0){
		if((sect_offset + remain_sects) < SECTORS_PER_PAGE)
            		num_sectors_to_read = remain_sects;
        	else
            		num_sectors_to_read = SECTORS_PER_PAGE - sect_offset;

		bank = lpn % N_BANKS;
		blk = (l2ptable[lpn] % N_PPNS_PB) / PAGES_PER_BLK;
		p_page = (l2ptable[lpn] % N_PPNS_PB) % PAGES_PER_BLK;

		if(gp == LRU)//////////////////
			Input_Lru(blk);
		Nand_Read(bank, blk, p_page, temp_page, &spare);

		for(i=0;i<SECTORS_PER_PAGE;i++){
			if(i>=sect_offset && i < sect_offset+num_sectors_to_read){
				read_buffer[buf_cnt] = temp_page[i];
				buf_cnt++;
			}
		}
		sect_offset = 0;
       		remain_sects -= num_sectors_to_read;
        	lpn++;
	}
	return;
}

void Ftl::Ftl_Write(u32 lba, u32 num_sectors, u32 *write_buffer)
{	
	int bank, lpn, i, sect_offset, remain_sects, buf_cnt, num_sectors_to_write;
	u32 temp_page[SECTORS_PER_PAGE], r_buf[SECTORS_PER_PAGE], spare;

	lpn = lba / SECTORS_PER_PAGE;
    	sect_offset = lba % SECTORS_PER_PAGE;
    	remain_sects = num_sectors;
	buf_cnt = 0;

 	while(remain_sects != 0){
        	if((sect_offset + remain_sects) < SECTORS_PER_PAGE)
            		num_sectors_to_write = remain_sects;
        	else
            		num_sectors_to_write = SECTORS_PER_PAGE - sect_offset;

		bank = lpn % N_BANKS;
		
		if(p_ptr[bank] % PAGES_PER_BLK == 0)
			free_check[bank]--;
			
		if(free_check[bank] == N_GC_BLOCKS){
			gc_blk = p_ptr[bank] / PAGES_PER_BLK;
			Garbage_Collection(bank);
		}

		if(l2ptable[lpn]==N_PPNS){
			for(i=0;i<SECTORS_PER_PAGE;i++){
				if(i<sect_offset || i>=sect_offset+num_sectors_to_write)
					temp_page[i] = 0xFFFFFFFF;
				else{
					temp_page[i] = write_buffer[buf_cnt];
					buf_cnt++;
				}
			}
			if(gp == LRU)////////////////////
				Input_Lru(p_ptr[bank]/PAGES_PER_BLK);
	
			Nand_Write(bank, p_ptr[bank]/PAGES_PER_BLK, p_ptr[bank]%PAGES_PER_BLK, temp_page, lpn);
			s.ftl_write += SECTORS_PER_PAGE;
			valid_check[N_PPNS_PB*bank+p_ptr[bank]] = 1;
			l2ptable[lpn] = N_PPNS_PB*bank+p_ptr[bank];
			if(gc_check[bank] == 1 && (p_ptr[bank] % PAGES_PER_BLK == PAGES_PER_BLK - 1)){
				p_ptr[bank] = free_blk[bank]*PAGES_PER_BLK;
				gc_check[bank] = 0;
			}else	p_ptr[bank]++;
		}
		else{	
			if(gp == LRU)////////////////////
				Input_Lru((l2ptable[lpn]%N_PPNS_PB)/PAGES_PER_BLK);
				Nand_Read(bank, (l2ptable[lpn]%N_PPNS_PB)/PAGES_PER_BLK, (l2ptable[lpn]%N_PPNS_PB)%PAGES_PER_BLK, r_buf, &spare); 
			for(i=0;i<SECTORS_PER_PAGE;i++){
				if(i<sect_offset || i>=sect_offset+num_sectors_to_write)
					temp_page[i] = r_buf[i];
				else{
					temp_page[i] = write_buffer[buf_cnt];
					buf_cnt++;
				}
			}
			valid_check[l2ptable[lpn]] = 0;
			//#ifdef COST_BENEFIT
			//age_check[BLKS_PER_BANK*bank+(l2ptable[lpn]%N_PPNS_PB)/PAGES_PER_BLK] = now();
			//#endif
			if(gp == LRU)/////////////////
				Input_Lru(p_ptr[bank]/PAGES_PER_BLK);
			Nand_Write(bank, p_ptr[bank]/PAGES_PER_BLK, p_ptr[bank]%PAGES_PER_BLK, temp_page, lpn);
			s.ftl_write += SECTORS_PER_PAGE;
			valid_check[N_PPNS_PB*bank+p_ptr[bank]] = 1;
			l2ptable[lpn] = N_PPNS_PB*bank+p_ptr[bank];
			if(gc_check[bank] == 1 && (p_ptr[bank] % PAGES_PER_BLK == PAGES_PER_BLK - 1)){
				p_ptr[bank] = free_blk[bank]*PAGES_PER_BLK;
				gc_check[bank] = 0;
			}else	p_ptr[bank]++; 
		}
	        sect_offset = 0;
       		remain_sects -= num_sectors_to_write;
        	lpn++;
    	}	
	return;
}

void Ftl::Garbage_Collection(u32 bank)
{
	s.gc++;

//#ifndef COST_BENEFIT
	// Greedy policy
	if(gp == GREEDY){
		int cnt_valid_page[BLKS_PER_BANK];
		int i, j, victim_blk, cnt, min;

		u32 read_buffer[SECTORS_PER_PAGE];
		u32 spare;

		cnt = 0;
		for(j=0;j<BLKS_PER_BANK;j++){
			if(j == gc_blk){
				cnt_valid_page[j] = PAGES_PER_BLK;
				continue;
			}
			for(i=0;i<PAGES_PER_BLK;i++){
				if(valid_check[N_PPNS_PB*bank+j*PAGES_PER_BLK+i] == 1)
					cnt++;
			}
			cnt_valid_page[j] = cnt;
			cnt = 0;
		}
		min = cnt_valid_page[0];
		victim_blk = 0;
		for(i=1;i<BLKS_PER_BANK;i++){
			if(cnt_valid_page[i]<min){
				min = cnt_valid_page[i];
				victim_blk = i;
			}
		}
		cnt = 0;
		for(i=0;i<PAGES_PER_BLK;i++){
			if(valid_check[N_PPNS_PB*bank+victim_blk*PAGES_PER_BLK+i] == 1){
				Nand_Read(bank, victim_blk, i, read_buffer, &spare);
				Nand_Write(bank, free_blk[bank], cnt, read_buffer, spare);
				l2ptable[spare] = N_PPNS_PB*bank + free_blk[bank]*PAGES_PER_BLK + cnt;
				valid_check[N_PPNS_PB*bank+victim_blk*PAGES_PER_BLK+i] = 0;
				valid_check[N_PPNS_PB*bank + free_blk[bank]*PAGES_PER_BLK + cnt] = 1;
				s.gc_write += SECTORS_PER_PAGE;
				p_ptr[bank]++;
				cnt++;	
			}
		}
		Nand_Erase(bank, victim_blk);
		free_blk[bank] = victim_blk;
		gc_check[bank] = 1;
		free_check[bank]++;
	}
	
	else{  // lru/////////////////////////////// 
	}

/*#else
	// Cost-Benefit policy
	int cnt_valid_page[BLKS_PER_BANK];
	int i, j, victim_blk, cnt;
	float max;
	float u[BLKS_PER_BANK], temp[BLKS_PER_BANK];
	long age[BLKS_PER_BANK];

	u32 read_buffer[SECTORS_PER_PAGE];
	u32 spare;

	cnt = 0;
	for(j=0;j<BLKS_PER_BANK;j++){
		if(j == gc_blk){
			cnt_valid_page[j] = PAGES_PER_BLK;
			continue;
		}
		for(i=0;i<PAGES_PER_BLK;i++){
			if(valid_check[N_PPNS_PB*bank+j*PAGES_PER_BLK+i] == 1)
				cnt++;
		}
		cnt_valid_page[j] = cnt;
		cnt = 0;
	}
	for(i=0;i<BLKS_PER_BANK;i++)
		u[i] = (float)((float)cnt_valid_page[i] / (float)PAGES_PER_BLK);
	for(j=0;j<BLKS_PER_BANK;j++){
		if(j == gc_blk){
			age[j] = 0;
			continue;
		}
		age[j] = now() - age_check[BLKS_PER_BANK*bank+j];
	}
	for(i=0;i<BLKS_PER_BANK;i++)
		temp[i] = (float)((float)age[i] * (float)(1-u[i])/(float)(2*u[i]));
	
	max = temp[0];
	victim_blk = 0;
	for(i=1;i<BLKS_PER_BANK;i++){
		if(temp[i]>max){
			max = temp[i];
			victim_blk = i;
		}
	}
	cnt = 0;
	for(i=0;i<PAGES_PER_BLK;i++){
		if(valid_check[N_PPNS_PB*bank+victim_blk*PAGES_PER_BLK+i] == 1){
			Nand_Read(bank, victim_blk, i, read_buffer, &spare);
			Nand_Write(bank, free_blk[bank], cnt, read_buffer, spare);
			l2ptable[spare] = N_PPNS_PB*bank + free_blk[bank]*PAGES_PER_BLK + cnt;
			valid_check[N_PPNS_PB*bank+victim_blk*PAGES_PER_BLK+i] = 0;
			s.gc_write++;
			age_check[BLKS_PER_BANK*bank+victim_blk] = now();
			valid_check[N_PPNS_PB*bank + free_blk[bank]*PAGES_PER_BLK + cnt] = 1;
			p_ptr[bank]++;
			cnt++;
		}
	}
	Nand_Erase(bank, victim_blk);
	free_blk[bank] = victim_blk;
	gc_check[bank] = 1;
	free_check[bank]++;

#endif
*/
	return;
}

void Ftl::Sim_Init(){
	s.gc = 0;
	s.host_write = 0;
	s.gc_write = 0;
	lrusize = N_BANKS * BLKS_PER_BANK;
	cout << lrusize << endl;
	srand(time(NULL));
}

int Ftl::Get_gc(){
	return s.gc;
}
void Ftl::Input_gc(int temp_gc){
	s.gc = temp_gc;

}
long Ftl::Get_host_write(){
	return s.host_write;
}
void Ftl::Input_host_write(long temp_host_write){
	s.host_write = temp_host_write;
}
long Ftl::Get_ftl_write(){//
	return s.ftl_write;
}
void Ftl::Input_ftl_write(long temp_ftl_write){//
	s.ftl_write = temp_ftl_write;
}
long Ftl::Get_gc_write(){
	return s.gc_write;
}
void Ftl::Input_gc_write(long temp_gc_write){
	s.gc_write = temp_gc_write;
}
void Ftl::Set_Policy(char* policy1, char* policy2){
	if(strcmp(policy1, "g") ==0) gp = GREEDY;
	else if(strcmp(policy1, "l") ==0) gp = LRU;
	else{
		cout << "Wrong Policy!" << endl;
		exit(1);
	}
	if(strcmp(policy2, "r") == 0 ) sp = RANDOM;
	else if(strcmp(policy2, "h") == 0) sp = HOTCOLD;
	else{
		cout << "Wrong Policy!" << endl;
		exit(1);
	}

}
int Ftl::Check_Gp(){
	if(gp == GREEDY) return 1;
	else return 0;
}
int Ftl::Check_Sp(){
	if(sp == RANDOM) return 1;
	else return 0;
}
////////////////////////
void Ftl::Input_Lru(int blk){
	if(lru.find(blk) == lru.end()){
		if(check.size() == lrusize){
			int removed = check.back();
			cout << "remove " << removed << endl;
			lru.erase(removed);
			check.pop_back();
			check.push_front(blk);
			lru[blk] = check.begin();
			cout << blk << " add" << endl;
		}
		else{
			check.push_front(blk);
			lru[blk] = check.begin();
			cout << blk << " add" << endl;
		}
	}

	else{
		check.remove(blk);
		check.push_front(blk);
		lru[blk] = check.begin();
		//cout << "already, exist " << blk << endl;
	}
}

int Ftl::Get_lru(){


}

