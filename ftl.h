#include "nand.h"
#include <iostream>
#include <map>
#include <list>

using namespace std;

enum gcpolicy{
	GREEDY,
	LRU

};

enum streampolicy{
	RANDOM,
	HOTCOLD
};

class Ftl:public Nand
{
	private:
		gcpolicy gp;
		streampolicy sp;

		struct ftl_stat {
			int gc;
			long host_write;
			long ftl_write;//
			long gc_write;
		} s;
		map<int, list<int> :: iterator> lru0;
		list<int> check0;
		map<int, list<int> :: iterator> lru1;
		list<int> check1;

	public:
		Ftl();
		~Ftl();
		void Ftl_Write(u32 lba, u32 num_sectors, u32 *write_buffer);//
		void Ftl_Read(u32 lba, u32 num_sectors, u32 *read_buffer);//
		void Garbage_Collection(u32 bank);
		void Sim_Init();
		int Get_Gc();
		void Input_Gc(int);
		long Get_Host_Write();
		void Input_Host_Write(long);
		long Get_Ftl_Write();//
		void Input_Ftl_Write(long);//
		long Get_Gc_Write();
		void Input_Gc_Write(long);
		static long Now();
		void Set_Policy(char*, char*);
		int Check_Gp();
		int Check_Sp();
		void Input_Lru(int, int);//////
		int Get_Lru(int);//////
};
