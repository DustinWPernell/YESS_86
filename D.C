#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include "Instructions.h"
#include "RegisterFile.h"
#include "PipeReg.h"
#include "PipeRegField.h"
#include "D.h"
#include "Status.h"

/* D constructor:
 * Initialize the D pipeline register.
 */
D::D()
{
   stat = new PipeRegField(SAOK);
   icode = new PipeRegField(INOP);
   ifun = new PipeRegField(FNONE);
   rA = new PipeRegField(RNONE);
   rB = new PipeRegField(RNONE);
   valC = new PipeRegField();
   valP = new PipeRegField();
}

/* getstat:
 * Return the stat pipeline register.
 *
 * @return: stat
 */
PipeRegField * D::getstat()
{
   return stat;
}

/* geticode:
 * Return the icode pipeline register. 
 *
 * @return: icode
 */
PipeRegField * D::geticode()
{
   return icode;
}

/* getifun:
 * Return the ifun pipeline register. 
 *
 * @return: ifun
 */
PipeRegField * D::getifun()
{
   return ifun;
}

/* getrA:
 * Return the rA pipeline register.
 *
 * @return: rA
 */
PipeRegField * D::getrA()
{
   return rA;
}

/* getrB:
 * Return the rB pipeline register.
 *
 * @return: rB 
 */
PipeRegField * D::getrB()
{
   return rB;
}

/* getvalC:
 * Return the valC pipeline register.
 *
 * @return: valC
 */
PipeRegField * D::getvalC()
{
   return valC;
}

/* getvalP:
 * Return the valP pipeline register.
 *
 * @return: valP
 */
PipeRegField * D::getvalP()
{
   return valP;
}

/* dump:
 * outputs the current values of the D pipeline register.
 */
void D::dump()
{
   dumpField("D: stat: ", 1, stat->getOutput(), false);
   dumpField(" icode: ", 1, icode->getOutput(), false);
   dumpField(" ifun: ", 1, ifun->getOutput(), false);
   dumpField(" rA: ", 1, rA->getOutput(), false);
   dumpField(" rB: ", 1, rB->getOutput(), false);
   dumpField(" valC: ", 16, valC->getOutput(), false);
   dumpField(" valP: ", 3, valP->getOutput(), true);
}
