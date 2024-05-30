#include "HMEnc.h"
#include <stdio.h>
#include <fstream>
#include <bitset>
#include <afx.h>
#include "openssl\aes.h"
#include "openssl\rand.h"

//#pragma comment(lib, "E:\HEVC\HM-16.18\source\OpenSSL\lib\libeay32.lib")
using namespace std;


struct ctr_state 
{ 
    unsigned char ivec[AES_BLOCK_SIZE];  
    unsigned int num; 
    unsigned char ecount[AES_BLOCK_SIZE]; 
}; 
/*
float NALInfo::ComputeCUSpaEffAverge()
{
//-----------zigzag顺序---------------
	int ctu_h=ceil((PicHei*1.0)/64);
	int ctu_w=ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;
	float SpaEff=0;
	int CUNumInframe=0;
	float SpaEffAve;
	for(int i=0;i<ctu_h;i++)//外层CTU是光栅扫描
	{
		for(int j=0;j<ctu_w;j++)
		{
			int curctusize_x=j;
			int curctusize_y=i;
			//printf("CTU(%d,%d)\n",curctusize_x,curctusize_y);
			//下面是CTU内部的深度优先扫描zigz,四层for: k l m n
			for(int k=0;k<4;k++)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				//printf("32size(%d,%d)\n",cur32size_x,cur32size_y);
				for(int l=0;l<4;l++)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					//printf("16size(%d,%d)\n",cur16size_x,cur16size_y);
					for(int m=0;m<4;m++)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;
						//printf("8size(%d,%d)\n",cur8size_x,cur8size_y);
						for(int n=0;n<4;n++)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
							
							if(valid_xy)
							{
								Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
								//Bool DepPUInCU=(PUTable[PUTable_x][PUTable_y].IntraPredMode != 36);
								if(FirPuInTruPU)
								{
									Bool FirPuInCU=(PUTable_x == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x/4))  && (PUTable_y == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y/4));
									if(FirPuInCU)
									{
										SpaEff += PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff;
										CUNumInframe +=1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	SpaEffAve=SpaEff/CUNumInframe;
	return SpaEffAve;
}

float NALInfo::ComputePUSpaEffAverge()
{
//-----------zigzag顺序---------------
	int ctu_h=ceil((PicHei*1.0)/64);
	int ctu_w=ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;
	float SpaEff=0;
	int PUNumInframe=0;
	float SpaEffAve;
	for(int i=0;i<ctu_h;i++)//外层CTU是光栅扫描
	{
		for(int j=0;j<ctu_w;j++)
		{
			int curctusize_x=j;
			int curctusize_y=i;
			//printf("CTU(%d,%d)\n",curctusize_x,curctusize_y);
			//下面是CTU内部的深度优先扫描zigz,四层for: k l m n
			for(int k=0;k<4;k++)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				//printf("32size(%d,%d)\n",cur32size_x,cur32size_y);
				for(int l=0;l<4;l++)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					//printf("16size(%d,%d)\n",cur16size_x,cur16size_y);
					for(int m=0;m<4;m++)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;
						//printf("8size(%d,%d)\n",cur8size_x,cur8size_y);
						for(int n=0;n<4;n++)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							//printf("4size(%d,%d)\n",cur4size_x,cur4size_y);

							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							//printf("PU(%d,%d)\n",PUTable_x,PUTable_y);

							if((PUTable_x < table_w) && (PUTable_y < table_h))
							{
								if(PUTable[PUTable_x][PUTable_y].IntraPredMode != 36)//被赋值的4x4PU
								{

									SpaEff += PUTable[PUTable_x][PUTable_y].TruPU_SpatEFF;
									PUNumInframe +=1;
								}															
							}
						}
					}
				}
			}
		}
	}

	SpaEffAve=SpaEff/PUNumInframe;
	return SpaEffAve;
}

void NALInfo::PickEncPU(float SpaEffAverage)
{
	//-----------zigzag顺序---------------
	int ctu_h=ceil((PicHei*1.0)/64);
	int ctu_w=ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;
	float SpaEff=0;
	int PUNumInframe=0;
	float SpaEffAve;
	int EncPUNum=0;

	for(int i=0;i<ctu_h;i++)//外层CTU是光栅扫描
	{
		for(int j=0;j<ctu_w;j++)
		{
			int curctusize_x=j;
			int curctusize_y=i;
			//printf("CTU(%d,%d)\n",curctusize_x,curctusize_y);
			//下面是CTU内部的深度优先扫描zigz,四层for: k l m n
			for(int k=0;k<4;k++)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				//printf("32size(%d,%d)\n",cur32size_x,cur32size_y);
				for(int l=0;l<4;l++)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					//printf("16size(%d,%d)\n",cur16size_x,cur16size_y);
					for(int m=0;m<4;m++)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;
						//printf("8size(%d,%d)\n",cur8size_x,cur8size_y);
						for(int n=0;n<4;n++)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							//printf("4size(%d,%d)\n",cur4size_x,cur4size_y);



							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							//printf("PU(%d,%d)\n",PUTable_x,PUTable_y);

							if((PUTable_x < table_w) && (PUTable_y < table_h))
							{
								if(PUTable[PUTable_x][PUTable_y].IntraPredMode != 36)//被赋值的4x4PU
								{

									if(PUTable[PUTable_x][PUTable_y].TruPU_SpatEFF > SpaEffAverage)
									{
										EncPUNum +=1;
										printf("EncCU(%d,%d)-PU(%d,%d) off=%06x:%d\n",
											PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x,
											PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y,
											PUTable[PUTable_x][PUTable_y].PUInPicLoc_x,
											PUTable[PUTable_x][PUTable_y].PUInPicLoc_y,
											PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset/8,
											PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset%8);
									}
								}									
							
							}
						}
					}
				}
			}
		}
	}

	printf("Enc PU number = %d\n",EncPUNum);
}
*/
/*
void NALInfo::ComputeIntraCUSpatEff()
{
	//-----------zigzag倒序-------------------
	int ctu_h=(int)ceil((PicHei*1.0)/64);
	int ctu_w=(int)ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;

	for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	{
		for(int j=ctu_w-1;j>=0;j--)
		{
			int curctusize_x=j;
			int curctusize_y=i;

			float CUEff=0;
			for(int k=3;k>=0;k--)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;			
						for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
							if(valid_xy)
							{
								Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
								if(FirPuInTruPU)
								{	
									CUEff += PUTable[PUTable_x][PUTable_y].TruPU_SpatEFF;
									Bool FirPuInCU=(PUTable_x == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x/4))  && (PUTable_y == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y/4));				
									if(FirPuInCU)//firstPU in 8x8CU
									{				
										if(PUTable[PUTable_x][PUTable_y].TruPUSize ==4)
										{
											PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff=CUEff/4;
										}
										else
										{
											PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff=CUEff;
										}
										CUEff=0;
										CUnum++;
									}
								}									
							}			
						}
					}
				}
			}
		}
	}
	printf("CU number=%d\n",CUnum);
}

void NALInfo::ComputeInterCUSpatEff()
{
	//-----------zigzag倒序-------------------
	int ctu_h=(int)ceil((PicHei*1.0)/64);
	int ctu_w=(int)ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;

	for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	{
		for(int j=ctu_w-1;j>=0;j--)
		{
			int curctusize_x=j;
			int curctusize_y=i;

			float InterCUEff=0;
			for(int k=3;k>=0;k--)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;			
						for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
							if(valid_xy)
							{
								Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
								if(FirPuInTruPU)//是一个PU的first 4x4PU 即 true PU
								{	
									//printf("truePU(%d,%d)\n",
									//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_x,
									//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_y);

									InterCUEff += PUTable[PUTable_x][PUTable_y].TruInterPU_SpatEff;
									Bool FirPuInCU=(PUTable_x == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x/4))  && (PUTable_y == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y/4));
									if(FirPuInCU)//是一个CU的firstPU in 8x8CU
									{	
										//printf("CU(%d,%d)\n",
										//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_x,
										//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_y);
										PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff=InterCUEff;
										InterCUEff=0;
									}
								}									
							}			
						}
					}
				}
			}
		}
	}
}
*/

Location::Location(){}
Location::~Location(){}
CostBits::CostBits(){}
CostBits::~CostBits(){}
CUCostBits::CUCostBits(){}
CUCostBits::~CUCostBits(){}
OffsetInfo::OffsetInfo(){}
OffsetInfo::~OffsetInfo(){}
ChunkInfo::ChunkInfo(){}
ChunkInfo::~ChunkInfo(){}
NALHeaderInfo::NALHeaderInfo(){}
NALHeaderInfo::~NALHeaderInfo(){}
SliceHeaderInfo::SliceHeaderInfo(){}
SliceHeaderInfo::~SliceHeaderInfo(){}
SAOInfo::SAOInfo(){}
SAOInfo::~SAOInfo(){}
NALInfo::NALInfo(){}
NALInfo::~NALInfo(){}
CUInfo::CUInfo(){}
CUInfo::~CUInfo(){}
BitExtractUnit::BitExtractUnit(){}
BitExtractUnit::~BitExtractUnit(){}
PUInfo::PUInfo(){}
PUInfo::~PUInfo(){}
TableU::TableU(){}
TableU::~TableU(){}
Dirlist::Dirlist(){}
Dirlist::~Dirlist(){}

//类初始化
void Dirlist::init()
{
	ForwDir=5;//self cankao
	BackDir=5;
}

void PUInfo::init()
{
	InTruPU_x=0;
	InTruPU_y=0;
	TruPUSize=0;	
	
	PUInPicLoc_x=0;	
	PUInPicLoc_y=0;	
	BelCU.init();

	IntraPredMode=36;	
	TruPU_SpatEFF=0;

	InterPredDir.init();
	TruInterPU_SpatEff=0;
}

void TableU::init()
{
	x=0;
	y=0;
	percentage=0;
}

void Location::init()
{
	x=0;
	y=0;
}

void CostBits::init()
{
	Totalcostbits=0;
}

void CUCostBits::init()
{
	PredBits=0;
	ResiBits=0;
	Totalcostbits=0;
}

void OffsetInfo::init()
{
	InFileOffsset=0;//bit单位
	InNALOffset=0;
}

void ChunkInfo::init()
{
	ChunkInFileOffset=0;//bit单位
	ChunkCostBits.init();//bit单位
	ChunkLen=0; //字节单位
}

void NALHeaderInfo::init()
{
	NALHeaderOffset.init();;
	NALHeaderCostBits.init();//bit单位
	NALHeaderLen=0;
}

void SliceHeaderInfo::init()
{
	SliceHeaderOffset.init();
	SliceHeaderCostBits.init();//bit单位
	SliceHeaderLen=0;
}

void SAOInfo::init()
{
	SAOOffsetInfo.init();
	SAOCostBits.init();//bit单位
}

void NALInfo::init()
{
	BelongChunkInfo.init();
	NALHeaderInfo.init();
	SliceHeaderInfo.init();
	SAOInfo.init();
	myCUInfo.init();
	PUTable=NULL;
	PicWid=0;
	PicHei=0;
	 EncTime=0;
	 ParseTime=0;
	 ModelTime=0;
	 AESTime=0;

}

void CUInfo::init()
{
	CUOffsetInfo.init(); //get
	CULocInPic.init();;	//get
	CostBits.init();	//get

	CUSpaEff=0;
	CUTemEff=0;

	LumaDirMode.init();
	LumaDirModeLen=0;
	ChromaDirMode.init();
	ChromaDirModeLen=0;
	Coefficient.init();
	CoefficientLen=0;
	PartSize.init();
	PartSizeLen=0;
	SplitFlag.init();
	SplitFlagLen=0;
	 IPCM.init();
	 IPCMLen=0;
	 MergeIdx.init();
	 MergeIdxLen=0;
	 SkipFlag.init();
	 SkipFlagLen=0;
	 InterDir.init();
	 InterDirLen=0;
	 RefIdx.init();
	 RefIdxLen=0;
	 MVD.init();
	 MVDLen=0;
	 AMVPIdx.init();
	 AMVPIdxLen=0;
	 
}

void NALInfo::InitMyfifoIdx()
{
	MY_bitoffsetinNAL=0;
	MY_m_fifo_idx_my=0;
	CUnum=0;
}

//计算03标志
void NALInfo::DecAddEight(int curByte, int NextByte)
{
	UInt form5byte;
	UInt form4byte;
	UInt form3byte;
	UInt form2byte;
	UInt form1byte;
	int MY_m_fifo_idx_cur=MY_m_fifo_idx_my;
	if(MY_m_fifo_idx_cur>=6)
	{
		 form5byte = myfifo[MY_m_fifo_idx_cur-6];
	}
	if(MY_m_fifo_idx_cur>=5)
	{
		 form4byte = myfifo[MY_m_fifo_idx_cur-5];
	}
	if(MY_m_fifo_idx_cur>=4)
	{
		 form3byte = myfifo[MY_m_fifo_idx_cur-4];
	}
	if(MY_m_fifo_idx_cur>=3)
	{
		 form2byte = myfifo[MY_m_fifo_idx_cur-3];
	}
	if(MY_m_fifo_idx_cur>=2)
	{
		form1byte = myfifo[MY_m_fifo_idx_cur-2];
	}

	UInt form0byte = myfifo[MY_m_fifo_idx_cur-1];//解码后的idx的前一个	
	Bool addflag=AddZerothree(form5byte,form4byte,form3byte,form2byte,form1byte,form0byte,curByte,NextByte);
	if(addflag)//len+1byte
	{
		//printf("我是%2X %2X 03 %2X,下一个+8\n",form2byte,form1byte,form0byte);
		MY_bitoffsetinNAL += 8;
	}
}

