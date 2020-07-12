//class to perform the combinational logic of
//the Execute stage
class ExecuteStage: public Stage
{
   private:
      uint64_t e_dstE;
      uint64_t e_valE;
      uint64_t e_Cnd;
      bool M_bubble;
      void setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd, 
                     uint64_t valE, uint64_t valA,
                     uint64_t dstE, uint64_t dstM);
      int64_t getaluA(uint64_t e_icode, uint64_t e_valA, uint64_t e_valC);
      int64_t getaluB(uint64_t e_icode, uint64_t e_valB);
      int64_t getalufun(uint64_t e_icode, uint64_t e_ifun);
      bool set_cc(uint64_t e_icode, uint64_t m_stat, uint64_t w_stat);
      uint64_t getdstE(uint64_t e_icode, uint64_t e_Cnd, uint64_t e_dstE);
      void doCC(int64_t aluA, int64_t aluB, int64_t valA, uint64_t instru);
      int64_t doALU(uint64_t alufun, int64_t aluA, int64_t aluB, bool setCC);
      uint64_t docond(uint64_t icode, uint64_t ifun);
      bool getBubble(uint64_t m_stat, uint64_t w_stat);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t gete_dstE();
      uint64_t gete_valE();
      uint64_t gete_Cnd();
      bool getM_Bubble();
};
