/*
 * Names: Dustin Pernell & Sam Howard
 * Team: 18
 */
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <ctype.h>

#include "Loader.h"
#include "Memory.h"
#include "Tools.h"


//first column in file is assumed to be 0
#define ADDRBEGIN 2   //starting column of 3 digit hex address 
#define ADDREND 4     //ending column of 3 digit hext address
#define DATABEGIN 7   //starting column of data bytes
#define COMMENT 28    //location of the '|' character 

/* Loader constructor:
 * Opens the .yo file named in the command line arguments, reads the contents of the file
 * line by line and loads the program into memory.  If no file is given or the file doesn't
 * exist or the file doesn't end with a .yo extension or the .yo file contains errors then
 * loaded is set to false.  Otherwise loaded is set to true.
 *
 * @param argc is the number of command line arguments passed to the main; should
 *        be 2
 * @param argv[0] is the name of the executable
 *        argv[1] is the name of the .yo file
 */
Loader::Loader(int argc, char * argv[])
{
   loaded = false;
   Memory * mem = Memory::getInstance();

   //Start by writing a method that opens the file (checks whether it ends 
   //with a .yo and whether the file successfully opens; if not, return without 
   //loading)
   if (checkFile(argv[1])) {

        //The file handle is declared in Loader.h.  You should use that and
        //not declare another one in this file.
        
        //Next write a simple loop that reads the file line by line and prints it out
        std::string instru;
        int lineNum = 1;
        int64_t preAddr = -1;
        bool hadError = false;
        while (inf.peek() != EOF && !hadError) {
             getline(inf, instru);  

             bool error = false;

            //Next, add a method that will write the data in the line to memory 
            //(call that from within your loop)
            if(!checkError(instru))
            {
                int64_t addr = convert(instru, ADDRBEGIN, ADDREND, error);
                uint8_t encode[15];
                bool hasValue = false;
                int valSize = getEncode(instru, encode, DATABEGIN, COMMENT, error, hasValue);
            
                if((addr >= preAddr) && addr >= 0 && !error){
                    if(hasValue){
                        mem->putByteArray(encode, valSize, addr, error);
                        preAddr=preAddr+valSize;
                    }
                }
                else if(addr != -3){
                    error = true;
                }

            }
            else
            {
                error = true;
            }
            
            //Finally, add code to check for errors in the input line.
            //When your code finds an error, you need to print an error message and return.
            //Since your output has to be identical to your instructor's, use this cout to print the
            //error message.  Change the variable names if you use different ones.
            if(error)
            {
                hadError = true;
                std::cout << "Error on line " << std::dec << lineNum
                   << ": " << instru << std::endl;
            }
            lineNum++;
        }

        //If control reaches here then no error was found and the program
        //was loaded into memory.
        loaded = !hadError;
    }
  
}

/* isLoaded:
 * returns the value of the loaded data member; loaded is set by the constructor
 *
 * @return value of loaded (true or false)
 */
bool Loader::isLoaded()
{
   return loaded;
}

/* checkFile:
 * Checks to see if the file used is a .yo
 * @params: argv[] - array of chars
 */
bool Loader::checkFile(char argv[]) {
    bool returnval;
    int argsize = strlen(argv);
    
    if (argsize > 3 
        && (argv[argsize - 3] == '.' 
        && (argv[argsize - 2] == 'y' || argv[argsize - 2] == 'Y') 
        && (argv[argsize - 1] == 'o' || argv[argsize - 1] == 'O'))) {
        inf.open (argv, std::ifstream::in);
        if (inf.good()) {
            returnval = true;
        } else {
            returnval = false;
        }
    } else {
        returnval = false;
    }
    return returnval;
}

/* convert:
 * Convert from a string to chars
 * 
 * @params: line - the actual String 
 * @params: addrb - address read
 * @params: addrend - end of address
 * @params: error - bool determining error
 */
int64_t Loader::convert(std::string line, int addrb, int addrend, bool& error) {
   if(!(findSpace(line, addrb, addrend) <= addrb)){
        std::string str = line.substr(addrb, addrend);
        uint32_t addr = std::stoul(str, nullptr, 16);
        return addr;
   }
   else if(!(findSpace(line, addrb, addrend) <= addrb)){
       error = true;
       return -2;
   }
   else{
       return -3;
   }
}

/* getEncode:
 * Returns the encoding of the full instruction
 *
 * @params: line - full line read
 * @params: byte[] - the byte array
 * @params: datab - data being read
 * @params: comment - the comment at the line read
 * @params: error - bool determining an error
 * @params: hasValue - a bool value
 */
