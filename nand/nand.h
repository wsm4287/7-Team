typedef unsigned int 		u32;

#define BANK_SHIFT					1
#define BLOCK_SHIFT					3
#define PAGES_PER_BLOCK_SHIFT		3

#define N_BANKS						(1 << BANK_SHIFT)

#define SECTORS_PER_PAGE			8

#define PAGES_PER_BLK				(1 << PAGES_PER_BLOCK_SHIFT)
#define BLKS_PER_BANK				(1 << BLOCK_SHIFT)


#define DATA_SIZE			(sizeof(u32) * 8)
#define SPARE_SIZE			(sizeof(u32))

// nand class prototypes
class Nand{

	public:
		Nand();
		~Nand();
		int Nand_Init(int nbanks, int nblks, int npages);
		int Nand_Read(int bank, int blk, int page, u32 *data, u32 *spare);
		int Nand_Write(int bank, int blk, int page, u32 *data, u32 spare);
		int Nand_Erase(int bank, int blk);
		int Nand_Blkdump(int bank, int blk);


};



