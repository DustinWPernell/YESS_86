#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include "RegisterFile.h"
#include "Instructions.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "Status.h"
/* M constructor:
 * Initialize the M pipeline register.
 */
M::M()
{
   stat = new PipeRegField(SAOK);
   icode = new PipeRegField(INOP);
   Cnd = new PipeRegField();
   valE = new PipeRegField();
   valA = new PipeRegField();
   dstE = new PipeRegField(RNONE);
   dstM = new PipeRegField(RNONE);
}

/* getstat:
 * Return the stat pipeline register field.
 */
PipeRegField * M::getstat()
{
   return stat;
}

/* geticode:
 * Return the icode pipeline register field. 
 */
PipeRegField * M::geticode()
{
   return icode;
}

/* getCnd:
 * Return the Cnd pipeline register field.
 */
PipeRegField * M::getCnd()
{
   return Cnd;
}

/* getvalE:
 * Return the valE pipeline register field.
 */
PipeRegField * M::getvalE()
{
   return valE;
}

/* getvalA:
 * Return the valA pipeline register field. 
 */
PipeRegField * M::getvalA()
{
   return valA;
}

/* getdstE:
 * Return the dstE pipeline register field. 
 */
PipeRegField * M::getdstE()
{
   return dstE;
}

/* getdstM:
 * Return the dstM pipeline register field. 
 */
PipeRegField * M::getdstM()
{
   return dstM;
}

/* dump:
 * Outputs the current values of the M pipeline register.
 */
void M:: dump()
{
   dumpField("M: stat: ", 1, stat->getOutput(), false);
   dumpField(" icode: ", 1, icode->getOutput(), false);
   dumpField(" Cnd: ", 1, Cnd->getOutput(), false);
   dumpField(" valE: ", 16, valE->getOutput(), false);
   dumpField(" valA: ", 16, valA->getOutput(), false);
   dumpField(" dstE: ", 1, dstE->getOutput(), false);
   dumpField(" dstM: ", 1, dstM->getOutput(), true);
}
