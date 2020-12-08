#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "nand.h"

using namespace std;

Nand::Nand(){

}

Nand::~Nand(){

}



int Nand::Nand_Init(int nbanks, int nblks, int npages)
{
	// initialize the NAND flash memory 
	// "nblks": the total number of flash blocks per bank
	// "npages": the number of pages per block
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	int i_bank, i_blk, i_page;
	int fd_bank, cur;
	char *bank_num = new char[100];
	
	if(nbanks>0 && nblks>0 && npages>0){
		for(i_bank=0;i_bank<nbanks;i_bank++){
			sprintf(bank_num, "./BANK_%d", i_bank);
			fd_bank = open(bank_num,O_CREAT|O_RDWR|O_TRUNC, 00777);
			
			delete bank_num;
			
			for(i_blk=0;i_blk<BLKS_PER_BANK;i_blk++){
				for(i_page=0;i_page<PAGES_PER_BLK;i_page++){
					cur = lseek(fd_bank,36*PAGES_PER_BLK*i_blk+36*i_page,SEEK_SET);
					write(fd_bank,"abcd",SPARE_SIZE);
				}
			}
			close(fd_bank);
		}
		cout <<"init: " << nbanks <<" banks, " << nblks <<" blocks, " << npages << " pages per block"<<endl;
		return 0;
        }	
	else{ 
		cout<<"number of bank, block, page should be bigger than 0" << endl; 
		return -1;
	}
}

int Nand::Nand_Write(int bank, int blk, int page, u32 *data, u32 spare)
{
	// write "data" and "spare" into the NAND flash memory pointed to by "blk" and "page"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	int fd_bank, r;
	int  *read_buf = new int[100];
	char *bank_num = new char[100];
	off_t cur;

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDWR, 00777);
	
	delete bank_num;

	if(page<0||page>=PAGES_PER_BLK) {
		cout <<"write(" << bank << "," << blk << "," << page << "): failed, invalid page number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}

	else if(blk<0||blk>=BLKS_PER_BANK) {
		cout <<"write(" << bank << "," << blk << "," << page << "): failed, invalid block number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}

	else if(bank<0||bank>=N_BANKS) {
		cout <<"write(" << bank << "," << blk << "," << page << "): failed, invalid bank number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}

	else{
		if(page==0){                           //page==0
			cur=lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);
			r = read(fd_bank, read_buf, sizeof(read_buf));
		
			if(read_buf[0]==read_buf[1]){
				cout <<"write(" << bank << "," << blk << "," << page << "): failed, the page was already written" << endl;
				close(fd_bank);
				delete read_buf;
				return -1;  //overwrite
			}

			else{
				cur=lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);
				write(fd_bank, data, DATA_SIZE);
				write(fd_bank, &spare, SPARE_SIZE); //erase
				if(*data==0){
					if(spare==0)
						cout <<"write(" << bank << "," << blk << "," << page << "): data = 0x00000000, spare = 0x00000000"<< endl;
					else
						cout <<"write(" << bank << "," << blk << "," << page << "): data = 0x00000000, spare = 0x"<< cout.width(5) << cout.fill('0') << hex << spare << endl;
				}
				else
					cout <<"write(" << bank << "," << blk << "," << page << "): data = 0x" << hex << *data <<", spare = 0x"<< cout.width(5) << cout.fill('0') << hex <<  spare << endl;
					close(fd_bank);
					delete read_buf;
					return 0;
			}
		}		
 	
		else{
			cur=lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*(page-1), SEEK_SET);
			r = read(fd_bank, read_buf, sizeof(read_buf));
			if(read_buf[0]==read_buf[1]){
				cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);
				write(fd_bank, data, DATA_SIZE);
				write(fd_bank, &spare, SPARE_SIZE);
				if(*data==0)
					cout <<"write(" << bank << "," << blk << "," << page << "): data = 0x00000000, spare = 0x"<< cout.width(5) << cout.fill('0') << hex << spare << endl;
				else
					cout <<"write(" << bank << "," << blk << "," << page << "): data = 0x" << hex << *data <<", spare = 0x"<< cout.width(5) << cout.fill('0') << hex << spare << endl;
					close(fd_bank);
					delete read_buf;
					return 0;
			}
			else{	
				cout <<"write(" << bank << "," << blk << "," << page << "): failed, the page is not being sequentially written" << endl;
				close(fd_bank);
				delete read_buf;
				return -1;
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
	int *read_buf = new int[100];
	char *bank_num = new char[100];
	off_t cur;

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDONLY, 00777);

	delete bank_num;

	if(page<0||page>=PAGES_PER_BLK){
		cout <<"read(" << bank << "," << blk << "," << page << "): failed, invalid page number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}else if(blk<0||blk>=BLKS_PER_BANK){
		cout <<"read(" << bank << "," << blk << "," << page << "): failed, invalid block number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}else if(bank<0||bank>=N_BANKS){
		cout <<"read(" << bank << "," << blk << "," << page << "): failed, invalid bank number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}else{
		cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*page, SEEK_SET);
		r = read(fd_bank, data, DATA_SIZE);
		r = read(fd_bank, spare, SPARE_SIZE);
		if(*data==0x64636261){
			cout <<"read(" << bank << "," << blk << "," << page << "): failed, trying to read an empty page" << endl;
			close(fd_bank);
			delete read_buf;
			return -1;
		}else{
			if(*data==0){
				if(spare==0)
					cout <<"read(" << bank << "," << blk << "," << page << "): data = 0x00000000, spare = 0x00000000"<< endl;
				else
					cout <<"read(" << bank << "," << blk << "," << page << "): data = 0x00000000, spare = 0x"<< cout.width(5) << cout.fill('0') << hex << *spare << endl;
			}
			else{
				cout <<"read(" << bank << "," << blk << "," << page << "): data = 0x" << hex << *data <<", spare = 0x"<< cout.width(5) << cout.fill('0') << hex << *spare << endl;
				close(fd_bank);
				delete read_buf;
				return 0;
			}
		}
	}
}

