#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "ExecuteStage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"

/*
 * doClockLow:
 * Performs the Decode stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   ExecuteStage * es = (ExecuteStage *) stages[ESTAGE];
   MemoryStage * ms = (MemoryStage *) stages[MSTAGE];
   RegisterFile * rf = RegisterFile::getInstance();

   bool error = false;

   uint64_t  icode = (dreg->geticode())->getOutput(), ifun = (dreg->getifun)()->getOutput(), 
             valC = (dreg->getvalC())->getOutput(), stat = (dreg->getstat())->getOutput(),
             rA = (dreg->getrA())->getOutput(), rB = (dreg->getrB())->getOutput(), valP = dreg->getvalP()->getOutput(),
             valA = 0, valB = 0, dstE = RNONE, dstM = RNONE, srcA = RNONE, srcB = RNONE;
   
   srcA = getd_srcA(icode, rA); 
   srcB = getd_srcB(icode, rB);
   dstE = getd_dstE(icode, rB);
   dstM = getd_dstM(icode, rA);
   valA = getd_valA(icode, valP, 
                    mreg->getdstM()->getOutput(), ms->getm_valM(), wreg->getdstM()->getOutput(), 
                    wreg->getvalM()->getOutput(), srcA, es->gete_dstE(), mreg->getdstE()->getOutput(), 
                    wreg->getdstE()->getOutput(), es->gete_valE(), mreg->getvalE()->getOutput(), 
                    wreg->getvalE()->getOutput(), rf->readRegister(srcA, error));

   valB = getd_valB(mreg->getdstM()->getOutput(), ms->getm_valM(), wreg->getdstM()->getOutput(), 
                    wreg->getvalM()->getOutput(), srcB, es->gete_dstE(), mreg->getdstE()->getOutput(), 
                    wreg->getdstE()->getOutput(), es->gete_valE(), mreg->getvalE()->getOutput(), 
                    wreg->getvalE()->getOutput(), rf->readRegister(srcB, error));

   E_bubble = getbubble(ereg->geticode()->getOutput(), es->gete_Cnd(), ereg->getdstM()->getOutput(), srcA, srcB);
   d_srcA = srcA;
   d_srcB = srcB;
  
   //provide the input values for the E register
   setEInput(ereg, stat, icode, ifun, valC, valA, valB, dstE, dstM, srcA, srcB);
   return false;
}

/* doClockHigh:
 * Applies the appropriate control signal to the D.
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void DecodeStage::doClockHigh(PipeReg ** pregs)
{
    if(E_bubble)
    {
        E * ereg = (E *) pregs[EREG];

        ereg->getstat()->bubble(SAOK);
        ereg->geticode()->bubble(INOP);
        ereg->getifun()->bubble(FNONE);
        ereg->getvalC()->bubble();
        ereg->getvalA()->bubble();
        ereg->getvalB()->bubble();
        ereg->getdstE()->bubble(RNONE);
        ereg->getdstM()->bubble(RNONE);
        ereg->getsrcA()->bubble(RNONE);
        ereg->getsrcB()->bubble(RNONE);
    }
    else
    {
        E * ereg = (E *) pregs[EREG];

        ereg->getstat()->normal();
        ereg->geticode()->normal();
        ereg->getifun()->normal();
        ereg->getvalC()->normal();
        ereg->getvalA()->normal();
        ereg->getvalB()->normal();
        ereg->getdstE()->normal();         
        ereg->getdstM()->normal();
        ereg->getsrcA()->normal();
        ereg->getsrcB()->normal();
    }
}

/* setEInput:
 * Provides the input to potentially be stored in the E register
 * during doClockHigh.
 *
 * @param: ereg - pointer to the E register instance
 * @param: stat - value to be stored in the stat pipeline register within E
 * @param: icode - value to be stored in the icode pipeline register within E
 * @param: ifun - value to be stored in the ifun pipeline register within E
 * @param: valC - value to be stored in the valC pipeline register within E
 * @param: valA - value to be stored in the valA pipeline register within E
 * @param: valB - value to be stored in the valB pipeline register within E
 * @param: dstE - value to be stored in the dstE pipeline register within E
 * @param: dstM - value to be stored in the dstM pipeline register within E
 * @param: srcA - value to be stored in the srcA pipeline register within E
 * @param: srcB - value to be stored in the srcB pipeline register within E

*/
void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t  icode, 
                           uint64_t ifun, uint64_t valC, uint64_t valA,
                           uint64_t valB, uint64_t dstE, uint64_t dstM,
                           uint64_t srcA, uint64_t srcB)
{
   ereg->getstat()->setInput(stat);
   ereg->geticode()->setInput(icode);
   ereg->getifun()->setInput(ifun);
   ereg->getvalA()->setInput(valA);
   ereg->getvalC()->setInput(valC);
   ereg->getvalB()->setInput(valB);
   ereg->getdstE()->setInput(dstE);
   ereg->getdstM()->setInput(dstM);
   ereg->getsrcA()->setInput(srcA);
   ereg->getsrcB()->setInput(srcB);
}
/* getd_srcA:
 * Need to pass in a D.C pointer to all of these 
 * Use to get rA, rB etc.
 *
 * @params d_icode - icode of instruction in DecodeStage
 * @params rA - rA value of the instruction
 */
