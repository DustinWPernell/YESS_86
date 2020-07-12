  
//class to perform the combinational logic of
//the Memory stage
class MemoryStage: public Stage
{
   private:
      uint64_t m_valM;
      uint64_t m_stat;
      void setWInput(W * wreg, uint64_t stat, uint64_t icode, uint64_t valE, 
                     uint64_t valM, uint64_t dstE,
                     uint64_t dstM);
      uint64_t addr(uint64_t m_icode, uint64_t m_valE, uint64_t m_valA);
      bool memRead(uint64_t m_icode);
      bool memWrite(uint64_t m_icode);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getm_valM();
      uint64_t getm_stat();
};
