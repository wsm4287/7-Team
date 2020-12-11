#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include "ftl.h"

using namespace std;

Ftl ftl;

int iter = 0;//
int max_sectors = 32;//

void Show_Info(void)
{
	
	cout << "Bank: " << ftl.Get_N_BANKS() << endl;
	cout << "Blocks / Bank: " <<  BLKS_PER_BANK <<" blocks" << endl;
	cout << "Pages / Bank: " <<  PAGES_PER_BLK <<" pages" << endl;
	cout << "OP ratio: "<< OP_RATIO << endl;
	cout << "Physical Blocks: " << ftl.Get_N_BLOCKS() << endl;
	cout << "User Blocks: " << ftl.Get_N_USER_BLOCKS() << endl;
	cout << "OP Blocks: " << ftl.Get_N_OP_BLOCKS() << endl;
	cout << "PPNs: " << ftl.Get_N_PPNS() << endl;
	cout << "LPNs: " << ftl.Get_N_LPNS() << endl;

	if(ftl.Check_Sp())
		cout << "Workload: Random" << endl;
	else
		cout << "Workload: Hot " << HOT_RATIO << " / Cold " << COLD_RATIO << endl;

	if(ftl.Check_Gp())
		cout << "FTL: Greedy policy" << endl;
	else
		cout << "FTL: LRU policy" << endl;

	cout << "Max Sectors: " << max_sectors << endl;

	cout << endl;
}



long Get_Lba()
{
	long lba;

	if(ftl.Check_Sp() == 0){
		double prob;

		prob = rand() % 100;
		if (prob < HOT_RATIO) 
		{
			lba = rand() % ftl.Get_HOT_LBA();
		} 
		else 
		{
			lba = ftl.Get_HOT_LBA() + (rand() % ftl.Get_COLD_LBA());
		}
	}
	else{
	lba = rand() % ftl.Get_N_LBAS();
	}
	return lba;
}

long Get_Sector_Cnt(long lba)
{
	if (ftl.Get_N_LBAS() - lba < max_sectors) {
		return (rand() % (ftl.Get_N_LBAS() - lba)) + 1;
	}

	return (rand() % max_sectors) + 1;
}


long Get_Data(long lba)
{
	if (lba % 0xF == 0x0) return 0x00;
	if (lba % 0xF == 0x1) return 0x11;
	else if (lba % 0xF == 0x2) return 0x22;
	else if (lba % 0xF == 0x3) return 0x33;
	else if (lba % 0xF == 0x4) return 0x44;
	else if (lba % 0xF == 0x5) return 0x55;
	else if (lba % 0xF == 0x6) return 0x66;
	else if (lba % 0xF == 0x7) return 0x77;
	else if (lba % 0xF == 0x8) return 0x88;
	else if (lba % 0xF == 0x9) return 0x99;
	else if (lba % 0xF == 0xa) return 0xaa;
	else if (lba % 0xF == 0xb) return 0xbb;
	else if (lba % 0xF == 0xc) return 0xcc;
	else if (lba % 0xF == 0xd) return 0xdd;
	else if (lba % 0xF == 0xe) return 0xee;
	else return 0xff;
}

void Sim()
{
	long lba, num_sectors, i;
	u32 *write_buffer;
	u32 *read_buffer;
	while (iter < ftl.Get_MAX_ITERATION())
	{
		lba = Get_Lba();
		num_sectors = Get_Sector_Cnt(lba);
		write_buffer = (u32 *)calloc(num_sectors, sizeof(u32));
		read_buffer = (u32 *)calloc(num_sectors, sizeof(u32));

		for (i = 0; i < num_sectors; i++) {
			memset(write_buffer + i, Get_Data(lba + i), sizeof(u32));
		}
		ftl.Ftl_Write(lba, num_sectors, write_buffer);
		ftl.Ftl_Read(lba, num_sectors, read_buffer);
		if (memcmp(write_buffer, read_buffer, num_sectors * sizeof(u32))) assert(0); 

		ftl.Input_Host_Write(ftl.Get_Host_Write()+num_sectors);
		iter++;
		if (iter % ftl.Get_N_LPNS() == 0) 
		{
			cout << fixed;
			cout.precision(2);
			cout << "[Run " << (int)iter/ftl.Get_N_LPNS() << "] host " << ftl.Get_Host_Write() << ", ftl " << ftl.Get_Ftl_Write() << ", valid page copy " << ftl.Get_Gc_Write() / SECTORS_PER_PAGE << ", GC# " << ftl.Get_Gc() << ", WAF " << (double)(ftl.Get_Ftl_Write()+ftl.Get_Gc_Write())/(double)ftl.Get_Host_Write() << endl;
		}

		free(write_buffer);
		free(read_buffer);
	}

}

void Show_Stat(void)
{
	cout << endl << "Results ------- " << endl;
	cout << "Host writes: " << ftl.Get_Host_Write() << endl;
	cout << "FTL write sectors: " << ftl.Get_Ftl_Write() << endl;
	cout << "GC writes: " << ftl.Get_Gc_Write() << endl;
	cout << "Number of GCs: " << ftl.Get_Gc() << endl;
	cout << fixed;
	cout.precision(2);
	cout << "Valid pages per GC: " <<  (double)ftl.Get_Gc_Write() / SECTORS_PER_PAGE / (double)ftl.Get_Gc()<< " pages" << endl;
	cout << "WAF: " << (double)(ftl.Get_Ftl_Write() + ftl.Get_Gc_Write()) / (double)ftl.Get_Host_Write() << endl;
}

int main(int argc, char* argv[])
{
	if(argc < 4){
		cout << "Usage: " << argv[0] << "<num> <policy> <stream>" << endl;
		exit(1);
	}
	ftl.Input_N_RUNS(atoi(argv[1]));
	ftl.Set_Policy(argv[2], argv[3]);
	ftl.Sim_Init();
	Show_Info();
	Sim();
	Show_Stat();
	return 0;
}
