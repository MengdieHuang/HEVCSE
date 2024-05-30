#ifndef HMENC_H
#define HMENC_H

#include "TLibDecoder/AnnexBread.h"
#include "TLibDecoder/NALread.h"
#include "TLibDecoder/TDecCu.h"
using namespace std;

class Location
{
public:
	int x;	
	int y;
public:
	Location();
	~Location();
	void init();//两个成员置零
};

class CostBits
{
public:
	int Totalcostbits;
public:
	CostBits();
	~CostBits();
	void init();//1成员置零
	int ComputeThisCostbits(int idxbefore,int idxafter,int neededbefore,int neededafter);//给成员赋值
};

class CUCostBits:CostBits
{
public:
	int Totalcostbits;
	int PredBits;
	int ResiBits;
public:
	CUCostBits();
	~CUCostBits();
	void init();//两个成员置零
	int ComputeThisCostbits(int idxbefore,int idxafter,int neededbefore,int neededafter);//给成员赋值
};

class OffsetInfo
{
public:
	int InFileOffsset;//bit单位
	int InNALOffset;
public:
	OffsetInfo();
	~OffsetInfo();
	void init();//将2个成员置零
};

class ChunkInfo
{
public:
	int ChunkInFileOffset;//bit单位
	CostBits ChunkCostBits;//bit单位
	int ChunkLen; //字节单位
public:
	ChunkInfo();
	~ChunkInfo();
	void init();//将两个成员置零,调用ChunkCostBits的init();
};

class NALHeaderInfo
{
public:
	OffsetInfo NALHeaderOffset;
	CostBits NALHeaderCostBits;//bit单位
	int NALHeaderLen;
public:
	NALHeaderInfo();
	~NALHeaderInfo();
	void init();//将1个成员置零,调用NALHeaderOffsetInfo和NALHeaderCostBits的init();
};

class SliceHeaderInfo
{
public:
	OffsetInfo SliceHeaderOffset;
	CostBits SliceHeaderCostBits;//bit单位
	int SliceHeaderLen;
public:
	SliceHeaderInfo();
	~SliceHeaderInfo();
	void init();//将1个成员置零,调用SliceHeaderOffsetInfo和SliceHeaderCostBits的init();
};

class SAOInfo
{
public:
	OffsetInfo SAOOffsetInfo;
	CostBits SAOCostBits;//bit单位
public:
	SAOInfo();
	~SAOInfo();
	void init();//调用SAOOffsetInfo和SAOCostBits的init();
private:
};

class CUInfo
{
public:
	Location CULocInPic;		//CU坐标
	OffsetInfo CUOffsetInfo;	//CU码流中定位
	CUCostBits CostBits;		//CU比特流长度

	float CUSpaEff;				//Intra CU空域相关影响
	float CUTemEff;				//Inter CU空域相关影响

	OffsetInfo LumaDirMode;		//亮度预测模式
	int LumaDirModeLen;
	OffsetInfo ChromaDirMode;	//色度预测模式
	int ChromaDirModeLen;
	//OffsetInfo SAO;			//SAO
	//int SAOLen;
	OffsetInfo Coefficient;		//变化系数
	int CoefficientLen;	
	OffsetInfo PartSize;		
	int PartSizeLen;
	OffsetInfo SplitFlag;
	int SplitFlagLen;			//CTU的切割标志
	OffsetInfo IPCM;
	int IPCMLen;
	OffsetInfo MergeIdx;
	int MergeIdxLen;
	OffsetInfo SkipFlag;
	int SkipFlagLen;
	OffsetInfo InterDir;
	int InterDirLen;
	OffsetInfo RefIdx;
	int RefIdxLen;
	OffsetInfo MVD;
	int MVDLen;
	OffsetInfo AMVPIdx;
	int AMVPIdxLen;

public:
	CUInfo();
	~CUInfo();
	void init();//调用LocationInCTU和CUCostBits和OffsetInfo的init();
};

enum CUType
{
	IntraCU = 1,
	InterCU = 0
};

class Dirlist
{
public:
	int ForwDir;	//L=0 A=1 RA=2 LB=3 LA=4
	int BackDir;
public:
	Dirlist();
	~Dirlist();
	void init();
};

class PUInfo
{
public:	
	int InTruPU_x;			 //4x4块所属PU的坐标
	int InTruPU_y;
	int TruPUSize;			 //4x4块所属PU的size 4x4/8x8/16x16/32x32/64x64
	int TruPUw;				 //4x4块所属PU的宽高
	int TruPUh;
	int PUInPicLoc_x;		
	int PUInPicLoc_y;		
	CUInfo BelCU;			 //4x4块所属CU的信息
	int IntraPredMode;		 //Intra PU 的帧内角度预测模式值[0-35] 
	float TruPU_SpatEFF;	 //Intra PU 的Spatial effect
	Dirlist InterPredDir;	 //Inter PU MVP的参考PU方向[ L /A /RA /LB /LA]
	float TruInterPU_SpatEff;//Inter PU 的Spatial effect
public:
	PUInfo();
	~PUInfo();
	void init();
};

