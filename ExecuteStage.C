#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ConditionCodes.h"

/* doClockLow:
 * Performs the Execute stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   
   MemoryStage * ms = (MemoryStage *) stages[MSTAGE];
   W * wreg = (W *) pregs[WREG];
   
   uint64_t icode = ereg->geticode()->getOutput(), valA = ereg->getvalA()->getOutput(), stat = ereg->getstat()->getOutput(), 
            valC = ereg->getvalC()->getOutput(), dstE = ereg->getdstE()->getOutput(), dstM = ereg->getdstM()->getOutput(), 
            valB = ereg->getvalB()->getOutput(), ifun = ereg->getifun()->getOutput(),
            Cnd = 0, valE = 0;
   
   int64_t aluA = getaluA(icode, valA, valC);
   int64_t aluB = getaluB(icode, valB);
   uint64_t alufun = getalufun(icode, ifun);
   valE = doALU(alufun, aluA, aluB, set_cc(icode, ms->getm_stat(), wreg->getstat()->getOutput()));

   Cnd = docond(icode, ifun);

   dstE = getdstE(icode, Cnd, dstE);
   M_bubble = getBubble(ms->getm_stat(), wreg->getstat()->getOutput());
   
   e_Cnd = Cnd;
   e_valE = valE;
   e_dstE = dstE;
   //provide the input values for the M register
   setMInput(mreg, stat, icode, Cnd, valE, valA, dstE, dstM);
   return false;
}

/* doClockHigh:
 * Applies the appropriate control signal to the E.
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
   if(M_bubble)
   {
        M * mreg = (M *) pregs[MREG];

        mreg->getstat()->bubble(SAOK);
        mreg->geticode()->bubble(INOP);
        mreg->getCnd()->bubble();
        mreg->getvalE()->bubble();
        mreg->getvalA()->bubble();
        mreg->getdstE()->bubble(RNONE);
        mreg->getdstM()->bubble(RNONE);
   }
   else
   {
       M * mreg = (M *) pregs[MREG];
       mreg->getstat()->normal();
       mreg->geticode()->normal();
       mreg->getCnd()->normal();
       mreg->getvalE()->normal();
       mreg->getvalA()->normal();
       mreg->getdstE()->normal();
       mreg->getdstM()->normal();
   }
}

/* setMInput:
 * Provides the input to potentially be stored in the M register
 * during doClockHigh.
 *
 * @param: mreg - pointer to the M register instance
 * @param: stat - value to be stored in the stat pipeline register within M
 * @param: icode - value to be stored in the icode pipeline register within M
 * @param: Cnd - value to be stored in the Cnd pipeline register within M
 * @param: valE - value to be stored in the valE pipeline register within M
 * @param: valA - value to be stored in the valA pipeline register within M
 * @param: dstE - value to be stored in the dstE pipeline register within M
 * @param: dstM - value to be stored in the dstM pipeline register within M
*/
void ExecuteStage::setMInput(M * mreg, uint64_t stat, uint64_t icode, 
                           uint64_t Cnd, uint64_t valE, uint64_t valA,
                           uint64_t dstE, uint64_t dstM)
{
   mreg->getstat()->setInput(stat);
   mreg->geticode()->setInput(icode);
   mreg->getCnd()->setInput(Cnd);
   mreg->getvalE()->setInput(valE);
   mreg->getvalA()->setInput(valA);
   mreg->getdstE()->setInput(dstE);
   mreg->getdstM()->setInput(dstM);
}

/* getaluA: 
 * Returns the value to be computed by the ALU based on the icode
 *
 * @params: e_icode - icode of the instruction in ExecuteStage
 * @params: e_valA - value of e_valA
 * @params: e_valC - value of e_valC
 */
int64_t ExecuteStage::getaluA(uint64_t e_icode, uint64_t e_valA, uint64_t e_valC)
{
   if (e_icode == IRRMOVQ || e_icode == IOPQ)
   {
       return e_valA;
   }
   if (e_icode == IIRMOVQ || e_icode == IRMMOVQ || e_icode == IMRMOVQ)
   {
       return e_valC;
   }
   if (e_icode == ICALL || e_icode == IPUSHQ)
   {
       return -8;
   }
   if (e_icode == IRET || e_icode == IPOPQ)
   {
       return 8;
   }
   return 0;
}

/* getaluB:
 * Returns the value of the other operand used by the ALU based on the icode
 *
 * @params: e_icode - icode of the instruction in ExecuteStage
 * @params: e_valB - value of e_valB
 */
int64_t ExecuteStage::getaluB(uint64_t e_icode, uint64_t e_valB)
{
    if(e_icode == IRMMOVQ || e_icode == IMRMOVQ || e_icode == IOPQ || e_icode == ICALL ||
        e_icode == IPUSHQ || e_icode == IRET || e_icode == IPOPQ)
    {
        return e_valB;
    }
    if(e_icode == IRRMOVQ || e_icode == IIRMOVQ)
    {
        return 0;
    }
    return 0;
}

/* getalufun:
 * Returns the value of e_fun or ADDQ
 *
 * @params: e_icode - icode of the instruction in ExecuteStage
 * @params: e_ifun - ifun of the instruction in ExecuteStage
 */
int64_t ExecuteStage::getalufun(uint64_t e_icode, uint64_t e_ifun)
{
    if(e_icode == IOPQ)
    {
        return e_ifun;
    }
    return ADDQ;
}

/* set_cc:
 * Returns a bool of whether the intruction will set condition codes
 *
 * @params: e_icode - icode of instruction in ExecuteStage
 * @params: m_stat - value of m_stat
 * @params: w_stat - value of w_stat
 */
