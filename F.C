#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstddef>
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"


/* F constructor
 * Initialize the F pipeline register. 
 */
F::F()
{
   predPC = new PipeRegField();
}

/* getpredPC:
 * Return the predPC pipeline register field.
 */
PipeRegField * F::getpredPC()
{
   return predPC;
}

/* dump:
 * Outputs the current values of the F pipeline register.
 */
void F::dump()
{
   dumpField("F: predPC: ", 3, predPC->getOutput(), true);
}
