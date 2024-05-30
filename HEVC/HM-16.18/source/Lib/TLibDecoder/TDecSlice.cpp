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

/** \file     TDecSlice.cpp
    \brief    slice decoder class
*/

#include "TDecSlice.h"
#include "TDecConformance.h"
#include "HMEnc.h"	//Maggie
#include "EncNALObject.h"//maggie
#include <fstream>//maggie
//! \ingroup TLibDecoder
//! \{

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TDecSlice::TDecSlice()
{
}

TDecSlice::~TDecSlice()
{
}

Void TDecSlice::create()
{
}

Void TDecSlice::destroy()
{
}

Void TDecSlice::init(TDecEntropy* pcEntropyDecoder, TDecCu* pcCuDecoder, TDecConformanceCheck *pDecConformanceCheck)
{
  m_pcEntropyDecoder     = pcEntropyDecoder;
  m_pcCuDecoder          = pcCuDecoder;
  m_pDecConformanceCheck = pDecConformanceCheck;
}


Void TDecSlice::decompressSlice(TComInputBitstream** ppcSubstreams, TComPic* pcPic, TDecSbac* pcSbacDecoder)
{
  TComSlice* pcSlice                 = pcPic->getSlice(pcPic->getCurrSliceIdx());

  const Int  startCtuTsAddr          = pcSlice->getSliceSegmentCurStartCtuTsAddr();
  const Int  startCtuRsAddr          = pcPic->getPicSym()->getCtuTsToRsAddrMap(startCtuTsAddr);
  const UInt numCtusInFrame          = pcPic->getNumberOfCtusInFrame();

  const UInt frameWidthInCtus        = pcPic->getPicSym()->getFrameWidthInCtus();
  const Bool depSliceSegmentsEnabled = pcSlice->getPPS()->getDependentSliceSegmentsEnabledFlag();
  const Bool wavefrontsEnabled       = pcSlice->getPPS()->getEntropyCodingSyncEnabledFlag();

  //---------------maggie------------------
  int bitsNeeded_before=-8;
  int bitsNeeded_after;
  //---------------------------------------

  m_pcEntropyDecoder->setEntropyDecoder ( pcSbacDecoder  );
  m_pcEntropyDecoder->setBitstream      ( ppcSubstreams[0] );//m_fifo_idx置为0
  m_pcEntropyDecoder->resetEntropy      (pcSlice);

  //----------------maggie------------------
  RyanNALUEnc.MY_m_fifo_idx_my=m_pcEntropyDecoder->Getm_pcEntropyDecoderIf()->Getm_pcBitstream()->getByteLocation();
  RyanNALUEnc.MY_bitoffsetinNAL += 8;
  //----------------------------------------

  // decoder doesn't need prediction & residual frame buffer
  pcPic->setPicYuvPred( 0 );
  pcPic->setPicYuvResi( 0 );

#if ENC_DEC_TRACE
  g_bJustDoIt = g_bEncDecTraceEnable;
#endif
  DTRACE_CABAC_VL( g_nSymbolCounter++ );
  DTRACE_CABAC_T( "\tPOC: " );
  DTRACE_CABAC_V( pcPic->getPOC() );
  DTRACE_CABAC_T( "\n" );

#if ENC_DEC_TRACE
  g_bJustDoIt = g_bEncDecTraceDisable;
#endif

  // The first CTU of the slice is the first coded substream, but the global substream number, as calculated by getSubstreamForCtuAddr may be higher.
  // This calculates the common offset for all substreams in this slice.

  const UInt subStreamOffset=pcPic->getSubstreamForCtuAddr(startCtuRsAddr, true, pcSlice);


  if (depSliceSegmentsEnabled) //处理dependent slice
  {
    // modify initial contexts with previous slice segment if this is a dependent slice.
    const UInt startTileIdx=pcPic->getPicSym()->getTileIdxMap(startCtuRsAddr);
    const TComTile *pCurrentTile=pcPic->getPicSym()->getTComTile(startTileIdx);
    const UInt firstCtuRsAddrOfTile = pCurrentTile->getFirstCtuRsAddr();

    if( pcSlice->getDependentSliceSegmentFlag() && startCtuRsAddr != firstCtuRsAddrOfTile)
    {
      if ( pCurrentTile->getTileWidthInCtus() >= 2 || !wavefrontsEnabled)
      {
        pcSbacDecoder->loadContexts(&m_lastSliceSegmentEndContextState);
      }
    }
  }

  // for every CTU in the slice segment...
  //遍历所有CTU进行解码
  //----maggie-----------
  clock_t ParseBeforeTime = clock();
  //---------------------
  Bool isLastCtuOfSliceSegment = false;
  for( UInt ctuTsAddr = startCtuTsAddr; !isLastCtuOfSliceSegment && ctuTsAddr < numCtusInFrame; ctuTsAddr++)
  {

    const UInt ctuRsAddr = pcPic->getPicSym()->getCtuTsToRsAddrMap(ctuTsAddr);
    const TComTile &currentTile = *(pcPic->getPicSym()->getTComTile(pcPic->getPicSym()->getTileIdxMap(ctuRsAddr)));
    const UInt firstCtuRsAddrOfTile = currentTile.getFirstCtuRsAddr();
    const UInt tileXPosInCtus = firstCtuRsAddrOfTile % frameWidthInCtus;
    const UInt tileYPosInCtus = firstCtuRsAddrOfTile / frameWidthInCtus;
    const UInt ctuXPosInCtus  = ctuRsAddr % frameWidthInCtus;
    const UInt ctuYPosInCtus  = ctuRsAddr / frameWidthInCtus;
    const UInt uiSubStrm=pcPic->getSubstreamForCtuAddr(ctuRsAddr, true, pcSlice)-subStreamOffset;
    TComDataCU* pCtu = pcPic->getCtu( ctuRsAddr );
    pCtu->initCtu( pcPic, ctuRsAddr );	//初始化CTU

    m_pcEntropyDecoder->setBitstream( ppcSubstreams[uiSubStrm] );//设置熵解码器
	//maggie--------
	RyanNALUEnc.myfifo = ppcSubstreams[uiSubStrm]->getFifo();
	//---------
    // set up CABAC contexts' state for this CTU	
    if (ctuRsAddr == firstCtuRsAddrOfTile) //为CTU设置上下文模型
    {
      if (ctuTsAddr != startCtuTsAddr) // if it is the first CTU, then the entropy coder has already been reset
      {
        m_pcEntropyDecoder->resetEntropy(pcSlice);
      }
    }
    else if (ctuXPosInCtus == tileXPosInCtus && wavefrontsEnabled)
    {
      // Synchronize cabac probabilities with upper-right CTU if it's available and at the start of a line.
      if (ctuTsAddr != startCtuTsAddr) // if it is the first CTU, then the entropy coder has already been reset
      {
        m_pcEntropyDecoder->resetEntropy(pcSlice);
      }
      TComDataCU *pCtuUp = pCtu->getCtuAbove();
      if ( pCtuUp && ((ctuRsAddr%frameWidthInCtus+1) < frameWidthInCtus)  )
      {
        TComDataCU *pCtuTR = pcPic->getCtu( ctuRsAddr - frameWidthInCtus + 1 );
        if ( pCtu->CUIsFromSameSliceAndTile(pCtuTR) )
        {
          // Top-right is available, so use it.
          pcSbacDecoder->loadContexts( &m_entropyCodingSyncContextState );
        }
      }
    }

#if ENC_DEC_TRACE
    g_bJustDoIt = g_bEncDecTraceEnable;
#endif

#if DECODER_PARTIAL_CONFORMANCE_CHECK != 0
    const UInt numRemainingBitsPriorToCtu=ppcSubstreams[uiSubStrm]->getNumBitsLeft();
#endif

    if ( pcSlice->getSPS()->getUseSAO() ) //SAO sample adaptive 样点自适应补偿
    {
      SAOBlkParam& saoblkParam = (pcPic->getPicSym()->getSAOBlkParam())[ctuRsAddr];
      Bool bIsSAOSliceEnabled = false;
      Bool sliceEnabled[MAX_NUM_COMPONENT];
      for(Int comp=0; comp < MAX_NUM_COMPONENT; comp++)
      {
        ComponentID compId=ComponentID(comp);
        sliceEnabled[compId] = pcSlice->getSaoEnabledFlag(toChannelType(compId)) && (comp < pcPic->getNumberValidComponents());
        if (sliceEnabled[compId])
        {
          bIsSAOSliceEnabled=true;
        }
        saoblkParam[compId].modeIdc = SAO_MODE_OFF;
      }
      if (bIsSAOSliceEnabled)
      {
        Bool leftMergeAvail = false;
        Bool aboveMergeAvail= false;

        //merge left condition
        Int rx = (ctuRsAddr % frameWidthInCtus);
        if(rx > 0)
        {
          leftMergeAvail = pcPic->getSAOMergeAvailability(ctuRsAddr, ctuRsAddr-1);
        }
        //merge up condition
        Int ry = (ctuRsAddr / frameWidthInCtus);
        if(ry > 0)
        {
          aboveMergeAvail = pcPic->getSAOMergeAvailability(ctuRsAddr, ctuRsAddr-frameWidthInCtus);
        }

		//-----------------maggie--------------
		bitsNeeded_before=m_pcEntropyDecoder->Getm_pcEntropyDecoderIf()->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();			
        pcSbacDecoder->parseSAOBlkParam( saoblkParam, sliceEnabled, leftMergeAvail, aboveMergeAvail, pcSlice->getSPS()->getBitDepths());
		bitsNeeded_after=m_pcEntropyDecoder->Getm_pcEntropyDecoderIf()->Getm_pcTDecBinIf()->getTDecBinCABAC()->Getm_bitsNeeded();		
		int idx_cur=m_pcEntropyDecoder->Getm_pcEntropyDecoderIf()->Getm_pcBitstream()->getByteLocation();
		RyanNALUEnc.ComputeSAOOffset(idx_cur,bitsNeeded_before,bitsNeeded_after);
		//----------------------------------------
	  }
    }

    m_pcCuDecoder->decodeCtu     ( pCtu, isLastCtuOfSliceSegment );	//parase a  CTU解析前后m_fifo_idx的差值就是CTU的字节数

#if DECODER_PARTIAL_CONFORMANCE_CHECK != 0
    const UInt numRemainingBitsPostCtu=ppcSubstreams[uiSubStrm]->getNumBitsLeft(); // NOTE: Does not account for changes in buffered bits in CABAC decoder, although it's probably good enough.
    if (TDecConformanceCheck::doChecking() && m_pDecConformanceCheck)
    {
      m_pDecConformanceCheck->checkCtuDecoding(numRemainingBitsPriorToCtu-numRemainingBitsPostCtu);
    }
#endif

     m_pcCuDecoder->decompressCtu ( pCtu );	//decoding process for a CU

#if ENC_DEC_TRACE
    g_bJustDoIt = g_bEncDecTraceDisable;
#endif

    //Store probabilities of second CTU in line into buffer 
    if ( ctuXPosInCtus == tileXPosInCtus+1 && wavefrontsEnabled) //存储第二个CTU的上下文
    {
    
		m_entropyCodingSyncContextState.loadContexts( pcSbacDecoder );
    }

    if (isLastCtuOfSliceSegment) //如果是该slice的最后一个CTU，则不分析剩余比特
    {
#if DECODER_CHECK_SUBSTREAM_AND_SLICE_TRAILING_BYTES
      pcSbacDecoder->parseRemainingBytes(false);
#endif
      if(!pcSlice->getDependentSliceSegmentFlag())
      {
        pcSlice->setSliceCurEndCtuTsAddr( ctuTsAddr+1 );
      }
      pcSlice->setSliceSegmentCurEndCtuTsAddr( ctuTsAddr+1 );
    }
    else if (  ctuXPosInCtus + 1 == tileXPosInCtus + currentTile.getTileWidthInCtus() &&
             ( ctuYPosInCtus + 1 == tileYPosInCtus + currentTile.getTileHeightInCtus() || wavefrontsEnabled)
            )
    {
      // The sub-stream/stream should be terminated after this CTU.
      // (end of slice-segment, end of tile, end of wavefront-CTU-row)
      UInt binVal;
      pcSbacDecoder->parseTerminatingBit( binVal );
      assert( binVal );
#if DECODER_CHECK_SUBSTREAM_AND_SLICE_TRAILING_BYTES
      pcSbacDecoder->parseRemainingBytes(true);
#endif
    }

  }


  //----------maggie---------
  RyanNALUEnc.ParseTime=(Double)(clock()-ParseBeforeTime) / CLOCKS_PER_SEC;
  //-------------------------
  assert(isLastCtuOfSliceSegment == true);


  if( depSliceSegmentsEnabled ) //depedent slice上下文模型
  {
    m_lastSliceSegmentEndContextState.loadContexts( pcSbacDecoder );//ctx end of dep.slice
  }

  //---------maggie--------------
  
	if(pcSlice->isIntra())
	{
		 //string inputpath="E:\\HEVC\\HM-16.18\\bin\\vc2012\\Win32\\Debug\\str.bin";
	 	 string inputpath="E:\\HEVC\\HM-16.18\\bin\\vc2012\\Win32\\Release\\str.bin";

		 Double ModelTime=0;
		 Double EncTime=0;
		 Double TotalEncTime=0;

		 clock_t ModelBeforeTime = clock();
		 RyanNALUEnc.ModelCalcute(pcSlice->getSliceType());
		 RyanNALUEnc.ModelTime = (Double)(clock()-ModelBeforeTime) / CLOCKS_PER_SEC;

		 clock_t AESBeforeTime = clock();
		 RyanNALUEnc.EncSlice(inputpath);
		 RyanNALUEnc.AESTime =(Double)(clock()-AESBeforeTime) / CLOCKS_PER_SEC;
		 //printf ("[ModelTime=%6.3f,EncTime=%6.3f,TotalEncTime=%6.3f] \n", ModelTime,EncTime,TotalEncTime);  
	}
	else
	{
		 //string inputpath="E:\\HEVC\\HM-16.18\\bin\\vc2012\\Win32\\Debug\\str.bin";
		 string inputpath="E:\\HEVC\\HM-16.18\\bin\\vc2012\\Win32\\Release\\str.bin";
		 Double ModelTime=0;
		 Double EncTime=0;
		 Double TotalEncTime=0;

		 clock_t ModelBeforeTime = clock();
		 RyanNALUEnc.ModelCalcute(pcSlice->getSliceType());
		 RyanNALUEnc.ModelTime = (Double)(clock()-ModelBeforeTime) / CLOCKS_PER_SEC;

		 clock_t AESBeforeTime = clock();
		 RyanNALUEnc.EncSlice(inputpath);
		 RyanNALUEnc.AESTime =(Double)(clock()-AESBeforeTime) / CLOCKS_PER_SEC;
		 //printf ("[ModelTime=%6.3f,EncTime=%6.3f,TotalEncTime=%6.3f] \n", ModelTime,EncTime,TotalEncTime);  
	}
	RyanNALUEnc.EncTime = (Double)(clock()-ParseBeforeTime) / CLOCKS_PER_SEC;

	//写入时间日志
	//string timepath="E:\\HEVC\\HM-16.18\\bin\\vc2012\\Win32\\Debug\\Time\\time.txt";
	string timepath="E:\\HEVC\\HM-16.18\\bin\\vc2012\\Win32\\Release\\Time\\HoneyBee_3840x2160_Enc4B_time.txt";
	ofstream Timefile;
	Timefile.open(timepath.c_str(),ios::app);
	Timefile<<RyanNALUEnc.ParseTime<<" "<<RyanNALUEnc.ModelTime<<" "<<RyanNALUEnc.AESTime<<" "<<RyanNALUEnc.EncTime<<"\n";
	Timefile.close();
  //--------------------------
}

//! \}
