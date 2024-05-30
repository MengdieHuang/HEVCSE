/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2017, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TDecEntropy.cpp
    \brief    entropy decoder class
*/

#include "TDecEntropy.h"
#include "TLibCommon/TComTU.h"
#include "TLibCommon/TComPrediction.h"
#include "TLibDecoder/EncNALObject.h"//maggie

#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
#include "../TLibCommon/Debug.h"
static const Bool bDebugRQT = DebugOptionList::DebugRQT.getInt()!=0;
static const Bool bDebugPredEnabled = DebugOptionList::DebugPred.getInt()!=0;
#endif

//! \ingroup TLibDecoder
//! \{

Void TDecEntropy::setEntropyDecoder         ( TDecEntropyIf* p )
{
  m_pcEntropyDecoderIf = p;
}

#include "TLibCommon/TComSampleAdaptiveOffset.h"

Void TDecEntropy::decodeSkipFlag( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
	//----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------	
  m_pcEntropyDecoderIf->parseSkipFlag( pcCU, uiAbsPartIdx, uiDepth );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
  	RyanNALUEnc.ComputeSkipFlagOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
}


Void TDecEntropy::decodeCUTransquantBypassFlag(TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
	//----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------	
  m_pcEntropyDecoderIf->parseCUTransquantBypassFlag( pcCU, uiAbsPartIdx, uiDepth );
  	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
  	RyanNALUEnc.ComputeTransquantBypassFlagOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
}


/** decode merge flag
 * \param pcSubCU
 * \param uiAbsPartIdx
 * \param uiDepth
 * \param uiPUIdx
 * \returns Void
 */
Void TDecEntropy::decodeMergeFlag( TComDataCU* pcSubCU, UInt uiAbsPartIdx, UInt uiDepth, UInt uiPUIdx )
{
	//----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------	
	// at least one merge candidate exists
  m_pcEntropyDecoderIf->parseMergeFlag( pcSubCU, uiAbsPartIdx, uiDepth, uiPUIdx );
	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeMergeFlagOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------

}

/** decode merge index
 * \param pcCU
 * \param uiPartIdx
 * \param uiAbsPartIdx
 * \param uiDepth
 * \returns Void
 */
Void TDecEntropy::decodeMergeIndex( TComDataCU* pcCU, UInt uiPartIdx, UInt uiAbsPartIdx, UInt uiDepth )
{
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

  UInt uiMergeIndex = 0;
  m_pcEntropyDecoderIf->parseMergeIndex( pcCU, uiMergeIndex );
  pcCU->setMergeIndexSubParts( uiMergeIndex, uiAbsPartIdx, uiPartIdx, uiDepth );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeMergeIndexOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------

}

Void TDecEntropy::decodeSplitFlag   ( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

  m_pcEntropyDecoderIf->parseSplitFlag( pcCU, uiAbsPartIdx, uiDepth );
	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeSplitFlagOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
}

Void TDecEntropy::decodePredMode( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------
  m_pcEntropyDecoderIf->parsePredMode( pcCU, uiAbsPartIdx, uiDepth );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputePredModeOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
}

Void TDecEntropy::decodePartSize( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

  m_pcEntropyDecoderIf->parsePartSize( pcCU, uiAbsPartIdx, uiDepth );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputePartSizeOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);		
	//-------------------------
}

Void TDecEntropy::decodePredInfo    ( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth, TComDataCU* pcSubCU )
{
  if( pcCU->isIntra( uiAbsPartIdx ) )                                 // If it is Intra mode, encode intra prediction mode.
  {
    decodeIntraDirModeLuma  ( pcCU, uiAbsPartIdx, uiDepth );
    if (pcCU->getPic()->getChromaFormat()!=CHROMA_400)
    {
      decodeIntraDirModeChroma( pcCU, uiAbsPartIdx, uiDepth );
      if (enable4ChromaPUsInIntraNxNCU(pcCU->getPic()->getChromaFormat()) && pcCU->getPartitionSize( uiAbsPartIdx )==SIZE_NxN)
      {
        UInt uiPartOffset = ( pcCU->getPic()->getNumPartitionsInCtu() >> ( pcCU->getDepth(uiAbsPartIdx) << 1 ) ) >> 2;
        decodeIntraDirModeChroma( pcCU, uiAbsPartIdx + uiPartOffset,   uiDepth+1 );
        decodeIntraDirModeChroma( pcCU, uiAbsPartIdx + uiPartOffset*2, uiDepth+1 );
        decodeIntraDirModeChroma( pcCU, uiAbsPartIdx + uiPartOffset*3, uiDepth+1 );
      }
    }
  }
  else                                                                // if it is Inter mode, encode motion vector and reference index
  {
    decodePUWise( pcCU, uiAbsPartIdx, uiDepth, pcSubCU );
  }
}

/** Parse I_PCM information.
 * \param pcCU  pointer to CUpointer to CU
 * \param uiAbsPartIdx CU index
 * \param uiDepth CU depth
 * \returns Void
 */
