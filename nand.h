typedef unsigned int 		u32;
#define DATA_SIZE			(sizeof(u32) * 8)
#define SPARE_SIZE			(sizeof(u32))

#define BLKS_PER_BANK				32
#define PAGES_PER_BLK				32
#define SECTORS_PER_PAGE			(DATA_SIZE / sizeof(u32))

#define N_GC_BLOCKS					1

#define OP_RATIO					7
#define N_PPNS_PB					(BLKS_PER_BANK * PAGES_PER_BLK)
#define N_USER_BLOCKS_PB       		((BLKS_PER_BANK - N_GC_BLOCKS) * 100 / (100 + OP_RATIO))
#define N_OP_BLOCKS_PB				(BLKS_PER_BANK - N_USER_BLOCKS_PB)
#define N_LPNS_PB					(N_USER_BLOCKS_PB * PAGES_PER_BLK)

#define N_RUNS						50

#define CHECK_VPAGE(vpn)			assert((vpn >= 0 && vpn < N_PPNS_PB) || vpn == MAX)
#define CHECK_LPAGE(lpn)			assert((lpn) < N_LPNS)

//#define COST_BENEFIT
//#define HOT_COLD

#define HOT_RATIO				90
#define COLD_RATIO				(100 - HOT_RATIO)

#define HOT_LBA_RATIO 			(100 - HOT_RATIO)
#define COLD_LBA_RATIO			(100 - HOT_LBA_RATIO)

//#define HOT_LPN 				((N_LPNS * HOT_LPN_RATIO) / 100)
//#define COLD_LPN				((N_LPNS * COLD_LPN_RATIO) / 100)



// nand class prototypes
class Nand{

	protected:
		int N_BANKS = 2;
		int N_PPNS;
		int N_BLOCKS;
		int N_USER_BLOCKS;
		int N_OP_BLOCKS;
		int N_LPNS;
		int N_LBAS;//
		long MAX_ITERATION;
		//int HOT_LPN;
		//int COLD_LPN;
		int HOT_LBA;//
		int COLD_LBA;//

	public:
		Nand();
		~Nand();
		int Nand_Read(int bank, int blk, int page, u32 *data, u32 *spare);
		int Nand_Write(int bank, int blk, int page, u32 *data, u32 spare);
		int Nand_Erase(int bank, int blk);
		int Nand_Blkdump(int bank, int blk);
		void Input_N_BANKS(int nbanks);
		int Get_N_BANKS();
		int Get_N_PPNS();
		int Get_N_BLOCKS();
		int Get_N_USER_BLOCKS();
		int Get_N_OP_BLOCKS();
		int Get_N_LPNS();
		int Get_N_LBAS();//
		long Get_MAX_ITERATION();
		//int Get_HOT_LPN();
		//int Get_COLD_LPN();
		int Get_HOT_LBA();//
		int Get_COLD_LBA();//
		void Set_Variable();

};

//int Nand::N_BANKS = 0;
