#include <string>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "FetchStage.h"
#include "DecodeStage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"
#include "Tools.h"


/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   Memory * mem = Memory::getInstance();
   DecodeStage * ds = (DecodeStage *) stages[DSTAGE];
   ExecuteStage * es = (ExecuteStage *) stages[ESTAGE];

   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   E * ereg = (E *) pregs[EREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   
   uint64_t f_pc = 0, icode = INOP, ifun = FNONE, valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE, stat = SAOK;

   //Code missing here to select the value of the PC
   //and fetch the instruction from memory.
   //Fetching the instruction will allow the icode, ifun,
   //rA, rB, and valC to be set.
   //The lab assignment describes what methods need to be
   //written.
   f_pc = selectPC(freg->getpredPC()->getOutput(), mreg->getCnd()->getOutput(),
                 mreg->getvalA()->getOutput(), wreg->getvalM()->getOutput(),
                 mreg->geticode()->getOutput(), wreg->geticode()->getOutput());


   bool error = false;

   uint64_t instru = mem->getByte(f_pc, error);
   if (error)
   {
        stat = SADR;
        icode = INOP;
        ifun = FNONE;
   }

   icode = Tools::getBits(instru, 4, 7);
   ifun = Tools::getBits(instru, 0, 3);
       
   stat = getStat(icode, error);
   bool needC = need_valC(icode);
   bool needReg = need_regids(icode);

   if (needC) {
        buildValC(f_pc, needReg, valC, error);
   }

   if (needReg) {
        getRegIds(f_pc, rA, rB, error);
   }

   valP = PCIncrement(f_pc, needC, needReg);
   uint64_t f_predPC = PredictPC(icode, valC, valP);
 
   freg->getpredPC()->setInput(f_predPC);
   D_bubble = getbubble(dreg->geticode()->getOutput(), ereg->geticode()->getOutput(), mreg->geticode()->getOutput(), es->gete_Cnd(), ereg->getdstM()->getOutput(), ds->getd_srcA(), ds->getd_srcB());
   F_stall = setf_stall(dreg->geticode()->getOutput(), ereg->geticode()->getOutput(), mreg->geticode()->getOutput(), ereg->getdstM()->getOutput(), ds->getd_srcA(), ds->getd_srcB());
   D_stall = setd_stall(ereg->geticode()->getOutput(), ereg->getdstM()->getOutput(), ds->getd_srcA(), ds->getd_srcB());

   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);

   if (debug) std::cout << std::hex << icode << std::endl;
   return false;
}

/* doClockHigh:
 * Applies the appropriate control signal to the F
 * and D register intances.
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   if (!F_stall)
   {
        freg->getpredPC()->normal();
   }
   
   if (D_bubble)
   {
        dreg->getstat()->bubble(SAOK);
        dreg->geticode()->bubble(INOP);
        dreg->getifun()->bubble();
        dreg->getrA()->bubble(RNONE);
        dreg->getrB()->bubble(RNONE);
        dreg->getvalC()->bubble();
        dreg->getvalP()->bubble();
   }
   else if (!D_stall)
   {
        dreg->getstat()->normal();
        dreg->geticode()->normal();
        dreg->getifun()->normal();
        dreg->getrA()->normal();
        dreg->getrB()->normal();
        dreg->getvalC()->normal();
        dreg->getvalP()->normal();
    }
}

/* setDInput:
 * Provides the input to potentially be stored in the D register
 * during doClockHigh.
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
    dreg->getstat()->setInput(stat);
    dreg->geticode()->setInput(icode);
    dreg->getifun()->setInput(ifun);
    dreg->getrA()->setInput(rA);
    dreg->getrB()->setInput(rB);
    dreg->getvalC()->setInput(valC);
    dreg->getvalP()->setInput(valP);
}


uint64_t FetchStage::selectPC(uint64_t F_predPC, uint64_t M_Cnd, uint64_t M_valA,
                          uint64_t W_valM, uint64_t M_icode, uint64_t W_icode)
{
   if(M_icode == IJXX && !M_Cnd)
   {
       return M_valA;
   }
   if(W_icode == IRET)
   {    
       return W_valM;
   }
   return F_predPC;
}

/* need_redids:
 * Returns a bool depending  on whether the instuction uses registers.
 *
 * @param: f_icode - the icode of the instruction in FetchStage
 * @return: bool
 */