Void TDecEntropy::decodeIPCMInfo( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
  if(!pcCU->getSlice()->getSPS()->getUsePCM()
    || pcCU->getWidth(uiAbsPartIdx) > (1<<pcCU->getSlice()->getSPS()->getPCMLog2MaxSize())
    || pcCU->getWidth(uiAbsPartIdx) < (1<<pcCU->getSlice()->getSPS()->getPCMLog2MinSize()) )
  {
    return;
  }
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

  m_pcEntropyDecoderIf->parseIPCMInfo( pcCU, uiAbsPartIdx, uiDepth );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeIPCMOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);			
	//-------------------------
}

Void TDecEntropy::decodeIntraDirModeLuma  ( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

  m_pcEntropyDecoderIf->parseIntraDirLumaAng( pcCU, uiAbsPartIdx, uiDepth );//cost 3 bit

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeIntraLumaPredOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);

	//-------------------------
}

Void TDecEntropy::decodeIntraDirModeChroma( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth )
{
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

  m_pcEntropyDecoderIf->parseIntraDirChroma( pcCU, uiAbsPartIdx, uiDepth );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeIntraChromaPredOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------

#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
  if (bDebugPredEnabled)
  {
    UInt cdir=pcCU->getIntraDir(CHANNEL_TYPE_CHROMA, uiAbsPartIdx);
    if (cdir==36)
    {
      cdir=pcCU->getIntraDir(CHANNEL_TYPE_LUMA, uiAbsPartIdx);
    }
    printf("coding chroma Intra dir: %d, uiAbsPartIdx: %d, luma dir: %d\n", cdir, uiAbsPartIdx, pcCU->getIntraDir(CHANNEL_TYPE_LUMA, uiAbsPartIdx));
  }
#endif
}


/** decode motion information for every PU block.
 * \param pcCU
 * \param uiAbsPartIdx
 * \param uiDepth
 * \param pcSubCU
 * \returns Void
 */