Bool NALInfo::AddZerothree(UInt &form5byte,UInt &form4byte,UInt &form3byte,UInt &form2byte, UInt &form1byte,UInt &form0byte,int curByte,int NextByte)
{
	if(curByte != NextByte)
	{
		if(( form2byte == 0x00) && (form1byte==0x00) && (form0byte==0x00 || form0byte==0x01 ||form0byte==0x02 ||form0byte==0x03))
		{
			if(form3byte !=0)
			{
				return true;
			}
			else
			{
				if(form5byte==0 && form4byte==0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

}

//表初始化
void NALInfo::InitPUTable()
{
	int x=PicWid/4;
	int y=PicHei/4;

	PUTable = new PUInfo *[x];//先开出x列
	for(int i=0;i<x;i++)
	{
		PUTable[i]= new PUInfo [y];//每行开出y行
	}

	//-------------IntraPredMode init=36----------
	for(int i=0;i<y;i++)
	{
		for(int j=0;j<x;j++)
		{
			PUTable[j][i].init(); //初始化为36
		}
	}
}

void NALInfo::DeletePUTable()
{
	int x=PicWid/4;
	int y=PicHei/4;

	for(int i=0;i<x;i++)
	{
		delete [] PUTable[i]; 
	}
	PUTable=NULL;
}

void NALInfo::SetSpatEffTable()
{
	Percen = new float **[35];
	for(int m=0;m<35;m++)
	{
		Percen[m]= new float *[5];
		for(int d=0;d<5;d++)
		{
			Percen[m][d] = new float [16];
		}
	}

	for(int mode=0;mode<35;mode++)
	{
	
		if(mode==0)
		{
			int m=mode;
			for(int i=0;i<16;i++)	
			{
				Percen[m][0][i]=0;	
				Percen[m][1][i]=1.0/16;
				Percen[m][3][i]=1.0/16;
			}
			Percen[m][2][0]=1;
			Percen[m][4][0]=1;
			for(int i=1;i<16;i++)	
			{
				Percen[m][2][i]=0;
				Percen[m][4][i]=0;
			}
		}
		else if(mode == 1)
		{
			int m=mode;
			for(int i=0;i<16;i++)	
			{
				Percen[m][0][i]=0;
				Percen[m][1][i]=1.0/16;
				Percen[m][2][i]=0;
				Percen[m][3][i]=1.0/16;
				Percen[m][4][i]=0;
			}
		}
		else if(mode==2)
		{
			int m=mode;
			for(int i=0;i<16;i++)	
			{
				Percen[m][0][i]=0;
				Percen[m][1][i]=0;
				Percen[m][2][i]=0;
				Percen[m][3][i]=(i+1+i)*1.0/512;
				Percen[m][4][15-i]=(i+1+i)*1.0/512;
			}
		}
		else if(mode==10)
		{
			int m=mode;
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=0;
				Percen[m][1][i]=0;
				Percen[m][2][i]=0;
				Percen[m][3][i]=1.0/16;
				Percen[m][4][i]=0;
			}
		}
		else if(mode==18)
		{
			int m=mode;
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=(16+16)*1.0/512;
				Percen[m][1][i]=((15-i)+1+(15-i))*1.0/512;
				Percen[m][2][i]=0;
				Percen[m][3][i]=((15-i)+1+(15-i))*1.0/512;
				Percen[m][4][i]=0;
			}
		}
		else if(mode==26)
		{
			int m=mode;
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=0;
				Percen[m][1][i]=1.0/16;
				Percen[m][2][i]=0;
				Percen[m][3][i]=0;
				Percen[m][4][i]=0;
			}
		}
		else if(mode==34)
		{
			int m=mode;
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=0;
				Percen[m][1][i]=(i+1+i)*1.0/512;
				Percen[m][2][15-i]=(i+1+i)*1.0/512;
				Percen[m][3][i]=0;
				Percen[m][4][i]=0;
			}
		}
		else if(mode >=3 && mode<=9)
		{
	
			int up_d;//距离mode 2的个数
			int m=mode;
			switch (m)
			{
				case 3:		up_d=6;break;
				case 4:		up_d=11;break;
				case 5:		up_d=15;break;
				case 6:		up_d=19;break;
				case 7:		up_d=23;break;
				case 8:		up_d=27;break;
				case 9:		up_d=30;break;
				default:	break;
			}

			//LA/A/RA方向全为0
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=0;
				Percen[m][1][i]=0;
				Percen[m][2][i]=0;
			}

			//计算L 和LB
			if((2*up_d)%4 == 0)//2个零界点，2段
			{
				int L_up=16-floor(2*up_d*1.0/4)-1;
				int L_down=L_up+1;
				for(int i=0;i<=L_up;i++)
				{
					int A=i*4;
					int B=(i+1)*4;
					Percen[m][3][L_up-i]=TrianglePerEqu(A,B,up_d);//L方向
					Percen[m][4][i]=Percen[m][3][L_up-i];//LB方向
				}
				for(int i=L_down;i<16;i++)
				{
					Percen[m][3][i]=1.0/16;
					Percen[m][4][i]=0;
				}
			}
			else //三段
			{
				int ys=(2*up_d)%4;
				int zs=(2*up_d)/4;
				int L_up=16-ceil(2*up_d*1.0/4)-1;
				int L_mid=L_up+1;
				int L_down=L_up+2;	//16-floor(2*up_d*1.0/4);
				for(int i=0;i<=L_up;i++)
				{
					int A,B;
					A=4-ys+(L_up-i)*4;
					B=4-ys+(L_up-i+1)*4;
					Percen[m][3][i]=TrianglePerEqu(A,B,up_d);

					A=i*4;
					B=(i+1)*4;
					Percen[m][4][i]=TrianglePerEqu(A,B,up_d);
					}
				for(int i=L_mid;i<L_down;i++)
				{
					int A,B;
					A=0;
					B=4-ys;
					Percen[m][3][i]=TrianglePerEqu(A,B,up_d)+ys*1.0/64;

					A=4-ys;
					B=64-2*up_d;
					Percen[m][4][i]=TrianglePerEqu(A,B,up_d);
				}
				for(int i=L_down;i<16;i++)
				{
					Percen[m][3][i]=1.0/16;
					Percen[m][4][i]=0;
				}
			}
		}
		else if(mode >=27 && mode <=33)	//L方向和A完全一致 	LB方向和RA完全一致
		{
			int le_d;//距离mode34的格数
			int m=mode;
			switch (m)
			{
				case 33:	le_d=6;break;
				case 32:	le_d=11;break;
				case 31:	le_d=15;break;
				case 30:	le_d=19;break;
				case 29:	le_d=23;break;
				case 28:	le_d=27;break;
				case 27:	le_d=30;break;
				default:	break;
			}

			//LA/L/LB方向全为0
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=0;
				Percen[m][3][i]=0;
				Percen[m][4][i]=0;
			}

			//计算A和RA
			if((2*le_d)%4 == 0)//2个零界点，2段
			{
				int L_up=16-floor(2*le_d*1.0/4)-1;
				int L_down=L_up+1;
				for(int i=0;i<=L_up;i++)
				{
					int A=i*4;
					int B=(i+1)*4;
					Percen[m][1][L_up-i]=TrianglePerEqu(A,B,le_d);//L方向
					Percen[m][2][i]=Percen[m][1][L_up-i];//LB方向
				}
				for(int i=L_down;i<16;i++)
				{
					Percen[m][1][i]=1.0/16;
					Percen[m][2][i]=0;
				}
			}
			else //三段
			{
				//L方向	
				int ys=(2*le_d)%4;
				int zs=(2*le_d)/4;
				int L_up=16-ceil(2*le_d*1.0/4)-1;
				int L_mid=L_up+1;
				int L_down=L_up+2;	//16-floor(2*up_d*1.0/4);
				for(int i=0;i<=L_up;i++)
				{
					int A,B;
					A=4-ys+(L_up-i)*4;
					B=4-ys+(L_up-i+1)*4;
					Percen[m][1][i]=TrianglePerEqu(A,B,le_d);

					A=i*4;
					B=(i+1)*4;
					Percen[m][2][i]=TrianglePerEqu(A,B,le_d);
				}
				for(int i=L_mid;i<L_down;i++)
				{
					int A,B;
					A=0;
					B=4-ys;
					Percen[m][1][i]=TrianglePerEqu(A,B,le_d)+ys*1.0/64;

					A=4-ys;
					B=64-2*le_d;
					Percen[m][2][i]=TrianglePerEqu(A,B,le_d);
				}
				for(int i=L_down;i<16;i++)
				{
					Percen[m][1][i]=1.0/16;
					Percen[m][2][i]=0;
				}
			}
		}
		else if(mode >=11 && mode <=17)
		{
			int m=mode;
			int dn_d;//距离mode 18的格数
			switch (m)
			{
				case 17:	dn_d=6;break;
				case 16:	dn_d=11;break;
				case 15:	dn_d=15;break;
				case 14:	dn_d=19;break;
				case 13:	dn_d=23;break;
				case 12:	dn_d=27;break;
				case 11:	dn_d=30;break;
				default:	break;
			}
		
			//LB和RA和LA为0
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=0;
				Percen[m][2][i]=0;
				Percen[m][4][i]=0;
			}

			//计算A
			for(int i=0;i<16;i++)
			{
				int A=i*4;
				int B=(i+1)*4;
				Percen[m][1][i]=Triangle2PerEqu(A,B,dn_d);
			}

			//计算L
			if((2*dn_d)%4 == 0)//2个零界点，2段
			{
				int L_up=floor(2*dn_d*1.0/4)-1;	//计算L
				int L_down=L_up+1;
				for(int i=0;i<=L_up;i++)
				{
					Percen[m][3][i]=1.0/16;
				}
				for(int i=L_down;i<16;i++)
				{
					int A=(i-L_down)*4;
					int B=(i-L_down+1)*4;
					Percen[m][3][i]=TrianglePerEqu(A,B,dn_d);
				}
			}
			else
			{	
				int ys=(2*dn_d)%4;
				int zs=(2*dn_d)/4;
				int L_up=floor(2*dn_d*1.0/4)-1;
				int L_mid=L_up+1;
				int L_down=L_up+2;
				for(int i=0;i<=L_up;i++)
				{
					Percen[m][3][i]=1.0/16;
				}
				for(int i=L_mid;i<L_mid+1;i++)
				{
					int A=0;
					int B=4-ys;
					Percen[m][3][i]=TrianglePerEqu(A,B,dn_d)+ys*1.0/64;
				}
				for(int i=L_down;i<16;i++)
				{
					int A=4-ys+(i-L_down)*4;
					int B=4-ys+(i-L_down+1)*4;
					Percen[m][3][i]=TrianglePerEqu(A,B,dn_d);
				}
			}
		}
		else if(mode >=19 && mode <=25)	//A与上一种的L对应，L与上一种的A对应
		{
			int m=mode;
			int ri_d;//距离mode 18的格数
			switch (m)
			{
				case 19:	ri_d=6;break;
				case 20:	ri_d=11;break;
				case 21:	ri_d=15;break;
				case 22:	ri_d=19;break;
				case 23:	ri_d=23;break;
				case 24:	ri_d=27;break;
				case 25:	ri_d=30;break;
				default:	break;
			}
		
			//LB和RA和LA为0
			for(int i=0;i<16;i++)
			{
				Percen[m][0][i]=0;
				Percen[m][2][i]=0;
				Percen[m][4][i]=0;
			}

			//计算L
			for(int i=0;i<16;i++)
			{
				int A=i*4;
				int B=(i+1)*4;
				Percen[m][3][i]=Triangle2PerEqu(A,B,ri_d);
			}

			//计算A
			if((2*ri_d)%4 == 0)//2个零界点，2段
			{
				int L_up=floor(2*ri_d*1.0/4)-1;	//计算L
				int L_down=L_up+1;
				for(int i=0;i<=L_up;i++)
				{
					Percen[m][1][i]=1.0/16;
				}
				for(int i=L_down;i<16;i++)
				{
					int A=(i-L_down)*4;
					int B=(i-L_down+1)*4;
					Percen[m][1][i]=TrianglePerEqu(A,B,ri_d);
				}
			}
			else
			{	
				int ys=(2*ri_d)%4;
				int zs=(2*ri_d)/4;
				int L_up=floor(2*ri_d*1.0/4)-1;
				int L_mid=L_up+1;
				int L_down=L_up+2;
				for(int i=0;i<=L_up;i++)
				{
					Percen[m][1][i]=1.0/16;
				}
				for(int i=L_mid;i<L_mid+1;i++)
				{
					int A=0;
					int B=4-ys;
					Percen[m][1][i]=TrianglePerEqu(A,B,ri_d)+ys*1.0/64;
				}
				for(int i=L_down;i<16;i++)
				{
					int A=4-ys+(i-L_down)*4;
					int B=4-ys+(i-L_down+1)*4;
					Percen[m][1][i]=TrianglePerEqu(A,B,ri_d);
				}
			}	
		}
	}
}

