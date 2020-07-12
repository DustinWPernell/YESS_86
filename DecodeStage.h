//class to perform the combinational logic of
//the Decode stage
class DecodeStage: public Stage
{
   private:
      uint64_t d_srcA;
      uint64_t d_srcB;
      bool E_bubble;
      void setEInput(E * ereg, uint64_t stat, uint64_t icode, uint64_t  ifun, 
                     uint64_t valC, uint64_t valA, uint64_t valB,
                     uint64_t dstE, uint64_t dstM, uint64_t srcA,
                     uint64_t  srcB);
      uint64_t getd_srcA(uint64_t d_icode, uint64_t rA);
      uint64_t getd_srcB(uint64_t d_icode, uint64_t rB);
      uint64_t getd_dstE(uint64_t d_icode, uint64_t rB);
      uint64_t getd_dstM(uint64_t d_icode, uint64_t rA);
      uint64_t getd_valA(uint64_t d_icode, uint64_t d_valP, uint64_t m_dstM, uint64_t m_valM, uint64_t w_dstM, uint64_t w_valM, uint64_t d_srcA, uint64_t e_dstE, uint64_t m_dstE, uint64_t w_dstE, uint64_t e_valE, uint64_t m_valE, uint64_t w_valE, uint64_t d_rvalA);
      uint64_t getd_valB(uint64_t m_dstM, uint64_t m_valM, uint64_t w_dstM, uint64_t w_valM, uint64_t d_srcB, uint64_t e_dstE, uint64_t m_dstE, uint64_t w_dstE, uint64_t e_valE, uint64_t m_valE, uint64_t w_valE, uint64_t d_rvalB);
      bool getbubble(uint64_t e_icode, uint64_t e_Cnd, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      uint64_t getd_srcA();
      uint64_t getd_srcB();
      bool getE_bubble();
};