//解码帧间预测模式，要分merge和AMVP处理解码CU运动信息
Void TDecEntropy::decodePUWise( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth, TComDataCU* pcSubCU )
{
  //PU划分情况
  PartSize ePartSize = pcCU->getPartitionSize( uiAbsPartIdx );
  //分块数量
  UInt uiNumPU = ( ePartSize == SIZE_2Nx2N ? 1 : ( ePartSize == SIZE_NxN ? 4 : 2 ) );
  UInt uiPUOffset = ( g_auiPUOffset[UInt( ePartSize )] << ( ( pcCU->getSlice()->getSPS()->getMaxTotalCUDepth() - uiDepth ) << 1 ) ) >> 4;
  //候选MV列表
  TComMvField cMvFieldNeighbours[MRG_MAX_NUM_CANDS << 1]; // double length for mv of both lists
  //候选索引列表
  UChar uhInterDirNeighbours[MRG_MAX_NUM_CANDS];
  //初始化分块信息、候选列表
  for ( UInt ui = 0; ui < pcCU->getSlice()->getMaxNumMergeCand(); ui++ )
  {
    uhInterDirNeighbours[ui] = 0;
  }
  Int numValidMergeCand = 0;
  Bool hasMergedCandList = false;

#if MCTS_ENC_CHECK
  UInt numSpatialMergeCandidates = 0;
#endif
  pcSubCU->copyInterPredInfoFrom( pcCU, uiAbsPartIdx, REF_PIC_LIST_0 );//从CTU复制当前PU的帧间信息
  pcSubCU->copyInterPredInfoFrom( pcCU, uiAbsPartIdx, REF_PIC_LIST_1 );

  //------------------maggie------------
	Int BelCUInPicLoc_x, BelCUInPicLoc_y, CUW, CUH;
	pcSubCU->getPartPosition(0, BelCUInPicLoc_x, BelCUInPicLoc_y, CUW, CUH);
	//printf("CU(%d,%d)\n",BelCUInPicLoc_x,BelCUInPicLoc_y);
  //------------------------------------

  //遍历待解码CU的每一个PU，对其中所有PU分块进行帧间预测解码
  //这里只是在解码运动信息，没有进行预测图像的重构
  for ( UInt uiPartIdx = 0, uiSubPartIdx = uiAbsPartIdx; uiPartIdx < uiNumPU; uiPartIdx++, uiSubPartIdx += uiPUOffset )
  {


	 //int PUInPicLoc_x=BelCUInPicLoc_x+4*(j%2);//PU pixels 坐标
	 //int PUInPicLoc_y=BelCUInPicLoc_y+4*(j/2);
	 //int PUTable_x=PUInPicLoc_x/4;//PU[][]下标
	 //int PUTable_y=PUInPicLoc_y/4;

	//-----------------------------------

	//解码merge flag
    decodeMergeFlag( pcCU, uiSubPartIdx, uiDepth, uiPartIdx );	 
    if ( pcCU->getMergeFlag( uiSubPartIdx ) )//如果是merge模式,merge flag=true
    {//在merge模式中，MVP直接被当作MV（意味着没有MVD）
	  //解码Merge Index
      decodeMergeIndex( pcCU, uiPartIdx, uiSubPartIdx, uiDepth );
#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
      if (bDebugPredEnabled)
      {
        std::cout << "Coded merge flag, CU absPartIdx: " << uiAbsPartIdx << " PU(" << uiPartIdx << ") absPartIdx: " << uiSubPartIdx;
        std::cout << " merge index: " << (UInt)pcCU->getMergeIndex(uiSubPartIdx) << std::endl;
      }
#endif
	  //获取merge Index
      const UInt uiMergeIndex = pcCU->getMergeIndex(uiSubPartIdx);//当选最优PU在Merge候选列表中的索引
      if ( pcCU->getSlice()->getPPS()->getLog2ParallelMergeLevelMinus2() && ePartSize != SIZE_2Nx2N && pcSubCU->getWidth( 0 ) <= 8 )
      {//第一个分块时执行该if内容，初始化分块
        if ( !hasMergedCandList )
        {
          pcSubCU->setPartSizeSubParts( SIZE_2Nx2N, 0, uiDepth ); // temporarily set.
#if MCTS_ENC_CHECK
          numSpatialMergeCandidates = 0;
          pcSubCU->getInterMergeCandidates( 0, 0, cMvFieldNeighbours, uhInterDirNeighbours, numValidMergeCand, numSpatialMergeCandidates );
#else
          pcSubCU->getInterMergeCandidates( 0, 0, cMvFieldNeighbours, uhInterDirNeighbours, numValidMergeCand );
#endif
          pcSubCU->setPartSizeSubParts( ePartSize, 0, uiDepth ); // restore.
          hasMergedCandList = true;
        }
      }
      else//其他分块获取merge索引和候选，调用getInterMergeCandidates构造merge候选列表
      {
#if MCTS_ENC_CHECK
        numSpatialMergeCandidates = 0;
        pcSubCU->getInterMergeCandidates( uiSubPartIdx-uiAbsPartIdx, uiPartIdx, cMvFieldNeighbours, uhInterDirNeighbours, numValidMergeCand, numSpatialMergeCandidates, uiMergeIndex );
#else
        pcSubCU->getInterMergeCandidates( uiSubPartIdx-uiAbsPartIdx, uiPartIdx, cMvFieldNeighbours, uhInterDirNeighbours, numValidMergeCand, uiMergeIndex );
#endif
      }

#if MCTS_ENC_CHECK
      if (m_pConformanceCheck->getTMctsCheck() &&  pcSubCU->isLastColumnCTUInTile() && (uiMergeIndex >= numSpatialMergeCandidates) )
      {
        m_pConformanceCheck->flagTMctsError("Merge Index using non-spatial merge candidate (Merge)");
      }

#endif
	  //根据m索引取出选中的MV候选
      pcCU->setInterDirSubParts( uhInterDirNeighbours[uiMergeIndex], uiSubPartIdx, uiPartIdx, uiDepth );
      TComMv cTmpMv( 0, 0 );//cTmpMv是一个MV cTmpMv.x=0 cTmpMv.y=0
	  
		//----------------maggie------------------
		Int PUx, PUy, PUw, PUh;
		pcSubCU->getPartPosition(uiPartIdx, PUx, PUy, PUw, PUh);
		//printf("PU(%d,%d)的Merge候选列表\n",PUx,PUy);

		//for(int i=0;i<MRG_MAX_NUM_CANDS*2;i++)
		//{
		//	printf("list[%d] MV_x=%d MV_y=%d\n",i,
		//		cMvFieldNeighbours[i].getHor(),
		//		cMvFieldNeighbours[i].getVer());
		//}
		//printf("MergeIdx=%d\n当选的是：\n",uiMergeIndex);

		int PUInPicLoc_x=PUx;//merge Pu
		int PUInPicLoc_y=PUy;
		int PUTable_x=PUInPicLoc_x/4;//PU[][]下标
		int PUTable_y=PUInPicLoc_y/4;

		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x=BelCUInPicLoc_x;
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y=BelCUInPicLoc_y;
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].PUInPicLoc_x=PUInPicLoc_x;
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].PUInPicLoc_y=PUInPicLoc_y;
		
		//为下属4x4PU的truPU信息赋值
		int iMax=PUh/4;
		int jMax=PUw/4;
		for(int i=0;i<iMax;i++)
		{
			for(int j=0;j<jMax;j++)
			{
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].InTruPU_x=j;
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].InTruPU_y=i;
				//printf("sub4x4PU(%d,%d)\n",(PUTable_x+j)*4,(PUTable_y+i)*4);
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].TruPUw=PUw;
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].TruPUh=PUh;
			}
		}
		//----------------------------------------
	  
	  //遍历两个list list0和list1
      for ( UInt uiRefListIdx = 0; uiRefListIdx < 2; uiRefListIdx++ )
      {
		int refidx=pcCU->getSlice()->getNumRefIdx( RefPicList( uiRefListIdx ) );//maggie
        if ( pcCU->getSlice()->getNumRefIdx( RefPicList( uiRefListIdx ) ) > 0 )
        {
		  //setMVPIdxSubParts( Int iMVPIdx, RefPicList eRefPicList, UInt uiAbsPartIdx, UInt uiPartIdx, UInt uiDepth )
          pcCU->setMVPIdxSubParts( 0, RefPicList( uiRefListIdx ), uiSubPartIdx, uiPartIdx, uiDepth);        
		  pcCU->setMVPNumSubParts( 0, RefPicList( uiRefListIdx ), uiSubPartIdx, uiPartIdx, uiDepth);		
		  //把候选列表中的所有MVD赋值为cTmpMv（0，0）
          pcCU->getCUMvField( RefPicList( uiRefListIdx ) )->setAllMvd( cTmpMv, ePartSize, uiSubPartIdx, uiDepth, uiPartIdx );      
		  //把候选列表中的所有MV赋值为cMvFieldNeighbours[ 2*uiMergeIndex + uiRefListIdx ]
		  pcCU->getCUMvField( RefPicList( uiRefListIdx ) )->setAllMvField( cMvFieldNeighbours[ 2*uiMergeIndex + uiRefListIdx ], ePartSize, uiSubPartIdx, uiDepth, uiPartIdx );
  			
		  //----------------maggie------------------
			int maggie_idx=(uiMergeIndex<<1)+uiRefListIdx;//当是B帧时，就有两个当选MV
			if(uiRefListIdx==0)//前向当选 PU MV
			{
				RyanNALUEnc.PUTable[PUTable_x][PUTable_y].InterPredDir.ForwDir=cMvFieldNeighbours[maggie_idx].NeiDirect;
			}
			else if(uiRefListIdx==1)//后向当选PU MV
			{
				RyanNALUEnc.PUTable[PUTable_x][PUTable_y].InterPredDir.BackDir=cMvFieldNeighbours[maggie_idx].NeiDirect;
			}
			//printf("当选的是list%d[%d]-Direct%d MV_x=%d,MV_y=%d\n",
			//	uiRefListIdx,maggie_idx,
			//	cMvFieldNeighbours[maggie_idx].NeiDirect,
			//	cMvFieldNeighbours[maggie_idx].getHor(),
			//	cMvFieldNeighbours[maggie_idx].getVer());
			//------------------------     
		}
      }
    }
    else//如果不是merge模式，是AMVP模式
    {
		//-------------------maggie-----------------------
		Int PUx, PUy, PUw, PUh;
		pcSubCU->getPartPosition(uiPartIdx, PUx, PUy, PUw, PUh);
		//printf("PU(%d,%d)AMVP候选列表\n",PUx,PUy);

		int PUInPicLoc_x=PUx;//merge Pu
		int PUInPicLoc_y=PUy;
		int PUTable_x=PUInPicLoc_x/4;//PU[][]下标
		int PUTable_y=PUInPicLoc_y/4;

		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.x=BelCUInPicLoc_x;
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].BelCU.CULocInPic.y=BelCUInPicLoc_y;
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].PUInPicLoc_x=PUInPicLoc_x;
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].PUInPicLoc_y=PUInPicLoc_y;

		int iMax=PUh/4;
		int jMax=PUw/4;
		for(int i=0;i<iMax;i++)
		{
			for(int j=0;j<jMax;j++)
			{
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].InTruPU_x=j;
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].InTruPU_y=i;
				//printf("sub4x4PU(%d,%d)\n",(PUTable_x+j)*4,(PUTable_y+i)*4);
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].TruPUw=PUw;
				RyanNALUEnc.PUTable[PUTable_x+j][PUTable_y+i].TruPUh=PUh;
			}
		}
		//--------------------------------------------------------
	  //解码帧间预测方向,前向 后向或多方向
      decodeInterDirPU( pcCU, uiSubPartIdx, uiDepth, uiPartIdx );
	  //pcCU->setInterDirSubParts在decodeInterDirPU（）里完成
	  //遍历list0和list1
      for ( UInt uiRefListIdx = 0; uiRefListIdx < 2; uiRefListIdx++ )
      {
        if ( pcCU->getSlice()->getNumRefIdx( RefPicList( uiRefListIdx ) ) > 0 )
        {//当参考索引数大于0时，取出AMVP参考列表
          decodeRefFrmIdxPU( pcCU,    uiSubPartIdx,              uiDepth, uiPartIdx, RefPicList( uiRefListIdx ) );
		  decodeMvdPU      ( pcCU,    uiSubPartIdx,              uiDepth, uiPartIdx, RefPicList( uiRefListIdx ) );   	     
		  //！！！！解码MVP的PU索引调用decodeMVPIdxPU解码MVP索引，计算MV=MVP+MVD
		  decodeMVPIdxPU   ( pcSubCU, uiSubPartIdx-uiAbsPartIdx, uiDepth, uiPartIdx, RefPicList( uiRefListIdx ) );

#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
          if (bDebugPredEnabled)
          {
            std::cout << "refListIdx: " << uiRefListIdx << std::endl;
            std::cout << "MVD horizontal: " << pcCU->getCUMvField(RefPicList(uiRefListIdx))->getMvd( uiAbsPartIdx ).getHor() << std::endl;
            std::cout << "MVD vertical:   " << pcCU->getCUMvField(RefPicList(uiRefListIdx))->getMvd( uiAbsPartIdx ).getVer() << std::endl;
            std::cout << "MVPIdxPU: " << pcCU->getMVPIdx(RefPicList( uiRefListIdx ), uiSubPartIdx) << std::endl;
            std::cout << "InterDir: " << (UInt)pcCU->getInterDir(uiSubPartIdx) << std::endl;
          }
#endif
        }
      }
    }
	//不确认这一块是做的什么，好像是list1不可用
    if ( (pcCU->getInterDir(uiSubPartIdx) == 3) && pcSubCU->isBipredRestriction(uiPartIdx) )
    {
      pcCU->getCUMvField( REF_PIC_LIST_1 )->setAllMv( TComMv(0,0), ePartSize, uiSubPartIdx, uiDepth, uiPartIdx);
      pcCU->getCUMvField( REF_PIC_LIST_1 )->setAllRefIdx( -1, ePartSize, uiSubPartIdx, uiDepth, uiPartIdx);
      pcCU->setInterDirSubParts( 1, uiSubPartIdx, uiPartIdx, uiDepth);
    }
  }
  return;
}