void NALInfo::DeleteSpatEffTable()
{
	for(int m=0;m<35;m++)
	{
		for(int d=0;d<5;d++)
		{
			delete [] Percen[m][d];
		}
		delete [] Percen[m];
	}
	Percen=NULL;
}

//打印
void NALInfo::PrintPUTable(int SliceType)
{

	//-----------zigzag倒序-------------------
	int ctu_h=(int)ceil((PicHei*1.0)/64);
	int ctu_w=(int)ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;

	for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	{
		for(int j=ctu_w-1;j>=0;j--)
		{
			int curctusize_x=j;
			int curctusize_y=i;
			//printf("CTU(%d,%d)\n",curctusize_x,curctusize_y);
			//下面是CTU内部的深度优先扫描zigz,四层for: k l m n
			for(int k=3;k>=0;k--)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				//printf("32size(%d,%d)\n",cur32size_x,cur32size_y);
				for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					//printf("16size(%d,%d)\n",cur16size_x,cur16size_y);
					for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;
						//printf("8size(%d,%d)\n",cur8size_x,cur8size_y);
						for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							//printf("4size(%d,%d)\n",cur4size_x,cur4size_y);



							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							//printf("PU(%d,%d)\n",PUTable_x,PUTable_y);

							if((PUTable_x < table_w) && (PUTable_y < table_h))
							{
								//如果是I frame
								if(SliceType==I_SLICE)
								{
									if(PUTable[PUTable_x][PUTable_y].IntraPredMode != 36)//被赋值的4x4PU
									{
										//printf("CU(%d,%d)-PU(%d,%d) size=%02dX%02d mode=%02d off=%06x:%d\n",
										//	PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x,
										//	PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y,
										//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_x,
										//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_y,
										//	PUTable[PUTable_x][PUTable_y].PUSize,
										//	PUTable[PUTable_x][PUTable_y].PUSize,
										//	PUTable[PUTable_x][PUTable_y].IntraPredMode,
										//	PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset/8,
										//	PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset%8);
									}								
								}
								else if(SliceType==B_SLICE || SliceType==P_SLICE)//如果是P Bfram 根据inter mode判定
								{
									if(PUTable[PUTable_x][PUTable_y].InterPredDir.ForwDir != 5)
									{
										//printf("CU(%d,%d)-PU(%d,%d) off=%06x:%d	Formdir=%d BackDir=%d\n",
										//	PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x,
										//	PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y,
										//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_x,
										//	PUTable[PUTable_x][PUTable_y].PUInPicLoc_y,
										//	//PUTable[PUTable_x][PUTable_y].PUSize,
										//	//PUTable[PUTable_x][PUTable_y].PUSize,
										//	//PUTable[PUTable_x][PUTable_y].IntraPredMode,
										//	PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset/8,
										//	PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset%8,
										//	PUTable[PUTable_x][PUTable_y].InterPredDir.ForwDir,
										//	PUTable[PUTable_x][PUTable_y].InterPredDir.BackDir);
									}
								
								}
							}
						}
					}
				}
			}
		}
	}
}

void NALInfo::PrintSptEffTable()
{
	for(int m=0;m<35;m++)
	{
		for(int d=0;d<5;d++)
		{
			for(int l=0;l<16;l++)
			{
				printf("SpaEffTab[%d][%d][%d]=%f\n",m,d,l,Percen[m][d][l]);
			}
		}
	}
}

int CostBits::ComputeThisCostbits(int idxbefore,int idxafter,int neededbefore,int neededafter)
{
	return (0-neededbefore)+8*(idxafter-idxbefore)+neededafter;
}

int CUCostBits::ComputeThisCostbits(int idxbefore,int idxafter,int neededbefore,int neededafter)
{
	return (0-neededbefore)+8*(idxafter-idxbefore)+neededafter;
}

void NALInfo::ComputeChunkInFileOffset(AnnexBStats stats)
{
	BelongChunkInfo.ChunkInFileOffset=BelongChunkInfo.ChunkInFileOffset+BelongChunkInfo.ChunkLen;
 	BelongChunkInfo.ChunkLen=stats.m_numZeroByteBytes+stats.m_numStartCodePrefixBytes+stats.m_numBytesInNALUnit+stats.m_numTrailingZero8BitsBytes;		
	
	//printf("Chunk in File offset=%08x,Chunk len=%d Byte\n",
	//	BelongChunkInfo.ChunkInFileOffset, 
	//	BelongChunkInfo.ChunkLen);	
}

void NALInfo::ComputeNALHeaderInFileOffset(AnnexBStats stats)
{
	ComputeNALHeaderInNALOffset();
 	NALHeaderInfo.NALHeaderOffset.InFileOffsset=BelongChunkInfo.ChunkInFileOffset+stats.m_numZeroByteBytes+stats.m_numStartCodePrefixBytes;
	NALLen=stats.m_numBytesInNALUnit;
	NALHeaderInfo.NALHeaderLen=2;

	//printf("NAL Header in File offset=%08x,NAL len=%d Byte\n",
	//	NALHeaderInfo.NALHeaderOffset.InFileOffsset, 
	//	NALLen);
}

void NALInfo::ComputeNALHeaderInNALOffset()
{
	NALHeaderInfo.NALHeaderOffset.InNALOffset=0;
	NALHeaderInfo.NALHeaderCostBits.Totalcostbits=16;
	MY_bitoffsetinNAL += NALHeaderInfo.NALHeaderCostBits.Totalcostbits;
}

void NALInfo::ComputeSliceHeaderOffset(int MY_m_fifo_idx_cur)
{
	//--------New-------------------
	SliceHeaderInfo.SliceHeaderLen=MY_m_fifo_idx_cur-MY_m_fifo_idx_my;
	SliceHeaderInfo.SliceHeaderCostBits.Totalcostbits=8*(MY_m_fifo_idx_cur-MY_m_fifo_idx_my);
	SliceHeaderInfo.SliceHeaderOffset.InNALOffset=NALHeaderInfo.NALHeaderOffset.InNALOffset+NALHeaderInfo.NALHeaderLen;
	SliceHeaderInfo.SliceHeaderOffset.InFileOffsset=NALHeaderInfo.NALHeaderOffset.InFileOffsset+NALHeaderInfo.NALHeaderLen;
	MY_bitoffsetinNAL += SliceHeaderInfo.SliceHeaderCostBits.Totalcostbits;
	//MY_m_fifo_idx_my=MY_m_fifo_idx_cur;  原来没有
	
	//printf("Slice header in NAL offset=%08x\nSlice header in File offset=%08x\nSlice Header len=%d Byte\n",
	//	SliceHeaderInfo.SliceHeaderOffset.InNALOffset,
	//	SliceHeaderInfo.SliceHeaderOffset.InFileOffsset,
	//	SliceHeaderInfo.SliceHeaderLen);
	//------------------------------
}

