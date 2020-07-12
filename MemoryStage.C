#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"


/* doClockLow:
 * Performs the Memory stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   
   //Added valA = mreg->getvalA()->etc.
   uint64_t icode = mreg->geticode()->getOutput(), valE = mreg->getvalE()->getOutput(), 
            dstE = mreg->getdstE()->getOutput(), dstM = mreg->getdstM()->getOutput(), 
            stat = mreg->getstat()->getOutput(), valA = mreg->getvalA()->getOutput(),
            valM = 0;

   Memory * mem = Memory::getInstance();
   bool error = false;
   uint64_t memAddr = addr(icode, valE, valA); 
   if(memRead(icode))
   {
       valM = mem->getLong(memAddr, error);

   }
   if(memWrite(icode))
   {
       mem->putLong(valA, memAddr, error);
   }

   if(error)
   {
       stat = SADR;
   }
  
   m_valM = valM;
   m_stat = stat;
    
   //provide the input values  for the W register
   setWInput(wreg, stat, icode, valE, valM, dstE, dstM);
   return false;
}

/* doClockHigh:
 * applies the appropriate control signal to the M
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void MemoryStage::doClockHigh(PipeReg ** pregs)
{
   W * wreg = (W *) pregs[WREG];
   
   wreg->getstat()->normal();
   wreg->geticode()->normal();
   wreg->getvalE()->normal();
   wreg->getvalM()->normal();
   wreg->getdstE()->normal();
   wreg->getdstM()->normal();
}

/* setWInput:
 * provides the input to potentially be stored in the W register
 * during doClockHigh
 *
 * @param: wreg - pointer to the W register instance
 * @param: stat - value to be stored in the stat pipeline register within W
 * @param: icode - value to be stored in the icode pipeline register within W
 * @param: valE - value to be stored in the valE pipeline register within W
 * @param: valM - value to be stored in the valM pipeline register within W
 * @param: dstE - value to be stored in the dstE pipeline register within W
 * @param: dstM - value to be stored in the dstM pipeline register within W
*/
void MemoryStage::setWInput(W * wreg, uint64_t stat, uint64_t icode, 
                           uint64_t valE, uint64_t valM,
                           uint64_t dstE, uint64_t dstM)
{
   wreg->getstat()->setInput(stat);
   wreg->geticode()->setInput(icode);
   wreg->getvalE()->setInput(valE);
   wreg->getvalM()->setInput(valM);
   wreg->getdstE()->setInput(dstE);
   wreg->getdstM()->setInput(dstM);
}

/* addr:
 * Returns the address in m_valE or m_valA
 *
 * @params: m_icode - icode of instruction
 * @params: m_valE - valE of instruction
 * @params: m_valA - value of m_valA
 */ 
uint64_t MemoryStage::addr(uint64_t m_icode, uint64_t m_valE, uint64_t m_valA)
{
    if (m_icode == IRMMOVQ || m_icode == IPUSHQ || 
        m_icode == ICALL || m_icode == IMRMOVQ)
    {
        return m_valE;
    }
    if (m_icode == IPOPQ || m_icode == IRET)
    {
        return m_valA;
    }
    return 0;
}

/* memRead:
 * Returns a bool of whether the address reads from memory
 *
 * @params: m_icode - icode of the instruction
 */
bool MemoryStage::memRead(uint64_t m_icode)
{
    if (m_icode == IMRMOVQ || m_icode == IPOPQ || m_icode == IRET)
    {
        return true;
    }
    return false;
}

/* memWrite:
 * Returns a bool of whether the instruction reads from memory
 *
 * @params: m_icode - icode of the instruction
 */
bool MemoryStage::memWrite(uint64_t m_icode)
{ 
    if (m_icode == IRMMOVQ || m_icode == IPUSHQ || m_icode == ICALL) 
    {
        return true;
    }
    return false;
}

/* getm_valM:
 * Returns m_valM
 */
uint64_t MemoryStage::getm_valM()
{
   return m_valM;
}

/* getm_stat:
 * Returns m_stat
 */
uint64_t MemoryStage::getm_stat()
{
    return m_stat;
}