/** decode inter direction for a PU block
 * \param pcCU
 * \param uiAbsPartIdx
 * \param uiDepth
 * \param uiPartIdx
 * \returns Void
 */
Void TDecEntropy::decodeInterDirPU( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth, UInt uiPartIdx )
{
  UInt uiInterDir;

  if ( pcCU->getSlice()->isInterP() )
  {
    uiInterDir = 1;
  }
  else
  {
	///----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

    m_pcEntropyDecoderIf->parseInterDir( pcCU, uiInterDir, uiAbsPartIdx );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeInterDirPUOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
  }

  pcCU->setInterDirSubParts( uiInterDir, uiAbsPartIdx, uiPartIdx, uiDepth );
}

Void TDecEntropy::decodeRefFrmIdxPU( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth, UInt uiPartIdx, RefPicList eRefList )
{
  Int iRefFrmIdx = 0;
  Int iParseRefFrmIdx = pcCU->getInterDir( uiAbsPartIdx ) & ( 1 << eRefList );

  if ( pcCU->getSlice()->getNumRefIdx( eRefList ) > 1 && iParseRefFrmIdx )
  {
	//----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

    m_pcEntropyDecoderIf->parseRefFrmIdx( pcCU, iRefFrmIdx, eRefList );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeRefFrameIdxOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
  }
  else if ( !iParseRefFrmIdx )
  {
    iRefFrmIdx = NOT_VALID;
  }
  else
  {
    iRefFrmIdx = 0;
  }

  PartSize ePartSize = pcCU->getPartitionSize( uiAbsPartIdx );
  pcCU->getCUMvField( eRefList )->setAllRefIdx( iRefFrmIdx, ePartSize, uiAbsPartIdx, uiDepth, uiPartIdx );
}

