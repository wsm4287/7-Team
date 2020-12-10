#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ftl.h"

using namespace std;

Nand::~Nand(){

}

int Nand::Get_N_BANKS(){
	return N_BANKS;
}
int Nand::Get_N_PPNS(){
	return N_PPNS;
}
int Nand::Get_N_BLOCKS(){
	return N_BLOCKS;
}
int Nand::Get_N_USER_BLOCKS(){
	return N_USER_BLOCKS;
}
int Nand::Get_N_OP_BLOCKS(){
	return N_OP_BLOCKS;
}
int Nand::Get_N_LPNS(){
	return N_LPNS;
}
int Nand::Get_N_LBAS(){
	return N_LBAS;
}
long Nand::Get_MAX_ITERATION(){
	return MAX_ITERATION;
}
int Nand::Get_HOT_LBA(){
	return HOT_LBA;
}
int Nand::Get_COLD_LBA(){
	return COLD_LBA;
}

void Nand::Set_Variable(){
	N_PPNS = N_PPNS_PB * N_BANKS;
	N_BLOCKS = BLKS_PER_BANK * N_BANKS;
	N_USER_BLOCKS = N_USER_BLOCKS_PB * N_BANKS;
	N_OP_BLOCKS = N_OP_BLOCKS_PB * N_BANKS;
	N_LPNS = N_LPNS_PB * N_BANKS;
	N_LBAS = N_LPNS * SECTORS_PER_PAGE;
	MAX_ITERATION = N_LPNS * N_RUNS;
	//HOT_LPN = (N_LPNS * HOT_LPN_RATIO) / 100;
	//COLD_LPN = (N_LPNS * COLD_LPN_RATIO) / 100;
	HOT_LBA = (N_LBAS * HOT_LBA_RATIO) / 100;//
	COLD_LBA = (N_LBAS * COLD_LBA_RATIO) / 100;//
}

Nand::Nand(){
	int nbanks = N_BANKS;
	int nblks = BLKS_PER_BANK;
	int npages = PAGES_PER_BLK;
	// initialize the NAND flash memory 
	// "nblks": the total number of flash blocks per bank
	// "npages": the number of pages per block
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	int i_bank, i_blk, i_page;
	int fd_bank, cur;
	char bank_num[100];

	u32 init_data[SECTORS_PER_PAGE];
	memset(init_data, 0xFF, DATA_SIZE);
	
	if(nbanks>0 && nblks>0 && npages>0){
		for(i_bank=0;i_bank<nbanks;i_bank++){
			sprintf(bank_num, "./BANK_%d", i_bank);
			fd_bank = open(bank_num,O_CREAT|O_RDWR|O_TRUNC, 00777);
			for(i_blk=0;i_blk<BLKS_PER_BANK;i_blk++){
				for(i_page=0;i_page<PAGES_PER_BLK;i_page++){
					cur = lseek(fd_bank,36*PAGES_PER_BLK*i_blk+36*i_page,SEEK_SET);
					write(fd_bank,init_data,DATA_SIZE);
				}
			}
			close(fd_bank);
		}
        }	
	else{ 
		cout<<"number of bank, block, page should be bigger than 0" << endl; 
	}
}
int Nand::Nand_Write(int bank, int blk, int page, u32 *data, u32 spare)
{
	// write "data" and "spare" into the NAND flash memory pointed to by "blk" and "page"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	int fd_bank, r;
	int  read_buf[100];
	char bank_num[100];
	off_t cur;

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDWR, 00777);

	if(page<0||page>=PAGES_PER_BLK) {printf("write(%d,%d,%d): failed, invalid page number\n",bank, blk, page);
		close(fd_bank);
		return -1;}
	else if(blk<0||blk>=BLKS_PER_BANK) {printf("write(%d,%d,%d): failed, invalid block number\n",bank, blk, page);
		close(fd_bank);
		return -1;}
	else if(bank<0||bank>=N_BANKS) {printf("write(%d,%d,%d): failed, invalid bank number\n", bank, blk, page);
		close(fd_bank);
		return -1;}
	else{
	if(page==0){                           
		cur=lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);
		r = read(fd_bank, read_buf, sizeof(read_buf));
		if(read_buf[0]==0xFFFFFFFF&&read_buf[1]==0xFFFFFFFF&&read_buf[2]==0xFFFFFFFF&&read_buf[3]==0xFFFFFFFF&&read_buf[4]==0xFFFFFFFF&&read_buf[5]==0xFFFFFFFF&&read_buf[6]==0xFFFFFFFF&&read_buf[7]==0xFFFFFFFF){
			cur=lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);
			write(fd_bank, data, DATA_SIZE);
			write(fd_bank, &spare, SPARE_SIZE); 
			/*if(*data==0){
				if(spare==0)
					printf("write(%d,%d,%d): data = 0x00000000, spare = 0x00000000\n", bank, blk, page);
				else
					printf("write(%d,%d,%d): data = 0x00000000, spare = %#010x\n", bank, blk, page, spare);
			}else
				printf("write(%d,%d,%d): data = %#x, spare = %#010x\n", bank, blk, page,*data, spare);*/
			close(fd_bank);
			return 0;			 
		}
		else{
			printf("write(%d,%d,%d): failed, the page was already written\n", bank, blk, page);				close(fd_bank);
			return -1;
		}
	}		
 	else{
		cur=lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*(page-1), SEEK_SET);
		r = read(fd_bank, read_buf, sizeof(read_buf));
		if(read_buf[0]==0xFFFFFFFF&&read_buf[1]==0xFFFFFFFF&&read_buf[2]==0xFFFFFFFF&&read_buf[3]==0xFFFFFFFF&&read_buf[4]==0xFFFFFFFF&&read_buf[5]==0xFFFFFFFF&&read_buf[6]==0xFFFFFFFF&&read_buf[7]==0xFFFFFFFF){
			printf("write(%d,%d,%d): failed, the page is not being sequentially written\n",bank, blk, page);
			close(fd_bank);
			return -1;
		}
		else{	
			cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);

			write(fd_bank, data, DATA_SIZE);
			write(fd_bank, &spare, SPARE_SIZE);
			/*if(*data==0)
				printf("write(%d,%d,%d): data = 0x00000000, spare = %#010x\n", bank, blk, page, spare);
			else
				printf("write(%d,%d,%d): data = %#x, spare = %#010x\n", bank, blk, page, *data, spare);*/
			close(fd_bank);
			return 0;
		}
	}
	}

}