void NALInfo::ComputeSAOOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//------------New------------------------
	SAOInfo.SAOCostBits.Totalcostbits=SAOInfo.SAOCostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	SAOInfo.SAOOffsetInfo.InNALOffset=MY_bitoffsetinNAL;
	SAOInfo.SAOOffsetInfo.InFileOffsset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+SAOInfo.SAOOffsetInfo.InNALOffset;
	MY_bitoffsetinNAL += SAOInfo.SAOCostBits.Totalcostbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	
	int curByte=(MY_bitoffsetinNAL-SAOInfo.SAOCostBits.Totalcostbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("SAO in NAL offset=%08x:%d\nSAO in File offset=%08x:%d\nSAO Total costbits=%d\n",
	//	SAOInfo.SAOOffsetInfo.InNALOffset/8,
	//	SAOInfo.SAOOffsetInfo.InNALOffset%8,
	//	SAOInfo.SAOOffsetInfo.InFileOffsset/8,
	//	SAOInfo.SAOOffsetInfo.InFileOffsset%8,
	//	SAOInfo.SAOCostBits.Totalcostbits);

}

void NALInfo::ComputeSplitFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------------new-------------
	int SplitFlagToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int SplitFlagInNALOffset=MY_bitoffsetinNAL;
	int SplitFlagInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+SplitFlagInNALOffset;
	MY_bitoffsetinNAL += SplitFlagToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += SplitFlagToatlbits;
	myCUInfo.CostBits.ResiBits=0;

	int curByte=(MY_bitoffsetinNAL-SplitFlagToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("SplitFlag in NAL offset=%08x:%d\nSplitFlag in File offset=%08x:%d\nSplitFlag Toatlbits=%d\n",
	//	SplitFlagInNALOffset/8,
	//	SplitFlagInNALOffset%8,
	//	SplitFlagInFileOffset/8,
	//	SplitFlagInFileOffset%8,
	//	SplitFlagToatlbits);

	//PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.SAO.InFileOffsset=SAOInfo.SAOOffsetInfo.InFileOffsset;
	//PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.SAOLen=SAOInfo.SAOCostBits.Totalcostbits;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.SplitFlag.InFileOffsset=SplitFlagInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.SplitFlagLen=SplitFlagToatlbits;
}

void NALInfo::ComputeSkipFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int SkipFlagToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int SkipFlagInNALOffset=MY_bitoffsetinNAL;
	int SkipFlagInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+SkipFlagInNALOffset;
	MY_bitoffsetinNAL += SkipFlagToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += SkipFlagToatlbits;

	int curByte=(MY_bitoffsetinNAL-SkipFlagToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("SkipFlag in NAL offset=%08x:%d\nSkipFlag in File offset=%08x:%d\nSkipFlag Toatlbits=%d\n",
	//	SkipFlagInNALOffset/8,
	//	SkipFlagInNALOffset%8,
	//	SkipFlagInFileOffset/8,
	//	SkipFlagInFileOffset%8,
	//	SkipFlagToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.SkipFlag.InFileOffsset=SkipFlagInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.SkipFlagLen=SkipFlagToatlbits;

}

void NALInfo::ComputeMergeIndexOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------

	//UInt form2byte = myfifo[MY_m_fifo_idx_my-2];
	//UInt form1byte = myfifo[MY_m_fifo_idx_my-1];
	//UInt curbyte = myfifo[MY_m_fifo_idx_my];
	//Bool addflag=AddZerothree(form2byte,form1byte,curbyte);
	//if(addflag)//len+1byte
	//{
	//	printf("我是%X %X 03 %X的%X\n",form2byte,form1byte,curbyte,curbyte);
	//}
	//else//len正常计算
	//{
	//	//pritnf();;
	//}
	int MergeIndexToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int MergeIndexInNALOffset=MY_bitoffsetinNAL;
	int MergeIndexInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+MergeIndexInNALOffset;
	MY_bitoffsetinNAL += MergeIndexToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += MergeIndexToatlbits;

	int curByte=(MY_bitoffsetinNAL-MergeIndexToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("MergeIndex in NAL offset=%08x:%d\nMergeIndex in File offset=%08x:%d\nMergeIndex Toatlbits=%d\n",
	//	MergeIndexInNALOffset/8,
	//	MergeIndexInNALOffset%8,
	//	MergeIndexInFileOffset/8,
	//	MergeIndexInFileOffset%8,
	//	MergeIndexToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.MergeIdx.InFileOffsset=MergeIndexInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.MergeIdxLen=MergeIndexToatlbits;
}

void NALInfo::ComputePredModeOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int PredModeToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int PredModeInNALOffset=MY_bitoffsetinNAL;
	int PredModeInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+PredModeInNALOffset;
	MY_bitoffsetinNAL += PredModeToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += PredModeToatlbits;

	int curByte=(MY_bitoffsetinNAL-PredModeToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("PredMode in NAL offset=%08x:%d\nPredMode in File offset=%08x:%d\nPredMode Toatlbits=%d\n",
	//	PredModeInNALOffset/8,
	//	PredModeInNALOffset%8,
	//	PredModeInFileOffset/8,
	//	PredModeInFileOffset%8,
	//	PredModeToatlbits);

}

void NALInfo::ComputePartSizeOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int PartSizeToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int PartSizeInNALOffset=MY_bitoffsetinNAL;
	int PartSizeInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+PartSizeInNALOffset;
	MY_bitoffsetinNAL += PartSizeToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += PartSizeToatlbits;

	int curByte=(MY_bitoffsetinNAL-PartSizeToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("PartSize in NAL offset=%08x:%d\nPartSize in File offset=%08x:%d\nPartSize Toatlbits=%d\n",
	//	PartSizeInNALOffset/8,
	//	PartSizeInNALOffset%8,
	//	PartSizeInFileOffset/8,
	//	PartSizeInFileOffset%8,
	//	PartSizeToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.PartSize.InFileOffsset=PartSizeInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.PartSizeLen=PartSizeToatlbits;
}

void NALInfo::ComputeIPCMOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int IPCMToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int IPCMInNALOffset=MY_bitoffsetinNAL;
	int IPCMInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+IPCMInNALOffset;
	MY_bitoffsetinNAL += IPCMToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += IPCMToatlbits;

	int curByte=(MY_bitoffsetinNAL-IPCMToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("IPCM in NAL offset=%08x:%d\nIPCM in File offset=%08x:%d\nIPCM Toatlbits=%d\n",
	//	IPCMInNALOffset/8,
	//	IPCMInNALOffset%8,
	//	IPCMInFileOffset/8,
	//	IPCMInFileOffset%8,
	//	IPCMToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.IPCM.InFileOffsset=IPCMInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.IPCMLen=IPCMToatlbits;
}

void NALInfo::ComputePredOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int PredToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int PredInNALOffset=MY_bitoffsetinNAL;
	int PredInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+PredInNALOffset;
	MY_bitoffsetinNAL += PredToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += PredToatlbits;

	int curByte=(MY_bitoffsetinNAL-PredToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("Luma and Chroma Pred mode in NAL offset=%08x:%d\nLuma and Chroma mode in File offset=%08x:%d\nLuma and Chroma mode Toatlbits=%d\n",
	//	PredInNALOffset/8,
	//	PredInNALOffset%8,
	//	PredInFileOffset/8,
	//	PredInFileOffset%8,
	//	PredToatlbits);
}

void NALInfo::ComputeIntraLumaPredOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int IntraCULumaPredModeToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int IntraCULumaPredModeInNALOffset=MY_bitoffsetinNAL;
	int IntraCULumaPredModeInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+IntraCULumaPredModeInNALOffset;
	MY_bitoffsetinNAL += IntraCULumaPredModeToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += IntraCULumaPredModeToatlbits;


	int curByte=(MY_bitoffsetinNAL-IntraCULumaPredModeToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("Luma Pred mode in NAL offset=%08x:%d\nLuma Pred mode in File offset=%08x:%d\nLuma Pred mode Toatlbits=%d\n",
	//	IntraCULumaPredModeInNALOffset/8,
	//	IntraCULumaPredModeInNALOffset%8,
	//	IntraCULumaPredModeInFileOffset/8,
	//	IntraCULumaPredModeInFileOffset%8,
	//	IntraCULumaPredModeToatlbits);


	//给CU的Luma pred offset赋值
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.LumaDirMode.InFileOffsset=IntraCULumaPredModeInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.LumaDirModeLen=IntraCULumaPredModeToatlbits;
	//printf("CU(%d,%d) lumamode=%d offset=%08x:%d len=%d bits\n",
	//	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CULocInPic.x,
	//	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CULocInPic.y,
	//	PUTable[CurCUInTab_x][CurCUInTab_y].IntraPredMode,
	//	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.LumaDirModeLuma.InFileOffsset/8,
	//	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.LumaDirModeLuma.InFileOffsset%8,
	//	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.LumaDirModeLumaLen);
}

void NALInfo::ComputeIntraChromaPredOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int IntraCUChromaPredModeToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int IntraCUChromaPredModeInNALOffset=MY_bitoffsetinNAL;
	int IntraCUChromaPredModeInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+IntraCUChromaPredModeInNALOffset;
	MY_bitoffsetinNAL += IntraCUChromaPredModeToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += IntraCUChromaPredModeToatlbits;


	int curByte=(MY_bitoffsetinNAL-IntraCUChromaPredModeToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);	
	//printf("chroma Pred mode in NAL offset=%08x:%d\nchroma Pred mode in File offset=%08x:%d\nchroma Pred mode Toatlbits=%d\n",
	//	IntraCUChromaPredModeInNALOffset/8,
	//	IntraCUChromaPredModeInNALOffset%8,
	//	IntraCUChromaPredModeInFileOffset/8,
	//	IntraCUChromaPredModeInFileOffset%8,
	//	IntraCUChromaPredModeToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.ChromaDirMode.InFileOffsset=IntraCUChromaPredModeInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.ChromaDirModeLen=IntraCUChromaPredModeToatlbits;

}

void NALInfo::ComputeMergeFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int MergeFlagToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int MergeFlagInNALOffset=MY_bitoffsetinNAL;
	int MergeFlagInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+MergeFlagInNALOffset;
	MY_bitoffsetinNAL += MergeFlagToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += MergeFlagToatlbits;

	int curByte=(MY_bitoffsetinNAL-MergeFlagToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
	//printf("MergeFlag in NAL offset=%08x:%d\nMergeFlag in File offset=%08x:%d\nMergeFlag Toatlbits=%d\n",
	//	MergeFlagInNALOffset/8,
	//	MergeFlagInNALOffset%8,
	//	MergeFlagInFileOffset/8,
	//	MergeFlagInFileOffset%8,
	//	MergeFlagToatlbits);

}

void NALInfo::ComputeInterDirPUOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int InterDirPUToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int InterDirPUInNALOffset=MY_bitoffsetinNAL;
	int InterDirPUInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+InterDirPUInNALOffset;
	MY_bitoffsetinNAL += InterDirPUToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += InterDirPUToatlbits;

	int curByte=(MY_bitoffsetinNAL-InterDirPUToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
	//printf("InterDirPU in NAL offset=%08x:%d\nInterDirPU in File offset=%08x:%d\nInterDirPU Toatlbits=%d\n",
	//	InterDirPUInNALOffset/8,
	//	InterDirPUInNALOffset%8,
	//	InterDirPUInFileOffset/8,
	//	InterDirPUInFileOffset%8,
	//	InterDirPUToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.InterDir.InFileOffsset=InterDirPUInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.InterDirLen=InterDirPUToatlbits;

}

void NALInfo::ComputeRefFrameIdxOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int RefFrameIdxToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int RefFrameIdxInNALOffset=MY_bitoffsetinNAL;
	int RefFrameIdxInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+RefFrameIdxInNALOffset;
	MY_bitoffsetinNAL += RefFrameIdxToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += RefFrameIdxToatlbits;

	int curByte=(MY_bitoffsetinNAL-RefFrameIdxToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
	//printf("RefFrameIdx in NAL offset=%08x:%d\nRefFrameIdx in File offset=%08x:%d\nRefFrameIdx Toatlbits=%d\n",
	//	RefFrameIdxInNALOffset/8,
	//	RefFrameIdxInNALOffset%8,
	//	RefFrameIdxInFileOffset/8,
	//	RefFrameIdxInFileOffset%8,
	//	RefFrameIdxToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.RefIdx.InFileOffsset=RefFrameIdxInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.RefIdxLen=RefFrameIdxToatlbits;
}

void NALInfo::ComputeMVDPUOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int MVDPUToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int MVDPUInNALOffset=MY_bitoffsetinNAL;
	int MVDPUInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+MVDPUInNALOffset;
	MY_bitoffsetinNAL += MVDPUToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += MVDPUToatlbits;

	int curByte=(MY_bitoffsetinNAL-MVDPUToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
	//printf("MVDPU in NAL offset=%08x:%d\nMVDPU in File offset=%08x:%d\nMVDPU Toatlbits=%d\n",
	//	MVDPUInNALOffset/8,
	//	MVDPUInNALOffset%8,
	//	MVDPUInFileOffset/8,
	//	MVDPUInFileOffset%8,
	//	MVDPUToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.MVD.InFileOffsset=MVDPUInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.MVDLen=MVDPUToatlbits;
}

void NALInfo::ComputeMVPIdxOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int MVPIdxToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int MVPIdxInNALOffset=MY_bitoffsetinNAL;
	int MVPIdxInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+MVPIdxInNALOffset;
	MY_bitoffsetinNAL += MVPIdxToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += MVPIdxToatlbits;

	int curByte=(MY_bitoffsetinNAL-MVPIdxToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
	//printf("MVPIdx in NAL offset=%08x:%d\nMVPIdx in File offset=%08x:%d\nMVPIdx Toatlbits=%d\n",
	//	MVPIdxInNALOffset/8,
	//	MVPIdxInNALOffset%8,
	//	MVPIdxInFileOffset/8,
	//	MVPIdxInFileOffset%8,
	//	MVPIdxToatlbits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.AMVPIdx.InFileOffsset=MVPIdxInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.AMVPIdxLen=MVPIdxToatlbits;
}

void NALInfo::ComputeCoeffOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	myCUInfo.CostBits.ResiBits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int CoeffInNALOffset=MY_bitoffsetinNAL;
	int CoeffInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+CoeffInNALOffset;
	MY_bitoffsetinNAL += myCUInfo.CostBits.ResiBits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += myCUInfo.CostBits.ResiBits;

	int curByte=(MY_bitoffsetinNAL-myCUInfo.CostBits.ResiBits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
	//printf("Coeff in NAL offset=%08x:%d\nCoeff in File offset=%08x:%d\nCoeff Toatlbits=%d\n",
	//	CoeffInNALOffset/8,
	//	CoeffInNALOffset%8,
	//	CoeffInFileOffset/8,
	//	CoeffInFileOffset%8,
	//	CUInfo.CostBits.ResiBits);
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.Coefficient.InFileOffsset=CoeffInFileOffset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CoefficientLen=myCUInfo.CostBits.ResiBits;
}

void NALInfo::ComputeTransquantBypassFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	//-------------New--------------
	int BypassFlagToatlbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	int BypassFlagInNALOffset=MY_bitoffsetinNAL;
	int BypassFlagInFileOffset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+BypassFlagInNALOffset;
	MY_bitoffsetinNAL += BypassFlagToatlbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	myCUInfo.CostBits.Totalcostbits += BypassFlagToatlbits;

	int curByte=(MY_bitoffsetinNAL-BypassFlagToatlbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
}

void NALInfo::ComputeFinishDecodeCUOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter)
{
	int FinishCostbits=myCUInfo.CostBits.ComputeThisCostbits(MY_m_fifo_idx_my,MY_m_fifo_idx_cur,bitsNeededBefore,bitsNeededafter);
	MY_bitoffsetinNAL +=FinishCostbits;
	MY_m_fifo_idx_my=MY_m_fifo_idx_cur;
	

	myCUInfo.CostBits.Totalcostbits += FinishCostbits;
	myCUInfo.CostBits.PredBits=myCUInfo.CostBits.Totalcostbits-myCUInfo.CostBits.ResiBits;
	myCUInfo.CUOffsetInfo.InNALOffset=MY_bitoffsetinNAL-myCUInfo.CostBits.Totalcostbits;
	myCUInfo.CUOffsetInfo.InFileOffsset=8*NALHeaderInfo.NALHeaderOffset.InFileOffsset+myCUInfo.CUOffsetInfo.InNALOffset;
	
	//int curByte=CUInfo.CUOffsetInfo.InNALOffset/8;
	//int NextByte=(CUInfo.CUOffsetInfo.InNALOffset+CUInfo.CostBits.Totalcostbits)/8;
	int curByte=(MY_bitoffsetinNAL-FinishCostbits)/8;
	int NextByte=MY_bitoffsetinNAL/8;
	DecAddEight(curByte,NextByte);
	//--------new--set CU off--------
	//PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CUOffsetInfo.InFileOffsset=CUInfo.CUOffsetInfo.InFileOffsset;
	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CostBits.Totalcostbits=myCUInfo.CostBits.Totalcostbits;
	if(PUTable[CurCUInTab_x][CurCUInTab_y].TruPUSize==4)
	{
		for(int i=0;i<2;i++)
		{
			for(int j=0;j<2;j++)
			{
				PUTable[(CurCUInTab_x)+j][(CurCUInTab_y)+i].BelCU.CUOffsetInfo.InFileOffsset=myCUInfo.CUOffsetInfo.InFileOffsset;
			}	
		}
	}
	else
	{
		PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CUOffsetInfo.InFileOffsset=myCUInfo.CUOffsetInfo.InFileOffsset;
	}	
	//---------------------

	//printf("CU(%d,%d) in NAL offset=%08x:%d\nCU in File offset=%08x:%d\nCU total bits=%d\nCU pred bits=%d\nCU resi bits=%d\n",
	//	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CULocInPic.x,
	//	PUTable[CurCUInTab_x][CurCUInTab_y].BelCU.CULocInPic.y,
	//	CUInfo.CUOffsetInfo.InNALOffset/8,
	//	CUInfo.CUOffsetInfo.InNALOffset%8,
	//	CUInfo.CUOffsetInfo.InFileOffsset/8,
	//	CUInfo.CUOffsetInfo.InFileOffsset%8,
	//	CUInfo.CostBits.Totalcostbits,
	//	CUInfo.CostBits.PredBits,
	//	CUInfo.CostBits.ResiBits);
	myCUInfo.CostBits.Totalcostbits=0;


}

//模型计算
Float NALInfo::ComputeModePercen(int mode, int direction ,int label)
{
	float per=Percen[mode][direction][label];
	return per;
}

float TrianglePerEqu(int A,int B,int d)
{
	float a=(32-d)*1.0/64; //S上三角/S矩形
	float b=((pow((64-2*d-A),2)-pow((64-2*d-B),2))*1.0)/pow((64-2*d),2);//求平方差
	float per=a*b;
	return per;
}
 
float Triangle2PerEqu(int A,int B,int d)
{
	float a=(32-d)*1.0/64; //S上三角/S矩形
	float b=((pow((64-A),2)-pow((64-B),2))*1.0)/pow(64,2);//求平方差
	float per=a*b;
	return per;
}

float ComputePUSpaEff(float percen,int curPusize,int truPUsize)
{
	//float spaeff=(percen*curPusize*1.0)/truPUsize;
	float spaeff=percen;
	return spaeff;
}

void NALInfo::ModelCalcute(SliceType slicetype)
{
	//printf("Slice %d",slicetype);
	if(slicetype==I_SLICE)
	{
		ComputeIntraPUSpatEFF();
		ComputeCUSpatEff();
		if(CUnum>0)
		{
			SortCUEff();
		}
	}
	else if(slicetype==P_SLICE || slicetype==B_SLICE)
	{
		ComputeInterPUSpaEff();
		ComputeCUSpatEff();
		if(CUnum>0)
		{
			SortCUEff();
		}
		
	}	
	//ComputeCUSpatEff();
	//SortCUEff();
}

void NALInfo::ComputeIntraPUSpatEFF()
{

	//-----------zigzag倒序-------------------
	int ctu_h=(int)ceil((PicHei*1.0)/64);
	int ctu_w=(int)ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;

	for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	{
		for(int j=ctu_w-1;j>=0;j--)
		{
			int curctusize_x=j;
			int curctusize_y=i;
			//printf("CTU(%d,%d)\n",curctusize_x,curctusize_y);
			//下面是CTU内部的深度优先扫描zigz,四层for: k l m n
			for(int k=3;k>=0;k--)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				//printf("32size(%d,%d)\n",cur32size_x,cur32size_y);
				for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					//printf("16size(%d,%d)\n",cur16size_x,cur16size_y);
					for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;
						//printf("8size(%d,%d)\n",cur8size_x,cur8size_y);
						for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
							if(valid_xy)
							{
								Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
								if(FirPuInTruPU)//是一个truPU中的first 4x4Pu
								{					
									int curTruePUsize=PUTable[PUTable_x][PUTable_y].TruPUSize;
									int curTruePUmode=PUTable[PUTable_x][PUTable_y].IntraPredMode;
									ComputeIntraPUNeiPUSpaEff(PUTable_x,PUTable_y,curTruePUsize,curTruePUmode);
								}									
							}					
						}
					}
				}
			}
		}
	}
}

void NALInfo::ComputeInterPUSpaEff()
{
	//printf("Frame %d:\n",FrameDecNo);

	//-----------zigzag倒序-------------------
	int ctu_h=(int)ceil((PicHei*1.0)/64);
	int ctu_w=(int)ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;

	for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	{
		for(int j=ctu_w-1;j>=0;j--)
		{
			int curctusize_x=j;
			int curctusize_y=i;
			//printf("CTU(%d,%d)\n",curctusize_x,curctusize_y);
			//下面是CTU内部的深度优先扫描zigz,四层for: k l m n
			for(int k=3;k>=0;k--)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				//printf("32size(%d,%d)\n",cur32size_x,cur32size_y);
				for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					//printf("16size(%d,%d)\n",cur16size_x,cur16size_y);
					for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;
						//printf("8size(%d,%d)\n",cur8size_x,cur8size_y);
						for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
							if(valid_xy)
							{
								Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
								if(FirPuInTruPU)//是一个PU的first 4x4PU
								{					
									int curPUw=PUTable[PUTable_x][PUTable_y].TruPUw;
									int curPUh=PUTable[PUTable_x][PUTable_y].TruPUh;
									int curPUFormDir=PUTable[PUTable_x][PUTable_y].InterPredDir.ForwDir;
									int curPUBackDir=PUTable[PUTable_x][PUTable_y].InterPredDir.BackDir;
									ComInterPUNeiPUSpaEff(PUTable_x,PUTable_y,curPUw,curPUh,curPUFormDir,curPUBackDir);
								}									
							}					
						}
					}
				}
			}
		}
	}
}

