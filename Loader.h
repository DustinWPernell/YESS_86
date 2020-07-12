
class Loader
{
   private:
      bool loaded;        //set to true if a file is successfully loaded into memory
      bool checkFile(char argv[]);
      int64_t convert(std::string line, int addrb, int addrend, bool & error);
      int getEncode(std::string line, uint8_t byte[], int datab, int comment, bool & error, bool& hasValue);
      int getSize(std::string encode);
      int findSpace(std::string line, int start, int end);
      std::string getString(std::string line, int start, int end);
      bool checkError(std::string line);
      bool checkBlank(std::string line);
      void checkHex(std::string line, int start, int end, bool& error);
      void checkSpecial(std::string line, bool& error);
      void checkValBytes(std::string line, bool& error);
      void checkAddress(std::string line, bool& error);
      std::ifstream inf;  //input file handle
   public:
      Loader(int argc, char * argv[]);
      bool isLoaded();
};
