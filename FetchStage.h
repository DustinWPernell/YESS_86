//class to perform the combinational logic of
//the Fetch stage
//
#define BYTE8 8
#define BYTE1 1

class FetchStage: public Stage
{
   private:
      bool F_stall;
      bool D_stall;
      bool D_bubble;
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      uint64_t selectPC(uint64_t f_predPC, uint64_t M_Cnd, uint64_t M_valA, 
                     uint64_t W_valM, uint64_t Micode, 
                     uint64_t W_icode);
      bool need_regids(uint64_t f_icode);
      bool need_valC(uint64_t f_icode);
      uint64_t PredictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
      uint64_t PCIncrement(uint64_t f_pc, bool needC, bool needReg);
      void getRegIds(uint64_t f_pc, uint64_t &rA, uint64_t &rB, bool & error);
      void buildValC(uint64_t f_pc, bool regIds, uint64_t &valC, bool & error);
      uint64_t getStat(uint64_t icode, bool &error);
      bool setf_stall(uint64_t d_icode, uint64_t e_icode, uint64_t m_icode, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB);
      bool setd_stall(uint64_t e_icode, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB);
      bool getbubble(uint64_t d_icode, uint64_t e_icode, uint64_t m_icode, uint64_t e_Cnd, uint64_t e_dstM, uint64_t d_srcA, uint64_t d_srcB);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);
      bool getD_bubble();
};
