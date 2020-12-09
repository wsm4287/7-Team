typedef unsigned int 		u32;
#define DATA_SIZE			(sizeof(u32) * 8)
#define SPARE_SIZE			(sizeof(u32))

// nand class prototypes
class Nand{

	protected:


	public:
		Nand();
		~Nand();
		int Nand_Read(int bank, int blk, int page, u32 *data, u32 *spare);
		int Nand_Write(int bank, int blk, int page, u32 *data, u32 spare);
		int Nand_Erase(int bank, int blk);
		int Nand_Blkdump(int bank, int blk);

};

