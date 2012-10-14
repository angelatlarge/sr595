#ifndef sr595_h
#define sr595_h
#include <avr/io.h>
class sr595
{
	public:
		sr595(uint8_t nCascadeCount, uint8_t fParallel, volatile uint8_t *ptrPort, volatile uint8_t *ptrDir, uint8_t nOE, uint8_t nDS, uint8_t nSHCP, uint8_t anSTCP[]);
		~sr595();
		void writeByte(uint8_t nIndex, uint8_t nData);	
		void writeData(uint8_t nStartIndex, uint8_t nCount, uint8_t anData[]);	
	protected:
		uint8_t m_nCascadeCount;
		volatile uint8_t *m_ptrPort;
		uint8_t m_nOE;
		uint8_t m_nDS;
		uint8_t m_nSHCP;
		uint8_t *m_anSTCP;
		uint8_t *m_anData;
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
				if (m_fOutput = nOutput) {				/* Assign AND TEST */
					OE_LO();
				} else {
					OE_HI();
				}
			}
		}
		void toggleOutput() {
			if (m_fOutput = (m_fOutput != 0) ^ 1) {	
				OE_LO();
			} else {
				OE_HI();
			}
		}
		
};
#endif sr595_h