void NALInfo::ComputeIntraPUNeiPUSpaEff(int CurPuLoc_x,int CurPuLoc_y,int CurPuSize,int CurPuMode)
{
	TableU LA_PU; //4x4 LA_PU在Table中的下标x,y
	TableU *A_PU;
	TableU *RA_PU;
	TableU *L_PU;
	TableU *LB_PU;
	int neiPUnum=CurPuSize/4;		//边长上有几个4x4值域[16,8,4,2,1]
	A_PU	= new TableU[neiPUnum];
	RA_PU	= new TableU[neiPUnum];
	L_PU	= new TableU[neiPUnum];
	LB_PU	= new TableU[neiPUnum];

	//初始化neighbour 4x4
	LA_PU.init();
	for(int i=0;i<neiPUnum;i++)
	{
		A_PU[i].init();
		RA_PU[i].init();
		L_PU[i].init();
		LB_PU[i].init();
	}

	//计算nei 4x4PU在table的下标
	LA_PU.x=CurPuLoc_x-1;	//左上4x4PU 在table中坐标
	LA_PU.y=CurPuLoc_y-1;
	for(int i=0;i<neiPUnum;i++)
	{
		A_PU[i].x=CurPuLoc_x+i;
		A_PU[i].y=CurPuLoc_y-1;//固定 
		RA_PU[i].x=CurPuLoc_x+i+neiPUnum;
		RA_PU[i].y=CurPuLoc_y-1;//固定 
		L_PU[i].x=CurPuLoc_x-1;//固定
		L_PU[i].y=CurPuLoc_y+i;
		LB_PU[i].x=CurPuLoc_x-1;//固定
		LB_PU[i].y=CurPuLoc_y+i+neiPUnum;
	}

	//计算五方4x4PU的percentage
	LA_PU.percentage=ComputeModePercen(CurPuMode,0,0);
	for(int i=0;i<neiPUnum;i++)
	{
		int n=i*(16/neiPUnum);
		int N=(i+1)*(16/neiPUnum)-1;
		for(int j=n;j<=N;j++)
		{
			A_PU[i].percentage  += ComputeModePercen(CurPuMode,1,j);
			RA_PU[i].percentage += ComputeModePercen(CurPuMode,2,j);
			L_PU[i].percentage  += ComputeModePercen(CurPuMode,3,j);
			LB_PU[i].percentage += ComputeModePercen(CurPuMode,4,j);	
		}
	}

	Bool L_Valid = (L_PU[0].x	>=0) && (L_PU[0].x			<= (PicWid/4)-1) && (L_PU[0].y	>= 0) && (L_PU[0].y			<= (PicHei/4)-1);
	Bool A_Valid = (A_PU[0].x	>=0) && (A_PU[neiPUnum-1].x <= (PicWid/4)-1) && (A_PU[0].y	>= 0) && (A_PU[0].y			<= (PicHei/4)-1);
	Bool RA_Valid= (RA_PU[0].x	>=0) && (RA_PU[neiPUnum-1].x<= (PicWid/4)-1) && (RA_PU[0].y	>= 0) && (RA_PU[0].y			<= (PicHei/4)-1);
	Bool LB_Valid= (LB_PU[0].x	>=0) && (LB_PU[0].x			<= (PicWid/4)-1) && (LB_PU[0].y >= 0) && (LB_PU[neiPUnum-1].y<= (PicHei/4)-1);
	Bool LA_Valid= (LA_PU.x		>=0) && (LA_PU.x			<= (PicWid/4)-1) && (LA_PU.y	>= 0) && (LA_PU.y			<= (PicHei/4)-1);


	//计算all 4x4pu的true PU location和累计percen
/*	printf("CU(%d,%d)\n",PUTable[CurPuLoc_x][CurPuLoc_y].BelCU.CULocInPic.x,
				PUTable[CurPuLoc_x][CurPuLoc_y].BelCU.CULocInPic.y);	*/	
	/*-----------maggie----------
	L可用	CurPuLoc_x > 0  && CurPuLoc_x <= (PicWid/4)-1
	R可用	CurPuLoc_x >=0  && CurPuLoc_x <  (PicWid/4)-1
	A可用	CurPuLoc_y > 0  && CurPuLoc_y <= (PicHei/4)-1
	B可用	CurPuLoc_y >=0  && CurPuLoc_y <  (PicHei/4)-1

	---------------------------*/
	//Bool L_Valid=false;
	//Bool A_Valid=false;
	//Bool RA_Valid=false;
	//Bool LB_Valid=false;
	//Bool LA_Valid=false;

	//if( (LA_PU.x >=0) && (LA_PU.y >=0) )
	if(LA_Valid)
	{
		LA_PU.truPU_x=LA_PU.x-PUTable[LA_PU.x][LA_PU.y].InTruPU_x;
		LA_PU.truPU_y=LA_PU.y-PUTable[LA_PU.x][LA_PU.y].InTruPU_y;
		int truPUSize=PUTable[LA_PU.truPU_x][LA_PU.truPU_y].TruPUSize;
		PUTable[LA_PU.truPU_x][LA_PU.truPU_y].TruPU_SpatEFF += 	ComputePUSpaEff(LA_PU.percentage,CurPuSize,truPUSize);
	}

	//if(A_PU[0].y > -1)
	if(A_Valid)
	{
		for(int i=0;i<neiPUnum;i++)//neiPUnum是边上4xx的个数
		{
			A_PU[i].truPU_x=A_PU[i].x-PUTable[A_PU[i].x][A_PU[i].y].InTruPU_x;
			A_PU[i].truPU_y=A_PU[i].y-PUTable[A_PU[i].x][A_PU[i].y].InTruPU_y;
			int truPUSize=PUTable[A_PU[i].truPU_x][A_PU[i].truPU_y].TruPUSize;
			PUTable[A_PU[i].truPU_x][A_PU[i].truPU_y].TruPU_SpatEFF += ComputePUSpaEff(A_PU[i].percentage,CurPuSize,truPUSize);
		}
	}

	//if( (RA_PU[neiPUnum-1].y >-1) && (RA_PU[neiPUnum-1].x < (PicWid/4))  )//&& (RA_PU[i].x >0)
	if(RA_Valid)
	{
		for(int i=0;i<neiPUnum;i++)
		{
			RA_PU[i].truPU_x=RA_PU[i].x-PUTable[RA_PU[i].x][RA_PU[i].y].InTruPU_x;
			RA_PU[i].truPU_y=RA_PU[i].y-PUTable[RA_PU[i].x][RA_PU[i].y].InTruPU_y;
			int truPUSize=PUTable[RA_PU[i].truPU_x][RA_PU[i].truPU_y].TruPUSize;
			PUTable[RA_PU[i].truPU_x][RA_PU[i].truPU_y].TruPU_SpatEFF += ComputePUSpaEff(RA_PU[i].percentage,CurPuSize,truPUSize);
		}
	}

	//if(L_PU[0].x > -1)
	if(L_Valid)
	{
		for(int i=0;i<neiPUnum;i++)
		{
			L_PU[i].truPU_x=L_PU[i].x-PUTable[L_PU[i].x][L_PU[i].y].InTruPU_x;;
			L_PU[i].truPU_y=L_PU[i].y-PUTable[L_PU[i].x][L_PU[i].y].InTruPU_y;
			int truPUSize=PUTable[L_PU[i].truPU_x][L_PU[i].truPU_y].TruPUSize;
			PUTable[L_PU[i].truPU_x][L_PU[i].truPU_y].TruPU_SpatEFF += ComputePUSpaEff(L_PU[i].percentage,CurPuSize,truPUSize);
		}
	}
	
	//if( (LB_PU[neiPUnum-1].x >-1) && (LB_PU[neiPUnum-1].y < (PicHei/4))  )//&& (LB_PU[i].y >0)
	if(LB_Valid)
	{
		for(int i=0;i<neiPUnum;i++)
		{
			LB_PU[i].truPU_x=LB_PU[i].x-PUTable[LB_PU[i].x][LB_PU[i].y].InTruPU_x;;
			LB_PU[i].truPU_y=LB_PU[i].y-PUTable[LB_PU[i].x][LB_PU[i].y].InTruPU_y;
			int truPUSize=PUTable[LB_PU[i].truPU_x][LB_PU[i].truPU_y].TruPUSize;
			PUTable[LB_PU[i].truPU_x][LB_PU[i].truPU_y].TruPU_SpatEFF += ComputePUSpaEff(LB_PU[i].percentage,CurPuSize,truPUSize);

		}
	}
	delete []A_PU;
	A_PU=NULL;
	delete []RA_PU;
	RA_PU=NULL;
	delete []L_PU;
	L_PU=NULL;
	delete []LB_PU;
	LB_PU=NULL;
}