/** decode motion vector difference for a PU block
 * \param pcCU
 * \param uiAbsPartIdx
 * \param uiDepth
 * \param uiPartIdx
 * \param eRefList
 * \returns Void
 */
Void TDecEntropy::decodeMvdPU( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth, UInt uiPartIdx, RefPicList eRefList )
{
  if ( pcCU->getInterDir( uiAbsPartIdx ) & ( 1 << eRefList ) )
  {
	//----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

    m_pcEntropyDecoderIf->parseMvd( pcCU, uiAbsPartIdx, uiPartIdx, uiDepth, eRefList );
	
	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeMVDPUOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
  }
}

Void TDecEntropy::decodeMVPIdxPU( TComDataCU* pcSubCU, UInt uiPartAddr, UInt uiDepth, UInt uiPartIdx, RefPicList eRefList )
{
  Int iMVPIdx = -1;

  TComMv cZeroMv( 0, 0 );
  TComMv cMv     = cZeroMv;
  Int    iRefIdx = -1;

  TComCUMvField* pcSubCUMvField = pcSubCU->getCUMvField( eRefList );
  AMVPInfo* pAMVPInfo = pcSubCUMvField->getAMVPInfo();

  iRefIdx = pcSubCUMvField->getRefIdx(uiPartAddr);
  cMv = cZeroMv;

  if ( (pcSubCU->getInterDir(uiPartAddr) & ( 1 << eRefList )) )
  {
	//----------maggie---------
	int bitsNeeded_before;
	int bitsNeeded_after;
	bitsNeeded_before=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	//---------------------------

    m_pcEntropyDecoderIf->parseMVPIdx( iMVPIdx );

	//------maggie--------------
  	bitsNeeded_after=m_pcEntropyDecoderIf->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();
	int idx_cur=m_pcEntropyDecoderIf->Getm_pcBitstream()->getByteLocation();
	RyanNALUEnc.ComputeMVPIdxOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
	//-------------------------
  }

  //创建AMVP候选列表
  pcSubCU->fillMvpCand(uiPartIdx, uiPartAddr, eRefList, iRefIdx, pAMVPInfo);

#if MCTS_ENC_CHECK
  if ((iRefIdx >= 0) && m_pConformanceCheck->getTMctsCheck() && pcSubCU->isLastColumnCTUInTile() && (iMVPIdx == pAMVPInfo->numSpatialMVPCandidates))
  {
    m_pConformanceCheck->flagTMctsError("Merge Index using non-spatial merge candidate (AMVP)");
  }
#endif

  pcSubCU->setMVPNumSubParts(pAMVPInfo->iN, eRefList, uiPartAddr, uiPartIdx, uiDepth);
  pcSubCU->setMVPIdxSubParts( iMVPIdx, eRefList, uiPartAddr, uiPartIdx, uiDepth );//merge模式也有这一个

  if ( iRefIdx >= 0 )
  {
    m_pcPrediction->getMvPredAMVP( pcSubCU, uiPartIdx, uiPartAddr, eRefList, cMv);//拿到预测MV MVP
	//----------maggie--------------
	Int PUx, PUy, PUw, PUh;
	pcSubCU->getPartPosition(uiPartIdx, PUx, PUy, PUw, PUh);
	//printf("PU(%d,%d)AMVP候选列表\n",PUx,PUy);

	int PUInPicLoc_x=PUx;//merge Pu
	int PUInPicLoc_y=PUy;
	int PUTable_x=PUInPicLoc_x/4;//PU[][]下标
	int PUTable_y=PUInPicLoc_y/4;

	RyanNALUEnc.PUTable[PUTable_x][PUTable_y].PUInPicLoc_x=PUInPicLoc_x;
	RyanNALUEnc.PUTable[PUTable_x][PUTable_y].PUInPicLoc_y=PUInPicLoc_y;

	int maggie_idx=pcSubCU->getMVPIdx(eRefList,uiPartAddr);//maggie
	//for(int i=0;i<pAMVPInfo->iN;i++)
	//{
	//	printf("list%d[%d] MV_x=%d MV_y=%d\n",eRefList,i,
	//		pAMVPInfo->m_acMvCand[i].getHor(),
	//		pAMVPInfo->m_acMvCand[i].getVer());
	//}
	if(eRefList==0)//前向当选 PU MV
	{
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].InterPredDir.ForwDir=cMv.AMVPNeiDirect;
	}
	else if(eRefList==1)//后向当选PU MV
	{
		RyanNALUEnc.PUTable[PUTable_x][PUTable_y].InterPredDir.BackDir=cMv.AMVPNeiDirect;
	}
	//printf("当选的是：list%d[%d]-Direct%d MV_x=%d,MV_y=%d\n",
	//	eRefList,maggie_idx,
	//	cMv.AMVPNeiDirect,
	//	cMv.getHor(),
	//	cMv.getVer());
	//------------------------    
	cMv += pcSubCUMvField->getMvd( uiPartAddr );//MVP+MVD=MV
  }

  PartSize ePartSize = pcSubCU->getPartitionSize( uiPartAddr );
  pcSubCU->getCUMvField( eRefList )->setAllMv(cMv, ePartSize, uiPartAddr, 0, uiPartIdx);
}

