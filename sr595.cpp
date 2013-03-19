// Copyright (c) 2012 All Right Reserved, Kirill Shklovsky
// 
// This file is part of sr595 library for AVR.

// sr595 library is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// sr595 library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with sr595 library.  If not, see <http://www.gnu.org/licenses/>.



#include "sr595.h"
sr595::sr595(
	uint8_t nCascadeCount, 
	uint8_t fParallel, 
	volatile uint8_t *ptrPort, 
	volatile uint8_t *ptrDir,
	uint8_t nOE, 
	uint8_t nInvertOE, 
	uint8_t nDS, 
	uint8_t nSHCP, 
	const uint8_t anSTCP[]
)
{
	m_nCascadeCount	= nCascadeCount;
	m_ptrPort		= ptrPort;
	m_nOE			= nOE;
	m_nInvertOE		= nInvertOE;
	m_nDS			= nDS;
	m_nSHCP			= nSHCP;
	for (int i = 0; i<nCascadeCount; i++) {
		m_anSTCP[i] = anSTCP[i];
	}
	m_fParallel 	= fParallel;
	
	// Set the port to output
	m_nPortBitMask = 0;
	m_nPortBitMask = m_nOE | m_nDS | m_nSHCP;
	for (int i=0; i<m_nCascadeCount; i++) {
		m_nPortBitMask |= (1<<m_anSTCP[i]);
	}
	*ptrDir |= m_nPortBitMask;
	// Write zeros to all port values
	*m_ptrPort &= 0x00 | (~m_nPortBitMask);
	
	// Disable output
	OE_HI();
	
	// Lower all STCP lines
	for (int i=0; i<m_nCascadeCount; i++) {
		STCP_LO(i);
	}
	
	// Assume shift registers read all zeros
	for (int i=0; i<m_nCascadeCount; i++) {
		m_anData[i]=0;
	}
	
}

void sr595::writeByte(uint8_t nIndex, uint8_t nData, uint8_t force)
{
	if (m_anData[nIndex] != nData || force) {
		m_anData[nIndex] = nData;
		for (int nByte = nIndex; nByte>=0; nByte--) {
			for (int nBit=7; nBit>=0; nBit--) {
				SHCP_LO();
				if (m_anData[nByte] & (0x01 << nBit)) {
					DS_HI();
				} else {
					DS_LO();
				}
				SHCP_HI();
			}
			if (m_fParallel) { break; }
		}
		STCP_HI(nIndex);
		SHCP_LO();
		STCP_LO(nIndex);		
	}
	
}

void sr595::forceClearAll() {
	for (uint8_t i=0;i<m_nCascadeCount;i++) {
		writeByte(i, 0x00, 1);
	}
}


void sr595::writeData(uint8_t nStartIndex, uint8_t nCount, uint8_t anData[], uint8_t nForce){
	uint8_t nSTCPmask = 0;
	uint8_t fPrevByteSent = 0;
	for (int nByte = nStartIndex + nCount - 1; nByte >=0 ; nByte--) {
		if (
				nForce 											// 1. If data write is forced, then we write
			|| (fPrevByteSent && (!m_fParallel))				// 2. If we have a serial connection, and we sent a byte already
																// 		(in serial connection we must write all subsequent bytes)
			|| m_anData[nByte] != anData[nByte-nStartIndex])	// 3. Cached value is not the same
		{
			fPrevByteSent = 1;
			m_anData[nByte] = anData[nByte-nStartIndex];
			for (int nBit=7; nBit>=0; nBit--) {
				SHCP_LO();
				if (m_anData[nByte] & (0x01 << nBit)) {
					DS_HI();
				} else {
					DS_LO();
				}
				SHCP_HI();
			}
			
			if (m_fParallel) {
				if (m_OeDisableDuringLoad) { OE_HI(); }
				STCP_HI(nByte);
				SHCP_LO();
				STCP_LO(nByte);		
				if (m_OeDisableDuringLoad) { OE_LO(); }
				if (nByte <=nStartIndex) break;
			} else {
				nSTCPmask |= (1<<m_anSTCP[nByte]);
			}
		}
	}
	if (!m_fParallel) {
		if (m_OeDisableDuringLoad) { OE_HI(); }
		*m_ptrPort |=  nSTCPmask;
		SHCP_LO();
		*m_ptrPort &=  (~nSTCPmask);
		if (m_OeDisableDuringLoad) { OE_LO(); }
	}
}