void NALInfo::ComInterPUNeiPUSpaEff(int CurPuLoc_x,int CurPuLoc_y,int PUw,int PUh,int PUFormDir,int PUBackDir)
{
	TableU *NeiPU =new TableU[5];
	Bool Dir[5];
	for(int i=0;i<5;i++)
	{
		NeiPU[i].init();
		Dir[i]=false;
	}

	//定位邻居5个4x4sizePu
	NeiPU[0].x=CurPuLoc_x-1;
	NeiPU[0].y=CurPuLoc_y+(PUh/4)-1;

	NeiPU[1].x=CurPuLoc_x+(PUw/4)-1;
	NeiPU[1].y=CurPuLoc_y-1;

	NeiPU[2].x=CurPuLoc_x+(PUw/4);
	NeiPU[2].y=CurPuLoc_y-1;

	NeiPU[3].x=CurPuLoc_x-1;
	NeiPU[3].y=CurPuLoc_y+(PUh/4);

	NeiPU[4].x=CurPuLoc_x-1;	//左上4x4PU 在table中坐标
	NeiPU[4].y=CurPuLoc_y-1;

	Bool L_Valid= (NeiPU[0].x >=0 ) && (NeiPU[0].x <= (PicWid/4)-1) && (NeiPU[0].y >= 0) &&(NeiPU[0].y <= (PicHei/4)-1);
	Bool A_Valid= (NeiPU[1].x >=0 ) && (NeiPU[1].x <= (PicWid/4)-1) && (NeiPU[1].y >= 0) &&(NeiPU[1].y <= (PicHei/4)-1);
	Bool RA_Valid=(NeiPU[2].x >=0)  && (NeiPU[2].x <= (PicWid/4)-1) && (NeiPU[2].y >= 0) &&(NeiPU[2].y <= (PicHei/4)-1);
	Bool LB_Valid=(NeiPU[3].x >=0 ) && (NeiPU[3].x <= (PicWid/4)-1) && (NeiPU[3].y >= 0) &&(NeiPU[3].y <= (PicHei/4)-1);
	Bool LA_Valid=(NeiPU[4].x >=0 ) && (NeiPU[4].x <= (PicWid/4)-1) && (NeiPU[4].y >= 0) &&(NeiPU[4].y <= (PicHei/4)-1);

	if(PUFormDir >=0 && PUFormDir<5)//说明当前PU的前向MVP来自空间预测
	{
		switch (PUFormDir)
		{
			case 0:		NeiPU[0].percentage +=0.5;break;
			case 1:		NeiPU[1].percentage +=0.5;break;
			case 2:		NeiPU[2].percentage +=0.5;break;
			case 3:		NeiPU[3].percentage +=0.5;break;
			case 4:		NeiPU[4].percentage +=0.5;break;
			default:	break;
		}
		if(PUBackDir>=0 && PUBackDir<5)//说明当前PU的后向MVP来自空间预测
		{
			switch (PUBackDir)
			{
				case 0:		NeiPU[0].percentage +=0.5;break;
				case 1:		NeiPU[1].percentage +=0.5;break;
				case 2:		NeiPU[2].percentage +=0.5;break;
				case 3:		NeiPU[3].percentage +=0.5;break;
				case 4:		NeiPU[4].percentage +=0.5;break;
				default:	break;
			}
		}
	}

	//计算4x4pu的true PU location
	//if(NeiPU[0].x >=0 && NeiPU[0].y>=0 &&  NeiPU[0].y<(PicHei/4) )//L可用
	if(L_Valid)
	{
		Dir[0]=true;
		NeiPU[0].truPU_x=NeiPU[0].x-PUTable[NeiPU[0].x][NeiPU[0].y].InTruPU_x;
		NeiPU[0].truPU_y=NeiPU[0].y-PUTable[NeiPU[0].x][NeiPU[0].y].InTruPU_y;	

/*		L_PU.truPU_x=L_PU.x-PUTable[L_PU.x][L_PU.y].InTruPU_x;
		L_PU.truPU_y=L_PU.y-PUTable[L_PU.x][L_PU.y].InTruPU_y;*/	
/*		printf("CU(%d,%d) L_PU(%d,%d)\n",
			PUTable[NeiPU[0].truPU_x][NeiPU[0].truPU_y].BelCU.CULocInPic.x,
			PUTable[NeiPU[0].truPU_x][NeiPU[0].truPU_y].BelCU.CULocInPic.y,		
			PUTable[NeiPU[0].truPU_x][NeiPU[0].truPU_y].PUInPicLoc_x,
			PUTable[NeiPU[0].truPU_x][NeiPU[0].truPU_y].PUInPicLoc_y);*/		
	}
	if(A_Valid)
	{
		Dir[1]=true;
		NeiPU[1].truPU_x=NeiPU[1].x-PUTable[NeiPU[1].x][NeiPU[1].y].InTruPU_x;
		NeiPU[1].truPU_y=NeiPU[1].y-PUTable[NeiPU[1].x][NeiPU[1].y].InTruPU_y;	
		//A_PU.truPU_x=A_PU.x-PUTable[A_PU.x][A_PU.y].InTruPU_x;
		//A_PU.truPU_y=A_PU.y-PUTable[A_PU.x][A_PU.y].InTruPU_y;	
		//printf("CU(%d,%d) A_PU(%d,%d)\n",
		//	PUTable[NeiPU[1].truPU_x][NeiPU[1].truPU_y].BelCU.CULocInPic.x,
		//	PUTable[NeiPU[1].truPU_x][NeiPU[1].truPU_y].BelCU.CULocInPic.y,		
		//	PUTable[NeiPU[1].truPU_x][NeiPU[1].truPU_y].PUInPicLoc_x,
		//	PUTable[NeiPU[1].truPU_x][NeiPU[1].truPU_y].PUInPicLoc_y);	
	}
	if(RA_Valid)
	{
		Dir[2]=true;
		NeiPU[2].truPU_x=NeiPU[2].x-PUTable[NeiPU[2].x][NeiPU[2].y].InTruPU_x;
		NeiPU[2].truPU_y=NeiPU[2].y-PUTable[NeiPU[2].x][NeiPU[2].y].InTruPU_y;	

		//RA_PU.truPU_x=RA_PU.x-PUTable[RA_PU.x][RA_PU.y].InTruPU_x;
		//RA_PU.truPU_y=RA_PU.y-PUTable[RA_PU.x][RA_PU.y].InTruPU_y;	
		//printf("CU(%d,%d) RA_PU(%d,%d)\n",
		//	PUTable[RA_PU.truPU_x][RA_PU.truPU_y].BelCU.CULocInPic.x,
		//	PUTable[RA_PU.truPU_x][RA_PU.truPU_y].BelCU.CULocInPic.y,
		//	PUTable[RA_PU.truPU_x][RA_PU.truPU_y].PUInPicLoc_x,
		//	PUTable[RA_PU.truPU_x][RA_PU.truPU_y].PUInPicLoc_y);
	}
	if(LB_Valid)
	{
		Dir[3]=true;
		NeiPU[3].truPU_x=NeiPU[3].x-PUTable[NeiPU[3].x][NeiPU[3].y].InTruPU_x;
		NeiPU[3].truPU_y=NeiPU[3].y-PUTable[NeiPU[3].x][NeiPU[3].y].InTruPU_y;

		//LB_PU.truPU_x=LB_PU.x-PUTable[LB_PU.x][LB_PU.y].InTruPU_x;
		//LB_PU.truPU_y=LB_PU.y-PUTable[LB_PU.x][LB_PU.y].InTruPU_y;	
		//printf("CU(%d,%d) LB_PU(%d,%d)\n",
		//	PUTable[LB_PU.truPU_x][LB_PU.truPU_y].BelCU.CULocInPic.x,
		//	PUTable[LB_PU.truPU_x][LB_PU.truPU_y].BelCU.CULocInPic.y,
		//	PUTable[LB_PU.truPU_x][LB_PU.truPU_y].PUInPicLoc_x,
		//	PUTable[LB_PU.truPU_x][LB_PU.truPU_y].PUInPicLoc_y);
	}
	if(LA_Valid)
	{
		Dir[4]=true;
		NeiPU[4].truPU_x=NeiPU[4].x-PUTable[NeiPU[4].x][NeiPU[4].y].InTruPU_x;
		NeiPU[4].truPU_y=NeiPU[4].y-PUTable[NeiPU[4].x][NeiPU[4].y].InTruPU_y;

		//LA_PU.truPU_x=LA_PU.x-PUTable[LA_PU.x][LA_PU.y].InTruPU_x;
		//LA_PU.truPU_y=LA_PU.y-PUTable[LA_PU.x][LA_PU.y].InTruPU_y;	
		//printf("CU(%d,%d) LA_PU(%d,%d)\n",
		//	PUTable[LA_PU.truPU_x][LA_PU.truPU_y].BelCU.CULocInPic.x,
		//	PUTable[LA_PU.truPU_x][LA_PU.truPU_y].BelCU.CULocInPic.y,	
		//	PUTable[LA_PU.truPU_x][LA_PU.truPU_y].PUInPicLoc_x,
		//	PUTable[LA_PU.truPU_x][LA_PU.truPU_y].PUInPicLoc_y);
	}
	//printf("\n");
	//计算nei PU的 Spat eff
	for(int i=0;i<5;i++)
	{
		if(Dir[i])
		{
			if(PUTable[CurPuLoc_x][CurPuLoc_y].TruInterPU_SpatEff == 0)
			{
				//PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruInterPU_SpatEff += NeiPU[i].percentage;
				int truPUarea=PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruPUh*PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruPUw;
				int curPUarea=PUh*PUw;
				PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruInterPU_SpatEff += ComputePUSpaEff(NeiPU[i].percentage,curPUarea,truPUarea);
			
			}
			else if(PUTable[CurPuLoc_x][CurPuLoc_y].TruInterPU_SpatEff > 0)
			{
				PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruInterPU_SpatEff += NeiPU[i].percentage*PUTable[CurPuLoc_x][CurPuLoc_y].TruInterPU_SpatEff;
				int truPUarea=PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruPUh*PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruPUw;
				int curPUarea=PUh*PUw;
				
				PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruInterPU_SpatEff += ComputePUSpaEff(NeiPU[i].percentage,curPUarea,truPUarea)*PUTable[CurPuLoc_x][CurPuLoc_y].TruInterPU_SpatEff;
			}
			//printf("Dir[%d] PU(%d,%d) percen=%f Spaeff=%f\n",i,
			//	PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].PUInPicLoc_x,
			//	PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].PUInPicLoc_y,
			//	NeiPU[i].percentage,
			//	PUTable[NeiPU[i].truPU_x][NeiPU[i].truPU_y].TruInterPU_SpatEff);
		}

	}

	delete []NeiPU;
	NeiPU=NULL;
}

void NALInfo::ComputeCUSpatEff()
{
	int ctu_h=(int)ceil((PicHei*1.0)/64);
	int ctu_w=(int)ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;
	int PUnumInCU=0;
	for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	{
		for(int j=ctu_w-1;j>=0;j--)
		{
			int curctusize_x=j;
			int curctusize_y=i;

			float CUEff=0;
			for(int k=3;k>=0;k--)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;			
						for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
							if(valid_xy)
							{
								Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
								if(FirPuInTruPU)
								{	
									/*printf("CU(%d,%d)-PU(%d,%d)eff=%f\n",
										PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x,
										PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y,
										PUTable[PUTable_x][PUTable_y].PUInPicLoc_x,
										PUTable[PUTable_x][PUTable_y].PUInPicLoc_y,
										PUTable[PUTable_x][PUTable_y].TruPU_SpatEFF+PUTable[PUTable_x][PUTable_y].TruInterPU_SpatEff);
*/
									CUEff += PUTable[PUTable_x][PUTable_y].TruPU_SpatEFF+PUTable[PUTable_x][PUTable_y].TruInterPU_SpatEff;
									PUnumInCU++;
									Bool FirPuInCU=(PUTable_x == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x/4))  && (PUTable_y == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y/4));				
									if(FirPuInCU)//firstPU in 8x8CU
									{	
										//PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff=CUEff/PUnumInCU;
										PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff=CUEff;
										PUnumInCU=0;
										CUEff=0;
										CUnum++;
									}

								}									
							}			
						}
					}
				}
			}
		}
	}
	//printf("CU number=%d\n",CUnum);
}

//选择策略
CUInfo *insert_CUsort(CUInfo array[],int len)
{
	int i,j;
	CUInfo key;
	for(i=1;i<len;i++)
	{
		key=array[i];
		for(j=i-1;j>=0;j-- )
		{
			if(array[j].CUSpaEff<key.CUSpaEff)
			{
				array[j+1]=array[j];
				array[j]=key;	
			}
		}
	}
	return array;
}