uint64_t DecodeStage::getd_srcA(uint64_t d_icode, uint64_t rA)
{
    if (d_icode == IRRMOVQ || d_icode == IRMMOVQ || 
        d_icode == IOPQ || d_icode == IPUSHQ)
    {
        return rA;
    }
    else if (d_icode == IPOPQ || d_icode == IRET)
    {
        return RSP;
    }
    else
    {
        return RNONE;
    }
}

/* getd_srcB:
 * Returns the value of rB based on the instructions icode.
 *
 * @params d_icode - icode of instruction in DecodeStage
 * @params rB - rB value of the instruction
 */
uint64_t DecodeStage::getd_srcB(uint64_t d_icode, uint64_t rB)
{
    if (d_icode == IOPQ || d_icode == IRMMOVQ || d_icode == IMRMOVQ)
    {
        return rB;
    }
    else if (d_icode == IPUSHQ || d_icode == IPOPQ || 
        d_icode == ICALL || d_icode == IRET)
    {
        return RSP;
    }
    else 
    {
        return RNONE;
    }
}

/* getd_dstE:
 * Returns either rB or RSP based on the instructions icode.
 *
 * @params: d_icode - icode of the instruction in DecodeStage
 * @params: rB - rB value of the instruction in DecodeStage
 */
uint64_t DecodeStage::getd_dstE(uint64_t d_icode, uint64_t rB)
{
    if (d_icode == IRRMOVQ || d_icode == IIRMOVQ || d_icode == IOPQ)
    {
        return rB;
    }
    else if (d_icode == IPUSHQ || d_icode == IPOPQ || 
        d_icode == ICALL || d_icode == IRET)
    {
        return RSP;
    }
    else 
    {
        return RNONE;
    }
}

/* getd_valA:
 * Returns the value of rA based on the type of instruction.
 *
 * @params: d_icode - icode of the instruction in DecodeStage
 * @params: rA - rA value of the instruction in DecodeStage
 */
uint64_t DecodeStage::getd_dstM(uint64_t d_icode, uint64_t rA)
{
    if (d_icode == IMRMOVQ || d_icode == IPOPQ)
    {
        return rA;
    }
    else
    {
        return RNONE;
    }
}

/* getd_valA:
 * Completes forwarding for valA.
 *
 * @params: d_icode - the icode of the instruction in DecodeStage
 * @params: d_valP - the address of the next sequential instruction
 * @params: m_dstM - value to be stored in m_dstM
 * @params: m_valM - value to be stored in m_valM
 * @params: w_dstM - value to be stored in w_dstM
 * @params: w_valM - value to be stored in w_valM
 * @params: d_srcA - value to be stored in d_srcA
 * @params: e_dstE - value to be stored in e_dstE
 * @params: m_dstE - value to be stored in m_dstE
 * @params: w_dstE - value to be stored in w_dstE
 * @params: e_valE - value to be stored in e_valE
 * @params: m_valE - value to be stored in m_valE
 * @params: w_valE - value to be stored in w_valE
 * @params: d_rvalA - value to be stored in d_rvalA
 */
