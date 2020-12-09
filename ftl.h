#include "nand.h"

enum GcPolicy{
	GREEDY,
	LRU

};

enum StreamPolicy{
	RANDOM,
	HOTCOLD
};

class Ftl:public Nand// : public Nand
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




    public:
			Ftl();
			~Ftl();
			void Ftl_Open();
			//void Ftl_Write(u32 lpn, u32 *write_buffer);
			void Ftl_Write(u32 lba, u32 num_sectors, u32 *write_buffer);//
			//void Ftl_Read(u32 lpn, u32 *read_buffer);
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
};
