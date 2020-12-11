#include "nand.h"
#include <iostream>
#include <map>
#include <list>

using namespace std;

enum GcPolicy{
	GREEDY,
	LRU

};

enum StreamPolicy{
	RANDOM,
	HOTCOLD
};

class Ftl:public Nand
{
	private:
		GcPolicy gp;
		StreamPolicy sp;

		struct Ftl_stat {
			int gc;
			long host_write;
			long ftl_write;//
			long gc_write;
		} s;
		map<int, list<int> :: iterator> lru0;
		list<int> check0;
		int lrusize0;
		map<int, list<int> :: iterator> lru1;
		list<int> check1;
		int lrusize1;

	public:
		Ftl();
		~Ftl();
		void Ftl_Write(u32 lba, u32 num_sectors, u32 *write_buffer);//
		void Ftl_Read(u32 lba, u32 num_sectors, u32 *read_buffer);//
		void Garbage_Collection(u32 bank);
		void Sim_Init();
		int Get_gc();
		void Input_gc(int);
		long Get_host_write();
		void Input_host_write(long);
		long Get_ftl_write();//
		void Input_ftl_write(long);//
		long Get_gc_write();
		void Input_gc_write(long);
		static long now();
		void Set_Policy(char*, char*);
		int Check_Gp();
		int Check_Sp();
		void Input_Lru(int, int);//////
		int Get_lru(int);//////
};