uint64_t DecodeStage::getd_valA(uint64_t d_icode, uint64_t d_valP, 
    uint64_t m_dstM, uint64_t m_valM, uint64_t w_dstM, uint64_t w_valM, 
    uint64_t d_srcA, uint64_t e_dstE, uint64_t m_dstE, uint64_t w_dstE, 
    uint64_t e_valE, uint64_t m_valE, uint64_t w_valE, uint64_t d_rvalA)
{
    if (d_icode == ICALL || d_icode == IJXX)
    {
        return d_valP;
    }
    if (d_srcA == RNONE)
    {
        return 0;
    }
    if (d_srcA == e_dstE) 
    {
        return e_valE;
    }
    if (d_srcA == m_dstM)
    {
        return m_valM;
    }
    if (d_srcA == m_dstE) 
    {
        return m_valE;
    }
    if (d_srcA == w_dstM) 
    {
        return w_valM;
    }
    if (d_srcA == w_dstE)
    {
        return w_valE;
    }
    return d_rvalA;
}

/* getd_valB:
 * Completes forwarding for d_valB
 *
 * @params: m_dstM - value of dstM
 * @params: m_valM - value of m_valM
 * @params: w_dstM - value of w_dstM
 * @params: w_valM - value of w_valM
 * @params: d_srcB - value of d_srcB
 * @params: e_dstE - value of e_dstE
 * @params: m_dstE - value of m_dstE
 * @params: w_dstE - value of w_dstE
 * @params: e_valE - value of e_valE
 * @params: m_valE - value of m_valE
 * @params: w_valE - value of w_valE
 * @params: d_rvalB - value of d_rvalB
 */
uint64_t DecodeStage::getd_valB(uint64_t m_dstM, uint64_t m_valM, 
    uint64_t w_dstM, uint64_t w_valM, uint64_t d_srcB, uint64_t e_dstE, 
    uint64_t m_dstE, uint64_t w_dstE, uint64_t e_valE, 
    uint64_t m_valE, uint64_t w_valE, uint64_t d_rvalB)
{
    if (d_srcB == RNONE)
    {
        return 0;
    }
    if (d_srcB == e_dstE)
    {
        return e_valE;
    }
    if (d_srcB == m_dstM) 
    {
        return m_valM;
    }
    if (d_srcB == m_dstE)
    {
        return m_valE;
    }
    if (d_srcB == w_dstM)
    {
        return w_valM;
    }
    if (d_srcB == w_dstE)
    {
        return w_valE;
    }
    return d_rvalB;
}

/* getd_srcA:
 * Returns the value of d_srcA.
 */
uint64_t DecodeStage::getd_srcA() 
{
    return d_srcA;
}

/* getd_srcB:
 * Returns the value of d_srcB
 */
uint64_t DecodeStage::getd_srcB()
{
    return d_srcB;
}

/* getBubble:
 * Returns a bool based on whether the instruction requires bubbling.
 *
 * @params: e_icode - value of e_icode 
 * @params: e_CND - A bool of whether the instruction's condition codes are met
 * @params: e_dstM - value of e_dstM
 * @params: d_srcA - value of srcA
 * @params: d_srcB - value of srcB
 */
bool DecodeStage::getbubble(uint64_t e_icode, uint64_t e_Cnd, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB)
{
    if ((e_icode == IJXX && !e_Cnd) ||
        ((e_icode == IMRMOVQ || e_icode == IPOPQ) && (e_dstM == d_srcA || e_dstM == d_srcB)))
    {
        return true;
    }
    return false;
}

/* getE_bubble:
 * Returns a bool of whether the E stage will need bubbling
 */
bool DecodeStage::getE_bubble()
{
    return E_bubble;
}
