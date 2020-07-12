#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "W.h"
#include "Stage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"

/* doClockLow:
 * Performs the Writeback stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    //Add code to the WritebackStage::doClockLow method to grab the icode value out of the W register.  
    //If the icode is equal to HALT then the doClockLow method should return true.  
    //Otherwise, the doClockLow method should return false.
   W * wreg = (W *)pregs[WREG];
   uint64_t stat = wreg->getstat()->getOutput();
   if(stat != SAOK)
   {
       return true;
   }
   return false;
}


/* doClockHigh:
 * Applies the appropriate control signals to W
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void WritebackStage::doClockHigh(PipeReg ** pregs)
{
    writeRegister((W *)pregs[WREG]);
}

/* writeRegister:
 * Writes to registers
 *
 * @params wreg - instance of W pointer
 */
void WritebackStage::writeRegister(W * wreg)
{
    bool error = false;
    RegisterFile * rf = RegisterFile::getInstance();
    rf->writeRegister(wreg->getvalE()->getOutput(), wreg->getdstE()->getOutput(), error);
    rf->writeRegister(wreg->getvalM()->getOutput(), wreg->getdstM()->getOutput(), error);
}