class TableU
{
public:
	int x;//intable下标
	int y;
	float percentage;
	int truPU_x;
	int truPU_y;
public:
	TableU();
	~TableU();
	void init();
};

class BitExtractUnit
{
public:
	int offset;
	int len;
public:
	BitExtractUnit();
	~BitExtractUnit();

};

class NALInfo
{
public:
	ChunkInfo BelongChunkInfo;	 //offset是Byte单位
	NALHeaderInfo NALHeaderInfo; //offset是Byte单位
	int NALLen;
	vector<uint8_t> myfifo;
	int MY_bitoffsetinNAL;
	int MY_m_fifo_idx_my;

	SliceHeaderInfo SliceHeaderInfo;
	SAOInfo SAOInfo;
	CUInfo myCUInfo;
	
	int PicWid; //Pixel
	int PicHei;

	PUInfo** PUTable;//以4x4像素为单位
	float ***Percen;// Intra PU Spateff Table
	int CurCUInTab_x;
	int CurCUInTab_y;
	CUInfo *CUArray;
	CUInfo *SortedCU;
	int CUnum;
	string keystr;
	Double EncTime;
	Double ParseTime;
	Double ModelTime;
	Double AESTime;
public:
	NALInfo();
	~NALInfo();
	void init();

	//计算定位
	void ComputeChunkInFileOffset(AnnexBStats stats);
	void ComputeNALHeaderInFileOffset(AnnexBStats stats);//同原来NALprint（）函数
	void ComputeSliceHeaderOffset(int MY_m_fifo_idx_cur);
	void ComputeSAOOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeSplitFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeSkipFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeMergeIndexOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeFinishDecodeCUOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputePredModeOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputePartSizeOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeIPCMOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputePredOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeCoeffOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeIntraLumaPredOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeIntraChromaPredOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeMergeFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeInterDirPUOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeRefFrameIdxOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeMVDPUOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeMVPIdxOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);
	void ComputeTransquantBypassFlagOffset(int MY_m_fifo_idx_cur,int bitsNeededBefore,int bitsNeededafter);

	//遇到标志位00 00 03/02/01/00 的处理
	Bool AddZerothree(UInt &form5byte,UInt &form4byte,UInt &form3byte,UInt &form2byte, UInt &form1byte,UInt &form0byte,int curByte,int NextByte);
	void DecAddEight(int curByte, int NextByte);

	//初始化和释放
	void InitMyfifoIdx();
	void InitPUTable();
	void DeletePUTable();
	Float ComputeModePercen(int mode, int direction ,int label);
	void SetSpatEffTable();
	void DeleteSpatEffTable();

	//打印
	void PrintPUTable(int SliceType);
	void PrintSptEffTable();

	//模型计算
	void ModelCalcute(SliceType slicetype);
	void ComputeIntraPUSpatEFF();
	void ComputeIntraPUNeiPUSpaEff(int CurPuLoc_x,int CurPuLoc_y,int CurPuSize,int CurPuMode);
	//void ComputeIntraCUSpatEff();
	void ComputeInterPUSpaEff();
	void ComInterPUNeiPUSpaEff(int CurPuLoc_x,int CurPuLoc_y,int PUw,int PUh,int PUFormDir,int PUBackDir);
	//void ComputeInterCUSpatEff();
	void ComputeCUSpatEff();

	//选择策略
	void SortCUEff();
	void PickEncU(BitExtractUnit *ExtractU,int ExtractedCUNum);
	//加密
	void EncSlice(string inpath);
	void EncryptPlain(string PlainPath,BitExtractUnit *ExtractBuf,int ExtractedNum);
	//void EncryptOnce(int ReadByteNum,char * PlainString,char *CipherString,int start_bit,int end_bit);
	void EncryptOnce(int ReadByteNum, char * PlainString, char *CipherString,int start_bit,int end_bit,unsigned char *ivtrans);
	void DecryptOnce(int ReadByteNum, char * CipherString, char *DecString,int start_bit,int end_bit,unsigned char *ivtrans);

private:
	//计算定位
	void ComputeNALHeaderInNALOffset();
};

void SetBitValue(char & szTemp,const int nPos,const int nValue);
float Triangle2PerEqu(int A,int B,int d);
float TrianglePerEqu(int A,int B,int d);
float ComputePUSpaEff(float percen,int curPusize,int truPUsize);
CUInfo *insert_CUsort(CUInfo array[],int len);	
#endif