bool FetchStage::need_regids(uint64_t f_icode)
{
    if(f_icode == IRRMOVQ || f_icode == IOPQ || f_icode == IPUSHQ || f_icode == IPOPQ ||
       f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ)
    {
        return true;
    }
    return false;
}

/* need_valC:
 * Returns a bool based on whther the instuction has a valC value.
 *
 * @params: f_icode - the icode of the instruction in FetchStage
 * @return: bool
 */
bool FetchStage::need_valC(uint64_t f_icode)
{
    if(f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ || f_icode == IJXX || f_icode == ICALL)
    {
        return true;
    }
    return false;
}

/* PredictPC:
 * Returns the address of the next instruction based the current instructions icode.
 *
 * @params: f_icode - the icode of the instuction in FetchStage
 * @params: f_valC - the valC of the instruction in FetchStage
 * @params: f_valP - the address of the next sequential instruction if the icode is not IJXX or ICALL
 * @return: f_valC
 * @return: f_valP
 */
uint64_t FetchStage::PredictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP)
{
    if(f_icode == IJXX || f_icode == ICALL)
    {
        return f_valC;
    }
    return f_valP;
}

/* PCIncrement:
 * Returns the incremented PC based on the size of the instruction.
 *
 * @params: f_pc - Program Counter
 * @params: needC - bool of needC
 */
uint64_t FetchStage::PCIncrement(uint64_t f_pc, bool needC, bool needReg)
{
    if(needC)
    {
        //8 bytes
        f_pc = f_pc + BYTE8;
    }
    
    if(needReg)
    {
        //1 bytes
        f_pc = f_pc + BYTE1;
    }
    
    f_pc = f_pc + BYTE1;
    return f_pc;
}

void FetchStage::getRegIds(uint64_t f_pc, uint64_t &rA, uint64_t &rB, bool & error) {
    Memory * mem = Memory::getInstance();
    uint64_t instru = mem->getByte(f_pc + BYTE1, error);

    rA = Tools::getBits(instru, 4, 7);
    rB = Tools::getBits(instru, 0, 3);
}

void FetchStage::buildValC(uint64_t f_pc, bool regIDs, uint64_t &valC, bool & error) {
    Memory * mem = Memory::getInstance();
    uint8_t byte[8];
    uint64_t start = f_pc + 1;
    
    if (regIDs)
    {
        start += 1;
    }

    for (int i = 0; i < 8; i++) {
        byte[i] = mem->getByte(start + i, error);
    }

    valC = Tools::buildLong(byte);
}

uint64_t FetchStage::getStat(uint64_t icode, bool &error)
{
    if (!(icode == INOP || icode == IHALT || icode == IRRMOVQ || icode == IIRMOVQ || icode == IRMMOVQ
         || icode == IMRMOVQ || icode == IOPQ || icode == IJXX || icode == ICALL || icode == IRET
         || icode == IPUSHQ || icode == IPOPQ))
    {
        error = true;
        return SINS;
    }
    
    if (icode == IHALT)
    {
        return SHLT;
    }
    return SAOK;
}

bool FetchStage::setf_stall(uint64_t d_icode, uint64_t e_icode, uint64_t m_icode, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB)
{
    if (((e_icode == IMRMOVQ || e_icode == IPOPQ) && 
        (e_dstM == d_srcA || e_dstM == d_srcB)) || 
       (d_icode == IRET || e_icode == IRET || m_icode == IRET))
    {
        return true;
    }
    return false;
}

bool FetchStage::setd_stall(uint64_t e_icode, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB)
{
    if ((e_icode == IMRMOVQ || e_icode == IPOPQ) && (e_dstM == d_srcA || e_dstM == d_srcB))
    {
        return true;
    }
    return false;
}

bool FetchStage::getbubble(uint64_t d_icode, uint64_t e_icode, uint64_t m_icode, uint64_t e_Cnd, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB)
{
    if ((e_icode == IJXX && !e_Cnd) || 
        ((!((e_icode == IMRMOVQ || e_icode == IPOPQ) && 
           (e_dstM == d_srcA || e_dstM == d_srcB))) && 
         (d_icode == IRET || e_icode == IRET || m_icode == IRET)))
    {
        return true;
    }
    return false;
}

bool FetchStage::getD_bubble()
{
    return D_bubble;
}