void NALInfo::SortCUEff()
{
	CUArray= new CUInfo[CUnum];

	//-----------zigzag倒序-------------------
	int ctu_h=(int)ceil((PicHei*1.0)/64);
	int ctu_w=(int)ceil((PicWid*1.0)/64);
	int table_h=PicHei/4;
	int table_w=PicWid/4;
	int CUindex=CUnum;

	for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	{
		for(int j=ctu_w-1;j>=0;j--)
		{
			int curctusize_x=j;
			int curctusize_y=i;

			float CUEff=0;
			for(int k=3;k>=0;k--)//CTU中32x32分区扫描
			{
				int cur32size_x=k%2;
				int cur32size_y=k/2;
				for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
				{
					int cur16size_x=l%2;
					int cur16size_y=l/2;
					for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
					{
						int cur8size_x=m%2;
						int cur8size_y=m/2;			
						for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
						{
							int cur4size_x=n%2; //x=0,1,0,1 
							int cur4size_y=n/2;	//y=0,0,1,1
							int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
							int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
							Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
							if(valid_xy)
							{
								Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
								if(FirPuInTruPU)
								{	
									CUEff += PUTable[PUTable_x][PUTable_y].TruPU_SpatEFF;
									Bool FirPuInCU=(PUTable_x == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x/4))  && (PUTable_y == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y/4));				
									if(FirPuInCU)//firstPU in 8x8CU
									{			
										int n=CUnum-CUindex;
										if(n<CUnum)
										{
											CUArray[n]=PUTable[PUTable_x][PUTable_y].BelCU;
											//CUArray[n].CUSpaEff=PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff;
											//CUArray[n].CUOffsetInfo.InFileOffsset=PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset;
											CUindex--;
											//printf("CU[%d]eff=%f\n",n,CUArray[n].CUSpaEff);		
										}
										
									}
								}									
							}			
						}
					}
				}
			}
		}
	}

	//SortedCU= new CUInfo[CUnum];
	SortedCU=insert_CUsort(CUArray,CUnum);
	//printf("CUeff从大到小：\n");
	//for(int i=0;i<CUnum;i++)
	//{
	//	printf("[%5d]-CU(%4d,%4d)eff=%f\n",i,SortedCU[i].CULocInPic.x,SortedCU[i].CULocInPic.y,SortedCU[i].CUSpaEff);
	//}
	//输出一个文件
	//fstream *Sortfile;
	//FILE *fp;
	//fp=fopen("E:\HEVC\HM-16.18\bin\vc2012\Win32\Debug\CUlabel.txt","W+");
	//fprintf(fp, "1帧的BlockWei表\n");
	//for(int i=ctu_h-1;i>=0;i--)//外层CTU是光栅扫描
	//{
	//	for(int j=ctu_w-1;j>=0;j--)
	//	{
	//		int curctusize_x=j;
	//		int curctusize_y=i;

	//		float CUEff=0;
	//		for(int k=3;k>=0;k--)//CTU中32x32分区扫描
	//		{
	//			int cur32size_x=k%2;
	//			int cur32size_y=k/2;
	//			for(int l=3;l>=0;l--)//32X32分区中，16x16分区扫描
	//			{
	//				int cur16size_x=l%2;
	//				int cur16size_y=l/2;
	//				for(int m=3;m>=0;m--)//16X16分区中，8x8分区扫描
	//				{
	//					int cur8size_x=m%2;
	//					int cur8size_y=m/2;			
	//					for(int n=3;n>=0;n--)//8X8分区中，4x4PU扫描
	//					{
	//						int cur4size_x=n%2; //x=0,1,0,1 
	//						int cur4size_y=n/2;	//y=0,0,1,1
	//						int PUTable_x=1*cur4size_x+2*cur8size_x+4*cur16size_x+8*cur32size_x+16*curctusize_x;
	//						int PUTable_y=1*cur4size_y+2*cur8size_y+4*cur16size_y+8*cur32size_y+16*curctusize_y;
	//						Bool valid_xy=((PUTable_x < table_w) && (PUTable_y < table_h));
	//						if(valid_xy)
	//						{
	//							Bool FirPuInTruPU=PUTable[PUTable_x][PUTable_y].InTruPU_x==0 && PUTable[PUTable_x][PUTable_y].InTruPU_y==0;
	//							if(FirPuInTruPU)
	//							{	
	//								//CUEff += PUTable[PUTable_x][PUTable_y].TruPU_SpatEFF;
	//								Bool FirPuInCU=(PUTable_x == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x/4))  && (PUTable_y == (PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y/4));				
	//								if(FirPuInCU)//firstPU in 8x8CU
	//								{			
	//									int n=CUnum-CUindex;
	//									if(n<CUnum)
	//									{
	//										//CUArray[n]=PUTable[PUTable_x][PUTable_y].BelCU;
	//										//CUArray[n].CUSpaEff=PUTable[PUTable_x][PUTable_y].BelCU.CUSpaEff;
	//										//CUArray[n].CUOffsetInfo.InFileOffsset=PUTable[PUTable_x][PUTable_y].BelCU.CUOffsetInfo.InFileOffsset;
	//										fprintf(fp, "%4d ",PUTable[PUTable_x][PUTable_y].BelCU.Bsign);
	//										CUindex--;
	//										//printf("CU[%d]eff=%f\n",n,CUArray[n].CUSpaEff);		
	//									}
	//									
	//								}
	//							}									
	//						}			
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

void NALInfo::PickEncU(BitExtractUnit *ExtractU,int ExtractedCUNum)
{
	int i=0;
	int n=0;//排名下标
	//前排名
	while(i<ExtractedCUNum)
	{
		if(SortedCU[n].CostBits.Totalcostbits>0)
		{
				//	printf("sortCU[%d]:\nIPCM off=%d,len=%d\nLumaMode off=%d,len=%d\nMergeIndex off=%d,len=%d\nSkipFlag off=%d,len=%d\nInterDir off=%d,len=%d\nRefIdx off=%d,len=%d\nMVD off=%d,len=%d\nAMVP off=%d,len=%d\nQTC off=%d,len=%d\n",
				//CUnum-1-n,
				//SortedCU[n].IPCM.InFileOffsset,
				//SortedCU[n].IPCMLen,
				//SortedCU[n].LumaDirMode.InFileOffsset,
				//SortedCU[n].LumaDirModeLen,
				//SortedCU[n].MergeIdx.InFileOffsset,
				//SortedCU[n].MergeIdxLen,
				//SortedCU[n].SkipFlag.InFileOffsset,
				//SortedCU[n].SkipFlagLen,
				//SortedCU[n].InterDir.InFileOffsset,
				//SortedCU[n].InterDirLen,
				//SortedCU[n].RefIdx.InFileOffsset,
				//SortedCU[n].RefIdxLen,
				//SortedCU[n].MVD.InFileOffsset,
				//SortedCU[n].MVDLen,
				//SortedCU[n].AMVPIdx.InFileOffsset,
				//SortedCU[n].AMVPIdxLen,
				//SortedCU[n].Coefficient.InFileOffsset,
				//SortedCU[n].CoefficientLen);
			ExtractU[i*9+0].offset=SortedCU[n].IPCM.InFileOffsset;
			ExtractU[i*9+0].len=SortedCU[n].IPCMLen;
			ExtractU[i*9+1].offset=SortedCU[n].LumaDirMode.InFileOffsset;
			ExtractU[i*9+1].len=SortedCU[n].LumaDirModeLen;
			ExtractU[i*9+2].offset=SortedCU[n].MergeIdx.InFileOffsset;
			ExtractU[i*9+2].len=SortedCU[n].MergeIdxLen;
			ExtractU[i*9+3].offset=SortedCU[n].SkipFlag.InFileOffsset;
			ExtractU[i*9+3].len=SortedCU[n].SkipFlagLen;
			ExtractU[i*9+4].offset=SortedCU[n].InterDir.InFileOffsset;
			ExtractU[i*9+4].len=SortedCU[n].InterDirLen;
			ExtractU[i*9+5].offset=SortedCU[n].RefIdx.InFileOffsset;
			ExtractU[i*9+5].len=SortedCU[n].RefIdxLen;
			ExtractU[i*9+6].offset=SortedCU[n].MVD.InFileOffsset;
			ExtractU[i*9+6].len=SortedCU[n].MVDLen;
			ExtractU[i*9+7].offset=SortedCU[n].AMVPIdx.InFileOffsset;
			ExtractU[i*9+7].len=SortedCU[n].AMVPIdxLen;
			ExtractU[i*9+8].offset=SortedCU[n].Coefficient.InFileOffsset;
			ExtractU[i*9+8].len=SortedCU[n].CoefficientLen;	

			i++;
		}
		else//跳过这个CU选下一个
		{
		
		}
		n++;
	}

	////后排名
	//while(i<ExtractedCUNum)
	//{
	//	if(SortedCU[CUnum-1-n].CostBits.Totalcostbits>0)
	//	{
	//		ExtractU[i*9+0].offset=SortedCU[CUnum-1-n].IPCM.InFileOffsset;
	//		ExtractU[i*9+0].len=SortedCU[CUnum-1-n].IPCMLen;
	//		ExtractU[i*9+1].offset=SortedCU[CUnum-1-n].LumaDirMode.InFileOffsset;
	//		ExtractU[i*9+1].len=SortedCU[CUnum-1-n].LumaDirModeLen;
	//		ExtractU[i*9+2].offset=SortedCU[CUnum-1-n].MergeIdx.InFileOffsset;
	//		ExtractU[i*9+2].len=SortedCU[CUnum-1-n].MergeIdxLen;
	//		ExtractU[i*9+3].offset=SortedCU[CUnum-1-n].SkipFlag.InFileOffsset;
	//		ExtractU[i*9+3].len=SortedCU[CUnum-1-n].SkipFlagLen;
	//		ExtractU[i*9+4].offset=SortedCU[CUnum-1-n].InterDir.InFileOffsset;
	//		ExtractU[i*9+4].len=SortedCU[CUnum-1-n].InterDirLen;
	//		ExtractU[i*9+5].offset=SortedCU[CUnum-1-n].RefIdx.InFileOffsset;
	//		ExtractU[i*9+5].len=SortedCU[CUnum-1-n].RefIdxLen;
	//		ExtractU[i*9+6].offset=SortedCU[CUnum-1-n].MVD.InFileOffsset;
	//		ExtractU[i*9+6].len=SortedCU[CUnum-1-n].MVDLen;
	//		ExtractU[i*9+7].offset=SortedCU[CUnum-1-n].AMVPIdx.InFileOffsset;
	//		ExtractU[i*9+7].len=SortedCU[CUnum-1-n].AMVPIdxLen;
	//		ExtractU[i*9+8].offset=SortedCU[CUnum-1-n].Coefficient.InFileOffsset;
	//		ExtractU[i*9+8].len=SortedCU[CUnum-1-n].CoefficientLen;
	//		i++;
	//	}
	//	else//跳过这个CU选下一个
	//	{
	//	
	//	}
	//	n++;
	//}
					//printf("sortCU[%d]:\nIPCM off=%d,len=%d\nLumaMode off=%d,len=%d\nMergeIndex off=%d,len=%d\nSkipFlag off=%d,len=%d\nInterDir off=%d,len=%d\nRefIdx off=%d,len=%d\nMVD off=%d,len=%d\nAMVP off=%d,len=%d\nQTC off=%d,len=%d\n",
				//CUnum-1-n,
				//SortedCU[CUnum-1-n].IPCM.InFileOffsset,
				//SortedCU[CUnum-1-n].IPCMLen,
				//SortedCU[CUnum-1-n].LumaDirMode.InFileOffsset,
				//SortedCU[CUnum-1-n].LumaDirModeLen,
				//SortedCU[CUnum-1-n].MergeIdx.InFileOffsset,
				//SortedCU[CUnum-1-n].MergeIdxLen,
				//SortedCU[CUnum-1-n].SkipFlag.InFileOffsset,
				//SortedCU[CUnum-1-n].SkipFlagLen,
				//SortedCU[CUnum-1-n].InterDir.InFileOffsset,
				//SortedCU[CUnum-1-n].InterDirLen,
				//SortedCU[CUnum-1-n].RefIdx.InFileOffsset,
				//SortedCU[CUnum-1-n].RefIdxLen,
				//SortedCU[CUnum-1-n].MVD.InFileOffsset,
				//SortedCU[CUnum-1-n].MVDLen,
				//SortedCU[CUnum-1-n].AMVPIdx.InFileOffsset,
				//SortedCU[CUnum-1-n].AMVPIdxLen,
				//SortedCU[CUnum-1-n].Coefficient.InFileOffsset,
				//SortedCU[CUnum-1-n].CoefficientLen);
}

//加密执行
void NALInfo::EncSlice(string inpath)
{
	if(CUnum>0)
	{
		 float x=0.005;
		 int ExcCUNum=x*CUnum;
		 int ExtractedUNum=ExcCUNum*9;//这种定义是错的，U只等价于语法元素单元，一个CU最多有9个U
		 BitExtractUnit *ExtractU= new BitExtractUnit[ExtractedUNum];
		 memset(ExtractU,0x00,ExtractedUNum);
		 PickEncU(ExtractU,ExcCUNum);
		 keystr="1234567812345678";//128bit密钥
		 EncryptPlain(inpath,ExtractU,ExtractedUNum);//加密单个CU,不能播
		 delete []ExtractU;
		 ExtractU=NULL;
		 delete []CUArray;
		 CUArray=NULL;
		 //delete []SortedCU;
		// SortedCU=NULL;
	}
}

void SetBitValue(char & szTemp,const int nPos,const int nValue)
{
	ASSERT( nPos<8 && nPos >= 0);	// 下标位只能是0-7
	if( nValue == 1)
		szTemp |= 1UL << nPos;		// 将nPos的bit位设置为1，其他位不变
	else if(nValue == 0)
		szTemp &= ~(1UL<<nPos);		// 将nPos的bit位设置为0，其他位不变 
	else
		ASSERT(FALSE);				// 否则报错！
}

