#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include "RegisterFile.h"
#include "Instructions.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "E.h"
#include "Status.h"

/* E constructor:
 * Initialize the E pipeline register.
 */
E::E()
{
   stat = new PipeRegField(SAOK);
   icode = new PipeRegField(INOP);
   ifun = new PipeRegField(FNONE);
   valC = new PipeRegField();
   valA = new PipeRegField();
   valB = new PipeRegField();
   dstE = new PipeRegField(RNONE);
   dstM = new PipeRegField(RNONE);
   srcA = new PipeRegField();
   srcB = new PipeRegField();
}

/* getstat:
 * Return the stat pipeline register field 
 */
PipeRegField * E::getstat()
{
   return stat;
}

/* geticode:
 * Return the icode pipeline register field 
 */
PipeRegField * E::geticode()
{
   return icode;
}


/* getifun:
 * Return the ifun pipeline register field 
 */
PipeRegField * E::getifun()
{
   return ifun;
}

/* getValC: 
 * Return the valC pipeline register field 
 */
PipeRegField * E::getvalC()
{
   return valC;
}

/* getvalA:
 * Return the valA pipeline register field 
 */
PipeRegField * E::getvalA()
{
   return valA;
}

/* getvalB:
 * Return the valB pipeline register field 
 */
PipeRegField * E::getvalB()
{
   return valB;
}

/* getdstE:
 * Return the dstE pipeline register field 
 */
PipeRegField * E::getdstE()
{
   return dstE;
}

/* getdstM: 
 * Return the dstM pipeline register field 
 */
PipeRegField * E::getdstM()
{
   return dstM;
}

/* getsrcA:
 * Return the srcA pipeline register field. 
 */
PipeRegField * E::getsrcA()
{
   return srcA;
}

/* getsrcB:
 * Return the srcB pipeline register field. 
 */
PipeRegField * E::getsrcB()
{
   return srcB;
}

/* dump:
 * Outputs the current values of the E pipeline register.
 */
void E::dump()
{
   dumpField("E: stat: ", 1, stat->getOutput(), false);
   dumpField(" icode: ", 1, icode->getOutput(), false);
   dumpField(" ifun: ", 1, ifun->getOutput(), false);
   dumpField(" valC: ", 16, valC->getOutput(), false);
   dumpField(" valA: ", 16, valA->getOutput(), true);
   dumpField("E: valB: ", 16, valB->getOutput(), false);
   dumpField(" dstE: ", 1, dstE->getOutput(), false);
   dumpField(" dstM: ", 1, dstM->getOutput(), false);
   dumpField(" srcA: ", 1, srcA->getOutput(), false);
   dumpField(" srcB: ", 1, srcB->getOutput(), true);
}