int Nand::Nand_Read(int bank, int blk, int page, u32 *data, u32 *spare)
{
	// read "data" and "spare" from the NAND flash memory pointed to by "blk" and "page"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	int fd_bank, r;
	int  read_buf[100];
	char bank_num[100];
	off_t cur;

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDONLY, 00777);

	if(page<0||page>=PAGES_PER_BLK){
		printf("read(%d,%d,%d): failed, invalid page number\n",bank, blk, page);
		close(fd_bank);
		return -1;
	}else if(blk<0||blk>=BLKS_PER_BANK){
		printf("read(%d,%d,%d): failed, invalid block number\n",bank, blk, page);
		close(fd_bank);
		return -1;
	}else if(bank<0||bank>=N_BANKS){
		printf("read(%d,%d,%d): failed, invalid bank number\n", bank, blk, page);
		close(fd_bank);
		return -1;
	}else{
		cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);
		r = read(fd_bank, data, DATA_SIZE);
		r = read(fd_bank, spare, SPARE_SIZE);
		if(data[0]==0xFFFFFFFF && data[1]==0xFFFFFFFF && data[2]==0xFFFFFFFF && data[3]==0xFFFFFFFF && data[4]==0xFFFFFFFF && data[5]==0xFFFFFFFF && data[6]==0xFFFFFFFF && data[7]==0xFFFFFFFF){
			close(fd_bank);
			return -1;
		}else{
			/*if(*data==0){
				if(spare==0)
					printf("read(%d,%d,%d): data = 0x00000000, spare = 0x00000000\n",bank, blk, page);
				else
					printf("read(%d,%d,%d): data = 0x00000000, spare = %#010x\n",bank, blk, page, *spare);
			}
			else
				printf("read(%d,%d,%d): data = %#x, spare = %#010x\n",bank, blk, page, *data, *spare);*/
			close(fd_bank);
			return 0;
		}
	}
}

int Nand::Nand_Erase(int bank, int blk)
{
	// erase the NAND flash memory block "blk"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	int fd_bank, r, i, j;
	int  read_buf[100];
	char bank_num[100];
	off_t cur;

	u32 init_data[SECTORS_PER_PAGE];
	memset(init_data, 0xFF, DATA_SIZE);

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDWR, 00777);
	j = 0;
	for(i=0;i<PAGES_PER_BLK;i++){
		cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*i, SEEK_SET);
		r = read(fd_bank, read_buf, sizeof(read_buf));
		if(read_buf[0]==0xFFFFFFFF&&read_buf[1]==0xFFFFFFFF&&read_buf[2]==0xFFFFFFFF&&read_buf[3]==0xFFFFFFFF&&read_buf[4]==0xFFFFFFFF&&read_buf[5]==0xFFFFFFFF&&read_buf[6]==0xFFFFFFFF&&read_buf[7]==0xFFFFFFFF) j++;
	}
	if(j==PAGES_PER_BLK){
		printf("erase(%d,%d): failed, trying to erase a free block\n",bank, blk);
		close(fd_bank);
		return -1;
	}else{
		for(i=0;i<PAGES_PER_BLK;i++){
			cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*i, SEEK_SET);
			write(fd_bank,init_data,DATA_SIZE);
		}
		//printf("erase(%d,%d): block erased\n", bank, blk);
		close(fd_bank);
		return 0;
        }
}


int Nand::Nand_Blkdump(int bank, int blk)
{
	// dump the contents of the NAND flash memory block [blk] (for debugging purpose)
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	
	int fd_bank, r, i, j;
	int  read_buf[100];
	char bank_num[100];
	off_t cur;

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDONLY, 00777);
	for(i=0;i<PAGES_PER_BLK;i++){
		cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*i, SEEK_SET);
		r = read(fd_bank, read_buf, DATA_SIZE);
		if(read_buf[0]==0xFFFFFFFF&&read_buf[1]==0xFFFFFFFF&&read_buf[2]==0xFFFFFFFF&&read_buf[3]==0xFFFFFFFF&&read_buf[4]==0xFFFFFFFF&&read_buf[5]==0xFFFFFFFF&&read_buf[6]==0xFFFFFFFF&&read_buf[7]==0xFFFFFFFF){
			 break;
		}
	}
	if(i==0){
		printf("blkdump(%d,%d): FREE\n",bank, blk);
		close(fd_bank);
		return 0;
	}else{
		printf("blkdump(%d,%d): Total %d page(s) written\n",bank, blk,i);
	}
	for(j=0;j<i;j++){
		cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*j, SEEK_SET);
		r = read(fd_bank, read_buf, sizeof(read_buf));
		printf("blkdump(%d,%d,%d): data = %#x, spare = %#010x\n", bank, blk, j, read_buf[0], read_buf[8]);
	}
	close(fd_bank);
	return 0;
}