void NALInfo::EncryptPlain(string PlainPath,BitExtractUnit *ExtractBuf,int ExtractedCUNum)
{
	string inputpath=PlainPath;
	fstream plainfile;
	plainfile.open(inputpath.c_str(),ios::in|ios::out|ios::binary);

	BitExtractUnit *ExtractUnit = new BitExtractUnit[ExtractedCUNum];
	for(int i=0;i<ExtractedCUNum;i++)
	{
		ExtractUnit[i].offset=ExtractBuf[i].offset;
		ExtractUnit[i].len=ExtractBuf[i].len;
	}

	for(int i=0;i<ExtractedCUNum;i++)//实际是加密一个CU中的一个语法元素bits，即一个extunit 
	{
		int bitlen=ExtractUnit[i].len;
		if(bitlen>0)
		{
			long startoffset=ExtractUnit[i].offset;
			int ShouldReadByteNum=0;
			long endoffset=startoffset+bitlen;
			long start_byte_offset=startoffset/8;
			int start_bit_offset=startoffset%8;	////值域0-7
			long end_byte_offset=endoffset/8;
			int end_bit_offset=endoffset%8;	//值域0-7

			if(end_bit_offset == 0)
			{
				//ShouldReadByteNum=bitlen/8;
				ShouldReadByteNum=end_byte_offset-start_byte_offset;
			}
			else if(end_bit_offset >0 && end_bit_offset < 8)
			{
				//ShouldReadByteNum=bitlen/8 + 1;
				ShouldReadByteNum=end_byte_offset-start_byte_offset+1;
			}

			unsigned char *iv=new unsigned char[16]; //初始向量为16字节,为每一个语法元素分配
			RAND_bytes(iv, 16);
	
			//----------------------加密-----------------
			 char *PlainString= new  char[ShouldReadByteNum];//提取一个语法元素bits包括的所有字节
			memset(PlainString,0x00,ShouldReadByteNum);
			 char *CipherString= new  char[ShouldReadByteNum];
			memset(CipherString,0x00,ShouldReadByteNum);
			//读入文件
			plainfile.seekg(start_byte_offset);
			plainfile.read(PlainString,ShouldReadByteNum);

				//打印
				//printf("Clean text:\n");
				//for(int i=0;i<ShouldReadByteNum;i++)
				//{
				//	printf("%d", PlainString[i]);
				//}
				//printf("\n");
			EncryptOnce(ShouldReadByteNum,PlainString,CipherString,start_bit_offset,end_bit_offset,iv);
				//打印
				//printf("cipher text:\n");
				////printf("cipher text: %s\n", CipherString);
				//for(int i=0;i<ShouldReadByteNum;i++)
				//{
				//	printf("%X", CipherString[i]);
				//}
				//printf("\n");

			//写入文件
			plainfile.seekp(start_byte_offset);
			plainfile.write(CipherString,ShouldReadByteNum);
			plainfile.flush();

			delete []PlainString;
			PlainString=NULL;
			delete []CipherString;
			CipherString=NULL;	
			//--------------------------------------------

			////---------------解密--------------------
			// char *RecCipherString= new  char[ShouldReadByteNum];
			//memset(RecCipherString,0x00,ShouldReadByteNum);
			// char *DecString= new  char[ShouldReadByteNum];//提取一个语法元素bits包括的所有字节
			//memset(DecString,0x00,ShouldReadByteNum);
			//plainfile.seekg(start_byte_offset);
			//plainfile.read(RecCipherString,ShouldReadByteNum);
		
			//	////打印
			//	//printf("cipher text:\n");
			//	////printf("cipher text: %s\n", RecCipherString);
			//	//for(int i=0;i<ShouldReadByteNum;i++)
			//	//{
			//	//	printf("%X", RecCipherString[i]);
			//	//}
			//	//printf("\n");		
		
			//DecryptOnce(ShouldReadByteNum,RecCipherString,DecString,start_bit_offset,end_bit_offset,iv);
		
			////打印
			////printf("Decrypted text:\n");
			//////printf("Decrypted text: %s\n", DecString);
			////for(int i=0;i<ShouldReadByteNum;i++)
			////{
			////	printf("%d", DecString[i]);
			////}
			////printf("\n");

			//plainfile.seekp(start_byte_offset);
			//plainfile.write(DecString,ShouldReadByteNum);
			//plainfile.flush();
		
			//delete []RecCipherString;
			//RecCipherString=NULL;	
			//delete []DecString;
			//DecString=NULL;	
			////---------------------------------------
			//delete []iv;
			//iv=NULL;			
		}
	}
	plainfile.close();
	delete []ExtractUnit;
	ExtractUnit=NULL;
}

void BytesEncrypt(unsigned char *text,unsigned char *outdata,unsigned const char * slice_key,int len,unsigned char *iv)
{
	//设置加密密钥
	AES_KEY Ekey;
	ctr_state Estate;
	AES_set_encrypt_key(slice_key, 128, &Ekey);//128指的是密钥比特长度
    Estate.num = 0;
    memset(Estate.ecount, 0, 16); 
    memcpy(Estate.ivec, iv, 16); 
	AES_set_encrypt_key(slice_key, 128, &Ekey);
    AES_ctr128_encrypt(text, outdata, len, &Ekey, Estate.ivec, Estate.ecount, &Estate.num);
}

void BytesDecrypt(unsigned char *outdata,unsigned char *decdata,unsigned const char * slice_key,int len, unsigned char *iv)
{
	//解密
	//unsigned char * decdata= new unsigned char[len];
	AES_KEY Dkey;
	ctr_state Dstate;
	AES_set_encrypt_key(slice_key, 128, &Dkey);
	Dstate.num = 0;
	memset(Dstate.ecount, 0, 16); 
	memcpy(Dstate.ivec, iv, 16); 
	AES_set_encrypt_key(slice_key, 128, &Dkey);
	AES_ctr128_encrypt(outdata, decdata, len, &Dkey, Dstate.ivec, Dstate.ecount, &Dstate.num);
}

void NALInfo::EncryptOnce(int ReadByteNum, char * PlainString, char *CipherString,int start_bit,int end_bit,unsigned char *ivtrans)
{
	int ShouldReadByteNum=ReadByteNum;//一个语法元素包含的字节数
	int start_bit_offset=start_bit;
	int end_bit_offset=end_bit;

	if(ShouldReadByteNum == 1)//只有plainstring[0]plainstring存放语法元素字节数组
	{
		bitset<8>	PlainStringBit_s(PlainString[0]);
		if(end_bit_offset == 0)
		{
			for(int i=7-start_bit_offset;i>=0;i--)
			{
				SetBitValue(PlainString[0],i,~PlainStringBit_s[i]);
			}
		}
		else if( end_bit_offset > 0 && end_bit_offset <8)
		{
			for(int i=7-start_bit_offset;i>=7-end_bit_offset;i--)//Debug试一下
			{
				SetBitValue(PlainString[0],i,~PlainStringBit_s[i]);
			}
		}
		CipherString[0]=PlainString[0];
	}
	else if(ShouldReadByteNum > 1)
	{
		//对于startByte
		bitset<8>	PlainStringBit_s(PlainString[0]);
		if(start_bit_offset == 0)
		{
			for(int i=7-start_bit_offset;i>=0;i--)
			{
				SetBitValue(PlainString[0],i,~PlainStringBit_s[i]);
			}
			CipherString[0]=PlainString[0];
		}
		else if (start_bit_offset >0 && start_bit_offset <8)
		{
			//从7-startbitoffset开始递减按位取反，相当于从startbitoffset开始递增按位取反
			for(int i=7-start_bit_offset;i>=0;i--)
			{
				SetBitValue(PlainString[0],i,~PlainStringBit_s[i]);
			}
			CipherString[0]=PlainString[0];
		}

		//对于lastByte
		bitset<8>	PlainStringBit_e(PlainString[ShouldReadByteNum-1]);
		if(end_bit_offset == 0)
		{
			for(int i=7;i > 7-start_bit_offset;i--)
			{
				SetBitValue(PlainString[ShouldReadByteNum-1],i,~PlainStringBit_e[i]);
			}
			CipherString[ShouldReadByteNum-1]=PlainString[ShouldReadByteNum-1];
		}
		else if (end_bit_offset >0 && end_bit_offset <8)
		{
			//从7开始直到7-startbitoffset开始递减按位取反
			for(int i=7;i > 7-start_bit_offset;i--)
			{
				SetBitValue(PlainString[ShouldReadByteNum-1],i,~PlainStringBit_e[i]);
			}
			CipherString[ShouldReadByteNum-1]=PlainString[ShouldReadByteNum-1];
		}

		if(ShouldReadByteNum > 2)
		{

			int AESplainlen=ShouldReadByteNum-2;
			unsigned char *AESplain= new unsigned char[AESplainlen];
			memset(AESplain,0,AESplainlen);
			for(int j=0;j<AESplainlen;j++)
			{ 
				AESplain[j]=PlainString[j+1];
			}
			//string keystr="1234567812345678";
			unsigned const char * slice_key = (unsigned const char *) keystr.data(); 
			unsigned char *AEScipher= new unsigned char[AESplainlen];
			memset(AEScipher,0,AESplainlen);
			unsigned char *iv=new unsigned char[16]; //初始向量为16字节,由解密端传递
			memcpy(iv, ivtrans, 16); 

			BytesEncrypt(AESplain,AEScipher,slice_key,AESplainlen,iv);
			
			for(int j=0;j<AESplainlen;j++)
			{ 
				CipherString[j+1]=AEScipher[j];
			}

			delete []AESplain;
			AESplain=NULL;
			delete []AEScipher;
			AEScipher=NULL;
			delete []iv;
			iv=NULL;
		}	
	}

}

void NALInfo::DecryptOnce(int ReadByteNum, char * CipherString, char *DecString,int start_bit,int end_bit,unsigned char *ivtrans)
{
	int ShouldReadByteNum=ReadByteNum;//一个语法元素包含的字节数
	int start_bit_offset=start_bit;
	int end_bit_offset=end_bit;

	if(ShouldReadByteNum == 1)//只有plainstring[0]plainstring存放语法元素字节数组
	{
		bitset<8>	PlainStringBit_s(CipherString[0]);
		if(end_bit_offset == 0)
		{
			for(int i=7-start_bit_offset;i>=0;i--)
			{

				SetBitValue(CipherString[0],i,~PlainStringBit_s[i]);
			}
		}
		else if( end_bit_offset > 0 && end_bit_offset <8)
		{
			for(int i=7-start_bit_offset;i>=7-end_bit_offset;i--)//Debug试一下
			{
				SetBitValue(CipherString[0],i,~PlainStringBit_s[i]);
			}
		}
		DecString[0]=CipherString[0];
	}
	else if(ShouldReadByteNum > 1)
	{
		//对于startByte
		bitset<8>	PlainStringBit_s(CipherString[0]);
		if(start_bit_offset == 0)
		{
			//比特位取反
			for(int i=7-start_bit_offset;i>=0;i--)
			{
				SetBitValue(CipherString[0],i,~PlainStringBit_s[i]);
			}
			DecString[0]=CipherString[0];
		}
		else if (start_bit_offset >0 && start_bit_offset <8)
		{
			//从7-startbitoffset开始递减按位取反，相当于从startbitoffset开始递增按位取反
			for(int i=7-start_bit_offset;i>=0;i--)
			{
				SetBitValue(CipherString[0],i,~PlainStringBit_s[i]);
			}
			DecString[0]=CipherString[0];
		}

		//对于lastByte
		bitset<8>	PlainStringBit_e(CipherString[ShouldReadByteNum-1]);
		if(end_bit_offset == 0)
		{
			for(int i=7;i > 7-start_bit_offset;i--)
			{
				SetBitValue(CipherString[ShouldReadByteNum-1],i,~PlainStringBit_e[i]);
			}
			DecString[ShouldReadByteNum-1]=CipherString[ShouldReadByteNum-1];
		}
		else if (end_bit_offset >0 && end_bit_offset <8)
		{
			//从7开始直到7-startbitoffset开始递减按位取反
			for(int i=7;i > 7-start_bit_offset;i--)
			{
				SetBitValue(CipherString[ShouldReadByteNum-1],i,~PlainStringBit_e[i]);
			}
			DecString[ShouldReadByteNum-1]=CipherString[ShouldReadByteNum-1];
		}

		if(ShouldReadByteNum > 2)
		{
			//替换成AES CTR解密
			int AESCipherlen=ShouldReadByteNum-2;
			unsigned char *AEScipher= new unsigned char[AESCipherlen];
			memset(AEScipher,0,AESCipherlen);
			for(int j=0;j<AESCipherlen;j++)
			{ 
				AEScipher[j]=CipherString[j+1];
			}
			//string keystr="1234567812345678";
			unsigned const char * slice_key = (unsigned const char *) keystr.data(); 
			unsigned char *iv=new unsigned char[16]; //初始向量为16字节,由解密端传递
			memcpy(iv, ivtrans, 16); 
	
			//解密
			unsigned char *AESDectext= new unsigned char[AESCipherlen];
			memset(AESDectext,0,AESCipherlen);
			BytesDecrypt(AEScipher,AESDectext,slice_key,AESCipherlen,iv);

			for(int j=0;j<AESCipherlen;j++)
			{ 
				DecString[j+1]=AESDectext[j];
			}

			delete []AEScipher;
			AEScipher=NULL;
			delete []AESDectext;
			AESDectext=NULL;
			delete []iv;
			iv=NULL;
		}	
	}

}