bool ExecuteStage::set_cc(uint64_t e_icode, uint64_t m_stat, uint64_t w_stat)
{
    if((e_icode == IOPQ) && 
       (m_stat != SADR && m_stat != SINS && m_stat != SHLT) &&
       (w_stat != SADR && w_stat != SINS && w_stat != SHLT))
    {
        return true;
    }
    return false;
}

/* getdstE:
 * Returns RNONE if the icode is IRRMOVQ and the condition codes are not used
 *
 * @params: e_icode - icode of the instruction in ExecuteStage
 * @params: e_CND - bool value of whether the condition codes are used
 * @params: e_dstE - value of e_dstE
 */
uint64_t ExecuteStage::getdstE(uint64_t e_icode, uint64_t e_Cnd, uint64_t e_dstE)
{
    if (e_icode == IRRMOVQ && !e_Cnd)
    {
        return RNONE;
    }
    return e_dstE;
}

/* doCC:
 * Sets the condition codes
 *
 * @params: aluA - value returned by the alu
 * @params: aluB - value returned by the alu
 * @params: valE - value of valE
 * @params: instu - name of the instruction
 */
void ExecuteStage::doCC(int64_t aluA, int64_t aluB, int64_t valE, uint64_t instru)
{
    ConditionCodes * cc = ConditionCodes::getInstance();
    bool error = false;
    
    cc->setConditionCode(false, ZF ,error);
    cc->setConditionCode(false, SF ,error);
    cc->setConditionCode(false, OF ,error);

    if (valE == 0)
    {
        cc->setConditionCode(true, ZF ,error);
    }

    if (valE < 0)
    {
        cc->setConditionCode(true, SF ,error);
    }

    if (((aluA > 0 && aluB > 0 && valE < 0) || 
        (aluA < 0 && aluB < 0 && valE >= 0)) &&
        (instru == ADDQ || instru == ANDQ))
    {
        cc->setConditionCode(true, OF ,error);
    }

    if (((aluA > 0 && aluB < 0 && valE > 0) ||
        (aluA < 0 && aluB > 0 && valE < 0)) &&
        (instru == SUBQ))
    {
        cc->setConditionCode(true, OF ,error);
    }
}

/* doALU: 
 * Make the calculations of the ALU
 *
 * @params: alufun - ifun of instruction
 * @params: aluA - the opperand of rA
 * @params: aluB - the opperand of rB
 * @params: setCC - bool to set condition codes
 */
int64_t ExecuteStage::doALU(uint64_t alufun, int64_t aluA, int64_t aluB, bool setCC)
{
    if (alufun == ADDQ)
    {
        int64_t valE = 0;
        valE = aluA + aluB;
        if(setCC) doCC(aluA, aluB, valE, ADDQ);
        return valE;
    }
    if (alufun == SUBQ)
    {
        int64_t valE = 0;
        valE = aluB - aluA;
        if(setCC) doCC(aluA, aluB, valE, SUBQ);
        return valE;
    }
    if (alufun == ANDQ)
    {
        int64_t valE = 0;
        valE = aluA & aluB;
        if(setCC) doCC(aluA, aluB, valE, ANDQ);
        return valE;
    }
    if (alufun == XORQ)
    {
        int64_t valE = 0;
        valE = aluA ^ aluB;
        if (setCC) doCC(aluA, aluB, valE, XORQ);
        return valE;
    }
    return 0;
}

/* gete_dste:
 * Returns the value of e_dstE
 */
uint64_t ExecuteStage::gete_dstE()
{
    return e_dstE;
}

/* e_valE
 * Returns the value of e_valE
 */
uint64_t ExecuteStage::gete_valE()
{
    return e_valE;
}

/* gete_Cnd:
 * Returns the value of e_Cnd
 */
uint64_t ExecuteStage::gete_Cnd()
{
    return e_Cnd;
}

/* docond:
 * Returns the value of the value to set condition codes
 *
 * @params: icode - icode of the instruction
 * @params: ifun - ifun of the instruction
 */
uint64_t ExecuteStage::docond(uint64_t icode, uint64_t ifun)
{
    ConditionCodes * cc = ConditionCodes::getInstance();
    bool error = false;

    bool ccOF = cc->getConditionCode(OF, error);
    bool ccZF = cc->getConditionCode(ZF, error);
    bool ccSF = cc->getConditionCode(SF, error);

    if (icode == IJXX || icode == ICMOVXX)
    {
        if (ifun ==  UNCOND)
        {
            return 1;
        }
        if (ifun == LESSEQ)
        {
            return ((ccSF && !ccOF) || (!ccSF && ccOF) || ccZF);
        }
        if (ifun == LESS)
        {
            return ((ccSF && !ccOF) || (!ccSF && ccOF));
        }
        if (ifun == EQUAL)
        {
            return ccZF;
        }
        if (ifun == NOTEQUAL)
        {
            return !ccZF;
        }
        if (ifun == GREATEREQ)
        {
            return !((ccSF && !ccOF) || (!ccSF && ccOF));
        }
        if (ifun == GREATER)
        {
            return (!((ccSF && !ccOF) || (!ccSF && ccOF)) && !ccZF);
        }
    }
    return 0;
}

/* getBubble:
 * Returns whether or not the instruction requires bubbling
 *
 * @params: m_stat - value of m_stat
 * @params: w_stat - value of w_stat
 */
bool ExecuteStage::getBubble(uint64_t m_stat, uint64_t w_stat)
{
    if ((m_stat == SADR || m_stat == SINS || m_stat == SHLT) ||
       (w_stat == SADR || w_stat == SINS || w_stat == SHLT))
    {
        return true;
    }
    return false;
}

bool ExecuteStage::getM_Bubble()
{
    return M_bubble;
}
