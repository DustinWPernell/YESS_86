//class to perform the combinational logic of
//the Writeback stage
class WritebackStage: public Stage
{
   private:
      void writeRegister(W* wreg);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);

};
