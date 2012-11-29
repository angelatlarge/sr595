#ifndef sr595_h
#define sr595_h
#include <avr/io.h>

#define SR595_CASCADE_MAX	5
class sr595
{
	public:
		sr595(uint8_t nCascadeCount, uint8_t fParallel, volatile uint8_t *ptrPort, volatile uint8_t *ptrDir, uint8_t nOE, uint8_t nInvertOE, uint8_t nDS, uint8_t nSHCP, uint8_t anSTCP[]);
		void writeByte(uint8_t nIndex, uint8_t nData);	
		void writeData(uint8_t nStartIndex, uint8_t nCount, uint8_t anData[]);	
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