Void TDecEntropy::xDecodeTransform        ( Bool& bCodeDQP, Bool& isChromaQpAdjCoded, TComTU &rTu, const Int quadtreeTULog2MinSizeInCU )
{
  TComDataCU *pcCU=rTu.getCU();
  const UInt uiAbsPartIdx=rTu.GetAbsPartIdxTU();
  const UInt uiDepth=rTu.GetTransformDepthTotal();
  const UInt uiTrDepth = rTu.GetTransformDepthRel();

  UInt uiSubdiv;
  const UInt numValidComponent = pcCU->getPic()->getNumberValidComponents();
  const Bool bChroma = isChromaEnabled(pcCU->getPic()->getChromaFormat());

  const UInt uiLog2TrafoSize = rTu.GetLog2LumaTrSize();
#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
  if (bDebugRQT)
  {
    printf("x..codeTransform: offsetLuma=%d offsetChroma=%d absPartIdx=%d, uiDepth=%d\n width=%d, height=%d, uiTrIdx=%d, uiInnerQuadIdx=%d\n",
        rTu.getCoefficientOffset(COMPONENT_Y), rTu.getCoefficientOffset(COMPONENT_Cb), uiAbsPartIdx, uiDepth, rTu.getRect(COMPONENT_Y).width, rTu.getRect(COMPONENT_Y).height, rTu.GetTransformDepthRel(), rTu.GetSectionNumber());
    fflush(stdout);
  }
#endif

  if( pcCU->isIntra(uiAbsPartIdx) && pcCU->getPartitionSize(uiAbsPartIdx) == SIZE_NxN && uiDepth == pcCU->getDepth(uiAbsPartIdx) )
  {
    uiSubdiv = 1;
  }
  else if( (pcCU->getSlice()->getSPS()->getQuadtreeTUMaxDepthInter() == 1) && (pcCU->isInter(uiAbsPartIdx)) && ( pcCU->getPartitionSize(uiAbsPartIdx) != SIZE_2Nx2N ) && (uiDepth == pcCU->getDepth(uiAbsPartIdx)) )
  {
    uiSubdiv = (uiLog2TrafoSize >quadtreeTULog2MinSizeInCU);
  }
  else if( uiLog2TrafoSize > pcCU->getSlice()->getSPS()->getQuadtreeTULog2MaxSize() )
  {
    uiSubdiv = 1;
  }
  else if( uiLog2TrafoSize == pcCU->getSlice()->getSPS()->getQuadtreeTULog2MinSize() )
  {
    uiSubdiv = 0;
  }
  else if( uiLog2TrafoSize == quadtreeTULog2MinSizeInCU )
  {
    uiSubdiv = 0;
  }
  else
  {
    assert( uiLog2TrafoSize > quadtreeTULog2MinSizeInCU );
    m_pcEntropyDecoderIf->parseTransformSubdivFlag( uiSubdiv, 5 - uiLog2TrafoSize );
  }

  for(Int chan=COMPONENT_Cb; chan<numValidComponent; chan++)
  {
    const ComponentID compID=ComponentID(chan);
    const UInt trDepthTotalAdj=rTu.GetTransformDepthTotalAdj(compID);

    const Bool bFirstCbfOfCU = uiTrDepth == 0;

    if( bFirstCbfOfCU )
    {
      pcCU->setCbfSubParts( 0, compID, rTu.GetAbsPartIdxTU(compID), trDepthTotalAdj);
    }
    if( bFirstCbfOfCU || rTu.ProcessingAllQuadrants(compID) )
    {
      if( bFirstCbfOfCU || pcCU->getCbf( uiAbsPartIdx, compID, uiTrDepth - 1 ) )
      {
        m_pcEntropyDecoderIf->parseQtCbf( rTu, compID, (uiSubdiv == 0) );
      }
    }
  }

  if( uiSubdiv )
  {
    const UInt uiQPartNum = pcCU->getPic()->getNumPartitionsInCtu() >> ((uiDepth+1) << 1);
    UInt uiYUVCbf[MAX_NUM_COMPONENT] = {0,0,0};

    TComTURecurse tuRecurseChild(rTu, true);

    do
    {
      xDecodeTransform( bCodeDQP, isChromaQpAdjCoded, tuRecurseChild, quadtreeTULog2MinSizeInCU );
      UInt childTUAbsPartIdx=tuRecurseChild.GetAbsPartIdxTU();
      for(UInt ch=0; ch<numValidComponent; ch++)
      {
        uiYUVCbf[ch] |= pcCU->getCbf(childTUAbsPartIdx , ComponentID(ch),  uiTrDepth+1 );
      }
    } while (tuRecurseChild.nextSection(rTu) );

    for(UInt ch=0; ch<numValidComponent; ch++)
    {
      UChar *pBase = pcCU->getCbf( ComponentID(ch) ) + uiAbsPartIdx;
      const UChar flag = uiYUVCbf[ch] << uiTrDepth;

      for( UInt ui = 0; ui < 4 * uiQPartNum; ++ui )
      {
        pBase[ui] |= flag;
      }
    }
  }
  else
  {
    assert( uiDepth >= pcCU->getDepth( uiAbsPartIdx ) );
    pcCU->setTrIdxSubParts( uiTrDepth, uiAbsPartIdx, uiDepth );

    {
      DTRACE_CABAC_VL( g_nSymbolCounter++ );
      DTRACE_CABAC_T( "\tTrIdx: abspart=" );
      DTRACE_CABAC_V( uiAbsPartIdx );
      DTRACE_CABAC_T( "\tdepth=" );
      DTRACE_CABAC_V( uiDepth );
      DTRACE_CABAC_T( "\ttrdepth=" );
      DTRACE_CABAC_V( uiTrDepth );
      DTRACE_CABAC_T( "\n" );
    }

    pcCU->setCbfSubParts ( 0, COMPONENT_Y, uiAbsPartIdx, uiDepth );

    if( (!pcCU->isIntra(uiAbsPartIdx)) && uiDepth == pcCU->getDepth( uiAbsPartIdx ) && ((!bChroma) || (!pcCU->getCbf( uiAbsPartIdx, COMPONENT_Cb, 0 ) && !pcCU->getCbf( uiAbsPartIdx, COMPONENT_Cr, 0 )) ))
    {
      pcCU->setCbfSubParts( 1 << uiTrDepth, COMPONENT_Y, uiAbsPartIdx, uiDepth );
    }
    else
    {
      m_pcEntropyDecoderIf->parseQtCbf( rTu, COMPONENT_Y, true );
    }

    // transform_unit begin
    UInt cbf[MAX_NUM_COMPONENT]={0,0,0};
    Bool validCbf       = false;
    Bool validChromaCbf = false;
    const UInt uiTrIdx = rTu.GetTransformDepthRel();

    for(UInt ch=0; ch<pcCU->getPic()->getNumberValidComponents(); ch++)
    {
      const ComponentID compID = ComponentID(ch);

      cbf[compID] = pcCU->getCbf( uiAbsPartIdx, compID, uiTrIdx );

      if (cbf[compID] != 0)
      {
        validCbf = true;
        if (isChroma(compID))
        {
          validChromaCbf = true;
        }
      }
    }

    if ( validCbf )
    {

      // dQP: only for CTU
      if ( pcCU->getSlice()->getPPS()->getUseDQP() )
      {
        if ( bCodeDQP )
        {
          const UInt uiAbsPartIdxCU=rTu.GetAbsPartIdxCU();
          decodeQP( pcCU, uiAbsPartIdxCU);
          bCodeDQP = false;
        }
      }

      if ( pcCU->getSlice()->getUseChromaQpAdj() )
      {
        if ( validChromaCbf && isChromaQpAdjCoded && !pcCU->getCUTransquantBypass(rTu.GetAbsPartIdxCU()) )
        {
          decodeChromaQpAdjustment( pcCU, rTu.GetAbsPartIdxCU() );
          isChromaQpAdjCoded = false;
        }
      }

      const UInt numValidComp=pcCU->getPic()->getNumberValidComponents();

      for(UInt ch=COMPONENT_Y; ch<numValidComp; ch++)
      {
        const ComponentID compID=ComponentID(ch);

        if( rTu.ProcessComponentSection(compID) )
        {
#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
          if (bDebugRQT)
          {
            printf("Call NxN for chan %d width=%d height=%d cbf=%d\n", compID, rTu.getRect(compID).width, rTu.getRect(compID).height, 1);
          }
#endif

          if (rTu.getRect(compID).width != rTu.getRect(compID).height)
          {
            //code two sub-TUs
            TComTURecurse subTUIterator(rTu, false, TComTU::VERTICAL_SPLIT, true, compID);

            do
            {
              const UInt subTUCBF = pcCU->getCbf(subTUIterator.GetAbsPartIdxTU(), compID, (uiTrIdx + 1));

              if (subTUCBF != 0)
              {
#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
                if (bDebugRQT)
                {
                  printf("Call NxN for chan %d width=%d height=%d cbf=%d\n", compID, subTUIterator.getRect(compID).width, subTUIterator.getRect(compID).height, 1);
                }
#endif
                m_pcEntropyDecoderIf->parseCoeffNxN( subTUIterator, compID );
              }
            } while (subTUIterator.nextSection(rTu));
          }
          else
          {
            if(isChroma(compID) && (cbf[COMPONENT_Y] != 0))
            {
              m_pcEntropyDecoderIf->parseCrossComponentPrediction( rTu, compID );
            }

            if(cbf[compID] != 0)
            {
              m_pcEntropyDecoderIf->parseCoeffNxN( rTu, compID );
            }
          }
        }
      }
    }
    // transform_unit end
  }
}

