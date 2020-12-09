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
	printf("Bank: %d\n", ftl.Get_N_BANKS());
	printf("Blocks / Bank: %d blocks\n", BLKS_PER_BANK);
	printf("Pages / Block: %d pages\n", PAGES_PER_BLK);
	printf("OP ratio: %d%%\n", OP_RATIO);
	printf("Physical Blocks: %d\n", ftl.Get_N_BLOCKS());
	printf("User Blocks: %d\n", ftl.Get_N_USER_BLOCKS());
	printf("OP Blocks: %d\n", ftl.Get_N_OP_BLOCKS());
	printf("PPNs: %d\n", ftl.Get_N_PPNS());
	printf("LPNs: %d\n", ftl.Get_N_LPNS());

//#ifndef HOT_COLD
if(ftl.Check_Sp())
	printf("Workload: Random\n");
//#else
else
	printf("Workload: Hot %d / Cold %d\n", HOT_RATIO, COLD_RATIO);
//#endif

//#ifndef COST_BENEFIT
if(ftl.Check_Gp())
	printf("FTL: Greedy policy\n");
//#else
else
	printf("FTL: LRU policy\n");
//#endif

	printf("Max Sectors: %d\n", max_sectors);

	printf("\n");
}

/*u32 Get_Lpn()
{
	long lpn;

//#ifdef HOT_COLD
	if(ftl.Check_Sp()){
		double prob;

		prob = rand() % 100;
		if (prob < HOT_RATIO) 
		{
		// HOT
		//	printf("HOT\n");
			lpn = rand() % ftl.Get_HOT_LPN();
		} 
		else 
		{
		// COLD
		//	printf("COLD\n");
			lpn = ftl.Get_HOT_LPN() + (rand() % ftl.Get_COLD_LPN());
		}
	}
	else{
//#else
	lpn = rand() % ftl.Get_N_LPNS();
//#endif
	}
	return lpn;
}*/

long Get_Lba()
{
	long lba;

//#ifdef HOT_COLD
	if(ftl.Check_Sp()){
		double prob;

		prob = rand() % 100;
		if (prob < HOT_RATIO) 
		{
		// HOT
			lba = rand() % ftl.Get_HOT_LBA();
		} 
		else 
		{
		// COLD
		//	printf("COLD\n");
			lba = ftl.Get_HOT_LBA() + (rand() % ftl.Get_COLD_LBA());
		}
	}
	else{
//#else
	lba = rand() % ftl.Get_N_LBAS();
//#endif
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
/*	u32 lpn;
	u32 write_buffer[SECTORS_PER_PAGE];
	u32 read_buffer[SECTORS_PER_PAGE];

	while (ftl.Get_host_write() < ftl.Get_MAX_ITERATION())
	{
		lpn = Get_Lpn();
		
		memset(write_buffer, Get_Data(lpn), DATA_SIZE);
		memset(read_buffer, 0, DATA_SIZE);

		ftl.Ftl_Write(lpn, write_buffer);
		ftl.Ftl_Read(lpn, read_buffer);
		
		if (memcmp(write_buffer, read_buffer, DATA_SIZE)) assert(0); 

		ftl.Input_host_write(ftl.Get_host_write()+1);
		if (ftl.Get_host_write() % ftl.Get_N_LPNS() == 0)
		{
			printf("[Run %d] host %ld, valid page copy %ld, GC# %d, WAF %.2f\n",\
				(int)ftl.Get_host_write()/ftl.Get_N_LPNS(), ftl.Get_host_write(), ftl.Get_gc_write(), ftl.Get_gc(), (double)(ftl.Get_host_write()+ftl.Get_gc_write())/(double)ftl.Get_host_write());
		}

	}*/
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

		ftl.Input_host_write(ftl.Get_host_write()+num_sectors);
		iter++;
		if (iter % ftl.Get_N_LPNS() == 0) 
		{
			printf("[Run %d] host %ld, ftl %ld, valid page copy %ld, GC# %d, WAF %.2f\n", \
				(int)iter/ftl.Get_N_LPNS(), ftl.Get_host_write(), ftl.Get_ftl_write(), ftl.Get_gc_write() / SECTORS_PER_PAGE, ftl.Get_gc(), (double)(ftl.Get_ftl_write()+ftl.Get_gc_write())/(double)ftl.Get_host_write());
		}

		free(write_buffer);
		free(read_buffer);
	}

}

void Show_Stat(void)
{
	printf("\nResults ------\n");
	printf("Host writes: %ld\n", ftl.Get_host_write());
	printf("FTL write sectors: %ld\n", ftl.Get_ftl_write());
	printf("GC writes: %ld\n", ftl.Get_gc_write());
	printf("Number of GCs: %d\n", ftl.Get_gc());
	printf("Valid pages per GC: %.2f pages\n", (double)ftl.Get_gc_write() / SECTORS_PER_PAGE / (double)ftl.Get_gc());
	printf("WAF: %.2f\n", (double)(ftl.Get_ftl_write() + ftl.Get_gc_write()) / (double)ftl.Get_host_write());
}

int main(int argc, char* argv[])
{
	if(argc < 3){
		cout << "Usage: " << argv[0] << " <num> <policy> <stream>" << endl;
		exit(1);
	}
	ftl.Input_N_BANKS(atoi(argv[1]));
	ftl.Set_Variable();
	ftl.Set_Policy(argv[2], argv[3]);
	ftl.Ftl_Open();
	ftl.Sim_Init();
	Show_Info();
	Sim();
	Show_Stat();
	return 0;
}
