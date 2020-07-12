CC = g++
CFLAGS = -g -c -Wall -std=c++11 -Og
OBJLAB6 = yess.o Simulate.o Loader.o Memory.o Tools.o RegisterFile.o ConditionCodes.o PipeRegField.o PipeReg.o 
OBJYESS = F.o D.o E.o M.o W.o FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o WritebackStage.o

LAB6FILES = RegisterFile.h ConditionCodes.h Memory.h
STAGES = FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h F.h D.h E.h M.h W.h
STNDPIPEREG = RegisterFile.h Instructions.h PipeRegField.h PipeReg.h Status.h
STNDSTAGE = RegisterFile.h PipeRegField.h PipeReg.h Stage.h Status.h Debug.h Instructions.h


.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJLAB6) $(OBJYESS)

yess.o: Loader.h Memory.h RegisterFile.h ConditionCodes.h Debug.h PipeReg.h Stage.h Simulate.h

Simulate.o: PipeRegField.h PipeReg.h Stage.h Simulate.h $(LAB6FILES) $(STAGES)

Loader.o: Loader.h Memory.h

Memory.o: Memory.h Tools.h

Tools.o: Tools.h

RegisterFile.o: RegisterFile.h Tools.h

ConditionCodes.o: ConditionCodes.h Tools.h

PipeRegField.o: PipeRegField.h

PipeReg.o: PipeReg.h

F.o: PipeRegField.h PipeReg.h F.h

D.o: $(STNDPIPEREG) D.h

E.o: $(STNDPIPEREG) E.h

M.o: $(STNDPIPEREG) M.h

W.o: $(STNDPIPEREG) W.h

FetchStage.o: $(STNDSTAGE) F.h D.h M.h W.h FetchStage.h DecodeStage.h ExecuteStage.h Memory.h Tools.h

DecodeStage.o: $(STNDSTAGE) D.h E.h M.h W.h DecodeStage.h ExecuteStage.h MemoryStage.h

ExecuteStage.o: $(STNDSTAGE) E.h M.h W.h ExecuteStage.h MemoryStage.h

MemoryStage.o: $(STNDSTAGE) M.h W.h MemoryStage.h Memory.h

WritebackStage.o: $(STNDSTAGE) W.h WritebackStage.h

clean:
	rm $(OBJLAB6) $(OBJYESS) yess

run:
	make clean
	make yess
	./run.sh

