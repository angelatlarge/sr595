#include "sr595.h"

sr595::sr595(uint8_t nCascadeCount, uint8_t fParallel, volatile uint8_t *ptrPort, volatile uint8_t *ptrDir, uint8_t nOE, uint8_t nInvertOE, uint8_t nDS, uint8_t nSHCP, uint8_t anSTCP[])
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
	
/*	
#define DDRC 
#define PORTC _SFR_IO8(0x15)

#define PORTD _SFR_IO8(0x12)
#define DDRD _SFR_IO8(0x11)

#define DDRB _SFR_IO8(0x17)
*/	
	// Assume shift registers read all zeros
	for (int i=0; i<m_nCascadeCount; i++) {
		m_anData[i]=0;
	}
	
}

void sr595::writeByte(uint8_t nIndex, uint8_t nData)
{
	if (m_anData[nIndex] != nData) {
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

void sr595::writeData(uint8_t nStartIndex, uint8_t nCount, uint8_t anData[])
{
	uint8_t fSentEarlier = 0;
	int8_t nHighestLoaded = -1;
	uint8_t nSTCPmask = 0;
	for (int nByte = nStartIndex + nCount - 1; nByte >=0 ; nByte--) {
		if (fSentEarlier || (m_anData[nByte] != anData[nByte-nStartIndex])) {
			fSentEarlier = 1;
			if (nByte > nHighestLoaded) { nHighestLoaded = nByte; }
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
		//~ for (int nByte = nHighestLoaded; nByte >=nStartIndex ; nByte--) {
			//~ STCP_HI(nByte);
		//~ }		
		SHCP_LO();
		*m_ptrPort &=  (~nSTCPmask);
		//~ for (int nByte = nHighestLoaded; nByte >=nStartIndex ; nByte--) {
			//~ STCP_LO(nByte);
		//~ }		
		if (m_OeDisableDuringLoad) { OE_LO(); }
	}
}