Void TDecEntropy::decodeQP          ( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  if ( pcCU->getSlice()->getPPS()->getUseDQP() )
  {
    m_pcEntropyDecoderIf->parseDeltaQP( pcCU, uiAbsPartIdx, pcCU->getDepth( uiAbsPartIdx ) );
  }
}

Void TDecEntropy::decodeChromaQpAdjustment( TComDataCU* pcCU, UInt uiAbsPartIdx )
{
  if ( pcCU->getSlice()->getUseChromaQpAdj() )
  {
    m_pcEntropyDecoderIf->parseChromaQpAdjustment( pcCU, uiAbsPartIdx, pcCU->getDepth( uiAbsPartIdx ) );
  }
}


//! decode coefficients
Void TDecEntropy::decodeCoeff( TComDataCU* pcCU, UInt uiAbsPartIdx, UInt uiDepth, Bool& bCodeDQP, Bool& isChromaQpAdjCoded )
{
  if( pcCU->isIntra(uiAbsPartIdx) )
  {
  }
  else
  {
    UInt uiQtRootCbf = 1;
    if( !( pcCU->getPartitionSize( uiAbsPartIdx) == SIZE_2Nx2N && pcCU->getMergeFlag( uiAbsPartIdx ) ) )
    {
      m_pcEntropyDecoderIf->parseQtRootCbf( uiAbsPartIdx, uiQtRootCbf );
    }
    if ( !uiQtRootCbf )
    {
      static const UInt cbfZero[MAX_NUM_COMPONENT]={0,0,0};
      pcCU->setCbfSubParts( cbfZero, uiAbsPartIdx, uiDepth );
      pcCU->setTrIdxSubParts( 0 , uiAbsPartIdx, uiDepth );
      return;
    }

  }

  TComTURecurse tuRecurse(pcCU, uiAbsPartIdx, uiDepth);

#if ENVIRONMENT_VARIABLE_DEBUG_AND_TEST
  if (bDebugRQT)
  {
    printf("..codeCoeff: uiAbsPartIdx=%d, PU format=%d, 2Nx2N=%d, NxN=%d\n", uiAbsPartIdx, pcCU->getPartitionSize(uiAbsPartIdx), SIZE_2Nx2N, SIZE_NxN);
  }
#endif

  Int quadtreeTULog2MinSizeInCU = pcCU->getQuadtreeTULog2MinSizeInCU(uiAbsPartIdx);
  
  xDecodeTransform( bCodeDQP, isChromaQpAdjCoded, tuRecurse, quadtreeTULog2MinSizeInCU );
}

//! \}