int Loader::getEncode(std::string line, uint8_t byte[], int datab, int comment, bool& error, bool&hasValue) {
    int place = 0;
    if(!(findSpace(line, datab, comment-1) <= datab)){
        for(int i = datab; i < comment; place++){
            std::string str;
            str = line.substr(i, 2);
            bool isHex = false;
            checkHex(str, 0, 1, isHex);
            if(!isHex && str.at(0) != ' ')
            {
                uint8_t encode = std::stoul(str, nullptr, 16);
                byte[place] = encode;
                hasValue = true;
            }
            i = i + 2;
        }
        //return place - 1;
        return getSize(line.c_str());
    }
    else if(!(findSpace(line, datab, comment-1) <= datab)){
        error = true;
    }
    return 0;
}

/* findSpace:
 * find all spaces in the line read
 *
 * @params: line - line read
 * @params: start - start of the line read
 * @params: end -  end of the line read
 */
int Loader::findSpace(std::string line, int start, int end)
{
    int place = start;
    while(line.at(place) != 0x20 && place <= end)
    {
        place++;
    }
    return place;
}

/* getSize:
 * Returns the size of the encoding
 *
 * @params: encode - the encoding of the instruction
 */
int Loader::getSize(std::string encode)
{
    int size = 0;
    while(encode.at(size + DATABEGIN) != ' ')
    {
        size++;
    }
    return size / 2;
}

/* checkError:
 * Return a bool of whether what was read is valid
 *
 * @param: line - line read
 */
bool Loader::checkError(std::string line)
{
    bool hasError = false;
    if(checkBlank(line))
     {
        checkHex(line, ADDRBEGIN, ADDREND, hasError);
        checkHex(line, DATABEGIN, COMMENT-1, hasError);
    
        checkSpecial(line, hasError);
        
        checkAddress(line, hasError);
            
        checkValBytes(line, hasError);
      }
      else if(line.at(COMMENT) != '|')
      {
          hasError = true;
      }
    return hasError;
}

/* checkBlank:
 * Check spaces at line with no comment
 *
 * @params: line - line read
 */
bool Loader::checkBlank(std::string line)
{
    bool check = false;
    for(int i = 0; i < COMMENT; i++)
    {
        if(line.at(i) != ' ')
        {
            check = true;
        }
    }
    return check;
}

/* checkHex:
 * checks to see if the hex values are in range.
 *
 * @params: line - the line read
 * @params: start - the start of the line read
 * @params: end - end of the line read
 * @params: error - bool determining there is an error
 */
void Loader::checkHex(std::string line, int start, int end, bool& error){
    for( int i = start; i <= end; i++){
        char hex = line.at(i);
        if (!((hex >= 0x30 && hex <= 0x39)
            || (hex >= 0x41 && hex <= 0x46)
            || (hex >= 0x61 && hex <= 0x66)
            || (hex == 0x20))) {
               error = true;
            }
    }
}

/* checkSpecial:
 * Checks for special characters used
 *
 * @params: line - line read
 * @params: error - bool determining an error
 */
void Loader::checkSpecial(std::string line, bool& error)
{
    if(line.at(5) != ':'||
        line.at(6) != ' ' ||
        line.at(COMMENT) != '|')
    {
        error = true;
    }

}

/* checkValBytes:
 * Check the bytes within the range
 *
 * @params: line - line read
 * @params: error - bool determining if there is an error
 */
void Loader::checkValBytes(std::string line, bool& error)
{
    int charCount = 0;
    bool hasSpace = false;
    for(int i = DATABEGIN; i < COMMENT; i++)
    {
        if(hasSpace && line.at(i) != ' ')
        {
            error = true;
        }
        else if(line.at(i) == ' ')
        {
            hasSpace = true;
        }
        else
        {
            charCount++;
        }
    }
    if(charCount > 0 && (charCount % 2) != 0)
    {
        error = true;
    }
}

/* checkAddress:
 * Gets the address at the line in the range
 *
 * @params: line - line read
 * @params: error - bool determining if there is an error
 */
void Loader::checkAddress(std::string line, bool& error)
{
    if(line.at(0) != '0' || line.at(1) != 'x')
    {
        error = true;
    }
    else if(line.at(2) == ' ' ||        
            line.at(3) == ' ' ||
            line.at(4) == ' ')
    {
        error = true;
    }
}

