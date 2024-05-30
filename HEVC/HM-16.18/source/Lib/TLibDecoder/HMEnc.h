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
	void init();//������Ա����
};

class CostBits
{
public:
	int Totalcostbits;
public:
	CostBits();
	~CostBits();
	void init();//1��Ա����
	int ComputeThisCostbits(int idxbefore,int idxafter,int neededbefore,int neededafter);//����Ա��ֵ
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
	void init();//������Ա����
	int ComputeThisCostbits(int idxbefore,int idxafter,int neededbefore,int neededafter);//����Ա��ֵ
};

class OffsetInfo
{
public:
	int InFileOffsset;//bit��λ
	int InNALOffset;
public:
	OffsetInfo();
	~OffsetInfo();
	void init();//��2����Ա����
};

class ChunkInfo
{
public:
	int ChunkInFileOffset;//bit��λ
	CostBits ChunkCostBits;//bit��λ
	int ChunkLen; //�ֽڵ�λ
public:
	ChunkInfo();
	~ChunkInfo();
	void init();//��������Ա����,����ChunkCostBits��init();
};

class NALHeaderInfo
{
public:
	OffsetInfo NALHeaderOffset;
	CostBits NALHeaderCostBits;//bit��λ
	int NALHeaderLen;
public:
	NALHeaderInfo();
	~NALHeaderInfo();
	void init();//��1����Ա����,����NALHeaderOffsetInfo��NALHeaderCostBits��init();
};

class SliceHeaderInfo
{
public:
	OffsetInfo SliceHeaderOffset;
	CostBits SliceHeaderCostBits;//bit��λ
	int SliceHeaderLen;
public:
	SliceHeaderInfo();
	~SliceHeaderInfo();
	void init();//��1����Ա����,����SliceHeaderOffsetInfo��SliceHeaderCostBits��init();
};

class SAOInfo
{
public:
	OffsetInfo SAOOffsetInfo;
	CostBits SAOCostBits;//bit��λ
public:
	SAOInfo();
	~SAOInfo();
	void init();//����SAOOffsetInfo��SAOCostBits��init();
private:
};

class CUInfo
{
public:
	Location CULocInPic;		//CU����
	OffsetInfo CUOffsetInfo;	//CU�����ж�λ
	CUCostBits CostBits;		//CU����������

	float CUSpaEff;				//Intra CU�������Ӱ��
	float CUTemEff;				//Inter CU�������Ӱ��

	OffsetInfo LumaDirMode;		//����Ԥ��ģʽ
	int LumaDirModeLen;
	OffsetInfo ChromaDirMode;	//ɫ��Ԥ��ģʽ
	int ChromaDirModeLen;
	//OffsetInfo SAO;			//SAO
	//int SAOLen;
	OffsetInfo Coefficient;		//�仯ϵ��
	int CoefficientLen;	
	OffsetInfo PartSize;		
	int PartSizeLen;
	OffsetInfo SplitFlag;
	int SplitFlagLen;			//CTU���и��־
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
	void init();//����LocationInCTU��CUCostBits��OffsetInfo��init();
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
	int InTruPU_x;			 //4x4������PU������
	int InTruPU_y;
	int TruPUSize;			 //4x4������PU��size 4x4/8x8/16x16/32x32/64x64
	int TruPUw;				 //4x4������PU�Ŀ��
	int TruPUh;
	int PUInPicLoc_x;		
	int PUInPicLoc_y;		
	CUInfo BelCU;			 //4x4������CU����Ϣ
	int IntraPredMode;		 //Intra PU ��֡�ڽǶ�Ԥ��ģʽֵ[0-35] 
	float TruPU_SpatEFF;	 //Intra PU ��Spatial effect
	Dirlist InterPredDir;	 //Inter PU MVP�Ĳο�PU����[ L /A /RA /LB /LA]
	float TruInterPU_SpatEff;//Inter PU ��Spatial effect
public:
	PUInfo();
	~PUInfo();
	void init();
};

class TableU
{
public:
	int x;//intable�±�
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
	ChunkInfo BelongChunkInfo;	 //offset��Byte��λ
	NALHeaderInfo NALHeaderInfo; //offset��Byte��λ
	int NALLen;
	vector<uint8_t> myfifo;
	int MY_bitoffsetinNAL;
	int MY_m_fifo_idx_my;

	SliceHeaderInfo SliceHeaderInfo;
	SAOInfo SAOInfo;
	CUInfo myCUInfo;
	
	int PicWid; //Pixel
	int PicHei;

	PUInfo** PUTable;//��4x4����Ϊ��λ
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

	//���㶨λ
	void ComputeChunkInFileOffset(AnnexBStats stats);
	void ComputeNALHeaderInFileOffset(AnnexBStats stats);//ͬԭ��NALprint��������
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

	//������־λ00 00 03/02/01/00 �Ĵ���
	Bool AddZerothree(UInt &form5byte,UInt &form4byte,UInt &form3byte,UInt &form2byte, UInt &form1byte,UInt &form0byte,int curByte,int NextByte);
	void DecAddEight(int curByte, int NextByte);

	//��ʼ�����ͷ�
	void InitMyfifoIdx();
	void InitPUTable();
	void DeletePUTable();
	Float ComputeModePercen(int mode, int direction ,int label);
	void SetSpatEffTable();
	void DeleteSpatEffTable();

	//��ӡ
	void PrintPUTable(int SliceType);
	void PrintSptEffTable();

	//ģ�ͼ���
	void ModelCalcute(SliceType slicetype);
	void ComputeIntraPUSpatEFF();
	void ComputeIntraPUNeiPUSpaEff(int CurPuLoc_x,int CurPuLoc_y,int CurPuSize,int CurPuMode);
	//void ComputeIntraCUSpatEff();
	void ComputeInterPUSpaEff();
	void ComInterPUNeiPUSpaEff(int CurPuLoc_x,int CurPuLoc_y,int PUw,int PUh,int PUFormDir,int PUBackDir);
	//void ComputeInterCUSpatEff();
	void ComputeCUSpatEff();

	//ѡ�����
	void SortCUEff();
	void PickEncU(BitExtractUnit *ExtractU,int ExtractedCUNum);
	//����
	void EncSlice(string inpath);
	void EncryptPlain(string PlainPath,BitExtractUnit *ExtractBuf,int ExtractedNum);
	//void EncryptOnce(int ReadByteNum,char * PlainString,char *CipherString,int start_bit,int end_bit);
	void EncryptOnce(int ReadByteNum, char * PlainString, char *CipherString,int start_bit,int end_bit,unsigned char *ivtrans);
	void DecryptOnce(int ReadByteNum, char * CipherString, char *DecString,int start_bit,int end_bit,unsigned char *ivtrans);

private:
	//���㶨λ
	void ComputeNALHeaderInNALOffset();
};

void SetBitValue(char & szTemp,const int nPos,const int nValue);
float Triangle2PerEqu(int A,int B,int d);
float TrianglePerEqu(int A,int B,int d);
float ComputePUSpaEff(float percen,int curPusize,int truPUsize);
CUInfo *insert_CUsort(CUInfo array[],int len);	
#endif