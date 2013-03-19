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

#ifndef sr595_h
#define sr595_h
#include <avr/io.h>

#ifndef SR595_CASCADE_MAX
#define SR595_CASCADE_MAX	5
#endif

/*
	Todo:
		* DDR does not work
		* I have a fast serial write method I need to paste in here.
		* Need more connection types: parallel DS
		* Need compile-time connection decisions to shrink code size and improve execution speed
		* m_nInvertOE is not honored everywhere
		
	Possible connection types
		* DS line connection
			* Serial shift registers (one data line, Q7S of SRn is connected to DS of SRn+1
			* Parallel shift registers (one data line, connected to *all* DSs of all SRs)
			* Parallel shift registers: independent DS lines 
		* Assume that SHCP is always shared
		* Could have shared or independent STCP lines
		* Could have shared or independent MR and OE lines
		
	Temp_display_C
		* Separate STCP lines
		* Serial data connection
		* Shared OE lines
		
*/

class sr595
{
	public:
		sr595(
			uint8_t nCascadeCount, 
			uint8_t fParallel, 
			volatile uint8_t *ptrPort, 
			volatile uint8_t *ptrDir, 
			uint8_t nOE, 
			uint8_t nInvertOE, 
			uint8_t nDS, 
			uint8_t nSHCP, 
			const uint8_t anSTCP[]);
		void writeByte(uint8_t nIndex, uint8_t nData, uint8_t force = 0);	
		uint8_t getByte(uint8_t nIndex) { return m_anData[nIndex]; }
		void writeData(uint8_t nStartIndex, uint8_t nCount, uint8_t anData[], uint8_t nForce = 0);
		void forceWriteData(uint8_t nStartIndex, uint8_t nCount, uint8_t anData[]);	
		void forceClearAll();
	protected:
		uint8_t m_nCascadeCount;
		volatile uint8_t *m_ptrPort;
		uint8_t m_OeDisableDuringLoad;
		uint8_t m_nOE;
		uint8_t m_nInvertOE;
		uint8_t m_nDS;
		uint8_t m_nSHCP;
		uint8_t m_anSTCP[SR595_CASCADE_MAX];
		uint8_t m_anData[SR595_CASCADE_MAX];
		uint8_t m_nPortBitMask;
		uint8_t m_fParallel;
		uint8_t m_fOutput;
		void SHCP_LO() 						{	*m_ptrPort &= ~(1<<m_nSHCP);				}
		void SHCP_HI() 						{	*m_ptrPort |=   1<<m_nSHCP;					}
		void DS_LO() 						{	*m_ptrPort &= ~(1<<m_nDS);					}
		void DS_HI() 						{	*m_ptrPort |=   1<<m_nDS;					}
		void STCP_LO(uint8_t nIndex) 		{	*m_ptrPort &= ~(1<<m_anSTCP[nIndex]);		}
		void STCP_HI(uint8_t nIndex) 		{	*m_ptrPort |=   1<<m_anSTCP[nIndex];		}
		void OE_LO() 						{	*m_ptrPort &= ~(1<<m_nOE);					}
		void OE_HI() 						{	*m_ptrPort |=   1<<m_nOE;					}
	public:
		void setOutput(uint8_t nOutput) {
			if (nOutput != m_fOutput) {
				if ( (m_fOutput = nOutput) && m_nInvertOE) {				/* Assign AND TEST in the first conjunct */
					OE_HI();
				} else {
					OE_LO();
				}
			}
		}
		void toggleOutput() {
			if ( (m_fOutput = (m_fOutput != 0)) ^ 1) {	/* Assign AND TEST */
				OE_LO();
			} else {
				OE_HI();
			}
		}
		uint8_t getOeDisableDuringLoad() { return m_OeDisableDuringLoad; }
		void	setOeDisableDuringLoad(uint8_t nNewVal) { m_OeDisableDuringLoad = nNewVal; }
		
};
#endif /* sr595_h */