int Nand::Nand_Erase(int bank, int blk)
{
	// erase the NAND flash memory block "blk"
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	int fd_bank, r, i, j;
	int *read_buf = new int[100];
	char *bank_num = new char[100];
	off_t cur;

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDWR, 00777);
	
	delete bank_num;

	if(blk<0||blk>=BLKS_PER_BANK){
		cout <<"erase(" << bank << "," << blk << "): failed, invalid block number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}else if(bank<0||bank>=N_BANKS){
		cout <<"erase(" << bank << "," << blk << "): failed, invalid bank number" << endl;
		delete read_buf;
		close(fd_bank);
		return -1;
	}else{
		j = 0;
		for(i=0;i<PAGES_PER_BLK;i++){
			cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*i, SEEK_SET);
			r = read(fd_bank, read_buf, sizeof(read_buf));
			if(read_buf[0]==0x64636261) j++;
		}
		if(j==PAGES_PER_BLK){
			cout <<"erase(" << bank << "," << blk << "): failed, trying to erase a free block" << endl;
			close(fd_bank);
			delete read_buf;
			return -1;
		}else{
			for(i=0;i<PAGES_PER_BLK;i++){
				cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*i, SEEK_SET);
				write(fd_bank,"abcd",SPARE_SIZE);
			}
			cout <<"erase(" << bank << "," << blk << "): block erased" << endl;
			close(fd_bank);
			delete read_buf;
			return 0;
		}
	}
}


int Nand::Nand_Blkdump(int bank, int blk)
{
	// dump the contents of the NAND flash memory block [blk] (for debugging purpose)
	// returns 0 on success
	// returns -1 on errors with printing appropriate error message

	
	int fd_bank, r, i, j;
	int *read_buf = new int[100];
	char *bank_num = new char[100];
	off_t cur;

	sprintf(bank_num, "./BANK_%d", bank);
	fd_bank = open(bank_num, O_RDONLY, 00777);

	delete bank_num;

	if(blk<0||blk>=BLKS_PER_BANK){
		cout <<"blkdump(" << bank << "," << blk << "): failed, invalid block number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}else if(bank<0||bank>=N_BANKS){
		cout <<"blkdump(" << bank << "," << blk << "): failed, invalid bank number" << endl;
		close(fd_bank);
		delete read_buf;
		return -1;
	}else{
		for(i=0;i<PAGES_PER_BLK;i++){
			cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*i, SEEK_SET);
			r = read(fd_bank, read_buf, DATA_SIZE);
			if(read_buf[0]==0x64636261){
				 break;
			}
		}
		if(i==0){
			cout << "blkdump(" << bank << "," << blk << "): FREE" << endl;
			close(fd_bank);
			delete read_buf;
			return 0;
		}else{
			cout << "blkdump(" << bank << "," << blk << "): Total " << i << " page(s) written" << endl;
		}
		for(j=0;j<i;j++){
			cur = lseek(fd_bank, 36*PAGES_PER_BLK*blk+36*j, SEEK_SET);
			r = read(fd_bank, read_buf, sizeof(read_buf));
			if(read_buf[0]==0){
				if(read_buf[8]==0)
					cout <<"blkdump(" << bank << "," << blk << "," << j << "): data = 0x00000000, spare = 0x00000000"<< endl;
				else
					cout <<"blkdump(" << bank << "," << blk << "," << j << "): data = 0x00000000, spare = 0x" << cout.width(5) << cout.fill('0') << hex << 8*(blk+j)<< endl;
			}
			else
				cout <<"blkdump(" << bank << "," << blk << "," << j << "): data = 0x" << read_buf[0] <<", spare = 0x" << cout.width(5) << cout.fill('0') << hex << 8*(blk+j) << endl;
		}
		close(fd_bank);
		delete read_buf;
		return 0;
	}
 
	delete read_buf;
	return 0;
}

