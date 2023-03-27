#include <filesystem>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

class Word_t {
public:
  uint32_t addr;
  uint32_t value;

  Word_t(const uint32_t inAddr, const uint32_t inValue) : addr(inAddr), value(inValue) {}
};

class Inst_t {
protected:
  bool reachable;
  uint32_t addr;
  uint32_t mPlaceId;
  uint32_t mTransitionId;

  void printReg(const uint8_t regNum) {
    if (regNum < 13)
      printf("r%d", regNum);
    else
      switch (regNum) {
      case 13:
        printf("sp");
        break;
      case 14:
        printf("lr");
        break;
      case 15:
        printf("pc");
      }
  }

public:
  Inst_t(const uint32_t inAddr) : reachable(false), addr(inAddr), mPlaceId(0), mTransitionId(0) {}
  static Inst_t *decodeThumb(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeARM32(const uint32_t inAddr, const uint32_t inCode);

  void setReachable(const bool inReachable) { reachable = inReachable; }
  bool isReachable() { return reachable; }
  uint32_t address() { return addr; }
  virtual const char *guard() { return ""; }
  void setPlaceId(const uint32_t inPlaceId) { mPlaceId = inPlaceId; }
  uint32_t placeId() { return mPlaceId; }
  void setTransitionId(const uint32_t inTransitionId) { mTransitionId = inTransitionId; }
  uint32_t transitionId() { return mTransitionId; }
  virtual void setTransitionIdTaken(const uint32_t inTransitionId) {}
  virtual uint32_t transitionIdTaken() { return 0; }
  virtual void setTargetIdTaken(const uint32_t inTargetId) {}
  virtual uint32_t targetIdTaken() { return 0; }

  virtual uint32_t branchAddress() { return 0; }
  virtual bool isFuncCall() { return false; }
  virtual bool isFuncReturn() { return false; }
  virtual void romeoFuncContent(){};
  virtual bool isLDRPC() { return false; }
  virtual uint32_t targetWord() { return 0; }
  virtual bool isCondBranch() { return false; }
  virtual bool isUncondBranch() { return false; }
  virtual void setImmByPC(const uint32_t inImm) {}
  virtual void Print() = 0;
  virtual uint8_t memAccessCount() { return 0; }

  void romeoFunc() {
    printf("int inst%x(core_t &core, mem_t &mem) { // ", addr);
    Print();
    printf("\n");
    romeoFuncContent();
    printf("  return cacheAccess(core.ICache, %d);\n", addr);
    printf("}\n\n");
  }
  void wReg(uint8_t reg) { printf("  core.regs.r[%d] = ", reg); }
  void pReg(uint8_t reg) { printf("core.regs.r[%d]", reg); }
  void updateSR(uint8_t reg) { printf("  updateSR(core.regs, core.regs.r[%d]);", reg); }
  void updateSRVal(const uint32_t val) { printf("  updateSR(core.regs, %d);", val); }
  void updateSRExp(const char *val) { printf("  updateSR(core.regs, %s);", val); }

private:
  static Inst_t *decodeThumb0(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeThumb1(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeThumb2(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeThumb3(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeThumb4(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeThumb5(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeThumb6(const uint32_t inAddr, const uint16_t inCode);
  static Inst_t *decodeThumb7(const uint32_t inAddr, const uint16_t inCode);

  static Inst_t *decodeDataProcessing(const uint32_t inAddr, const uint32_t inCode);
};

/* Decode 0 */

class MOVS_t : public Inst_t {
  uint8_t sReg, dReg;

public:
  MOVS_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    sReg = (inCode >> 3) & 0b111;
    dReg = inCode & 0b111;
  }
  virtual void Print() { printf("%x: movs r%d, r%d", addr, dReg, sReg); }
  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(sReg);
    printf(";\n");
    updateSR(dReg);
    printf("\n");
  };
};

class LSL_t : public Inst_t {
  uint8_t sReg, dReg, imm5;

public:
  LSL_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    sReg = (inCode >> 3) & 0b111;
    dReg = inCode & 0b111;
    imm5 = (inCode >> 6) & 0b11111;
  }
  virtual void Print() { printf("%x: lsl r%d, r%d, #%d", addr, dReg, sReg, imm5); }
  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(sReg);
    printf(" << %d;\n", imm5);
    //    updateSR(dReg);
  };
};

class LSR_t : public Inst_t {
  uint8_t sReg, dReg, imm5;

public:
  LSR_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    sReg = (inCode >> 3) & 0b111;
    dReg = inCode & 0b111;
    imm5 = (inCode >> 6) & 0b11111;
  }
  virtual void Print() { printf("%x: lsr r%d, r%d, #%d", addr, dReg, sReg, imm5); }
  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(sReg);
    printf(" >> %d;\n", imm5);
    //    updateSR(dReg);
  };
};

class ASR_t : public Inst_t {
  uint8_t sReg, dReg, imm5;

public:
  ASR_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    sReg = (inCode >> 3) & 0b111;
    dReg = inCode & 0b111;
    imm5 = (inCode >> 6) & 0b11111;
  }
  virtual void Print() { printf("%x: asr r%d, r%d, #%d", addr, dReg, sReg, imm5); }
  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(sReg);
    printf(" >> %d;\n", imm5);
    //    updateSR(dReg);
  };
};

class SUBR_t : public Inst_t {
  uint8_t sReg, dReg, imm5;

public:
  SUBR_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    sReg = (inCode >> 3) & 0b111;
    dReg = inCode & 0b111;
    imm5 = (inCode >> 6) & 0b11111;
  }
  virtual void Print() { printf("%x: sub r%d, r%d, #%d", addr, dReg, sReg, imm5); }
};

class ADDR_t : public Inst_t {
  uint8_t sReg, dReg, imm3;

public:
  ADDR_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    sReg = (inCode >> 3) & 0b111;
    dReg = inCode & 0b111;
    imm3 = (inCode >> 6) & 0b111;
  }

  virtual void Print() { printf("%x: add r%d, r%d, #%d", addr, dReg, sReg, imm3); }

  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(sReg);
    printf(" + %d;\n", imm3);
  };
};

Inst_t *Inst_t::decodeThumb0(const uint32_t inAddr, const uint16_t inCode) {
  const uint16_t opCode = (inCode >> 11) & 0b11;
  const uint16_t imm5 = (inCode >> 6) & 0b11111;
  switch (opCode) {
  case 0:
    if (imm5 == 0)
      return new MOVS_t(inAddr, inCode);
    else
      return new LSL_t(inAddr, inCode);
    break;
  case 1:
    return new LSR_t(inAddr, inCode);
    break;
  case 2:
    return new ASR_t(inAddr, inCode);
    break;
  case 3:
    if (inCode & 0b1000000000)
      return new SUBR_t(inAddr, inCode);
    else
      return new ADDR_t(inAddr, inCode);
    break;
  }
  return NULL;
}

/* Decode 1 */

class MOV_t : public Inst_t {
  uint8_t dReg, imm8;

public:
  MOV_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = (inCode >> 8) & 0b111;
    imm8 = inCode & 0b11111111;
  }
  virtual void Print() { printf("%x: movs r%d, #%d", addr, dReg, imm8); }
  virtual void romeoFuncContent() {
    wReg(dReg);
    printf("%d;\n", imm8);
    updateSR(dReg);
    printf("\n");
  };
};

class CMP_t : public Inst_t {
  uint8_t dReg, imm8;

public:
  CMP_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = (inCode >> 8) & 0b111;
    imm8 = inCode & 0b11111111;
  }

  virtual void Print() { printf("%x: cmp r%d, #%d", addr, dReg, imm8); }

  virtual void romeoFuncContent() {
    printf("  uint32_t val = ");
    pReg(dReg);
    printf(" - ");
    printf("%d;\n", imm8);
    updateSRExp("val");
    printf("\n");
  };
};

class ADD_t : public Inst_t {
  uint8_t dReg, imm8;

public:
  ADD_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = (inCode >> 8) & 0b111;
    imm8 = inCode & 0b11111111;
  }
  virtual void Print() { printf("%x: adds r%d, #%d", addr, dReg, imm8); }

  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(dReg);
    printf(" + %d;\n", imm8);
    updateSR(dReg);
    printf("\n");
  };
};

class SUB_t : public Inst_t {
  uint8_t dReg, imm8;

public:
  SUB_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = (inCode >> 8) & 0b111;
    imm8 = inCode & 0b11111111;
  }

  virtual void Print() { printf("%x: subs r%d, #%d", addr, dReg, imm8); }

  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(dReg);
    printf(" - %d;\n", imm8);
    updateSR(dReg);
    printf("\n");
  };
};

Inst_t *Inst_t::decodeThumb1(const uint32_t inAddr, const uint16_t inCode) {
  const uint16_t opCode = (inCode >> 11) & 0b11;
  switch (opCode) {
  case 0:
    return new MOV_t(inAddr, inCode);
    break;
  case 1:
    return new CMP_t(inAddr, inCode);
    break;
  case 2:
    return new ADD_t(inAddr, inCode);
    break;
  case 3:
    return new SUB_t(inAddr, inCode);
    break;
  }
  return NULL;
}

/* Decode 2 */

class LDRPC_t : public Inst_t {
  uint8_t dReg, imm8;
  uint32_t immByPC;

public:
  LDRPC_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = (inCode >> 8) & 0b111;
    imm8 = inCode & 0b11111111;
    immByPC = 0;
  }
  virtual bool isLDRPC() { return true; }
  virtual uint32_t targetWord() { return addr + 2 + imm8 * 4; }
  virtual void Print() { printf("%x: ldr r%d, [pc, #%d]", addr, dReg, imm8 << 2); }
  virtual void setImmByPC(const uint32_t inImm) { immByPC = inImm; }
  virtual uint8_t memAccessCount() { return 1; }
  virtual void romeoFuncContent() {
    wReg(dReg);
    printf("%d;\n", immByPC);
  }
};

class RSB_t : public Inst_t {
  uint8_t dReg, sReg;

public:
  RSB_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = inCode & 0b111;
    sReg = (inCode >> 3) & 0b111;
  }
  virtual void Print() { printf("%x: negs r%d, r%d", addr, dReg, sReg); }
  virtual void romeoFuncContent() {
    wReg(dReg);
    printf("0 - ");
    pReg(sReg);
    printf(";\n");
    updateSR(dReg);
    printf("\n");
  };
};

class CMPR_t : public Inst_t {
  uint8_t dReg, sReg;

public:
  CMPR_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = inCode & 0b111;
    sReg = (inCode >> 3) & 0b111;
  }

  virtual void Print() { printf("%x: cmp r%d, r%d", addr, dReg, sReg); }

  virtual void romeoFuncContent() {
    printf("  uint32_t val = ");
    pReg(dReg);
    printf(" - ");
    pReg(sReg);
    printf(";\n");
    updateSRExp("val");
    printf("\n");
  };
};

class BX_t : public Inst_t {
  uint8_t reg;
  uint32_t targetId;

public:
  BX_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr), targetId(0) { reg = (inCode >> 3) & 0b1111; }
  virtual void Print() {
    printf("%x: bx ", addr);
    printReg(reg);
  }
  virtual void setTargetIdTaken(const uint32_t inTargetId) { targetId = inTargetId; }
  virtual uint32_t targetIdTaken() { return targetId; }
  virtual bool isFuncReturn() { return reg == 14; }
};

class BLX_t : public Inst_t {
  uint8_t reg;

public:
  BLX_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) { reg = (inCode >> 3) & 0b1111; }
  virtual void Print() {
    printf("%x: blx ", addr);
    printReg(reg);
  }
};

class SDPADD_t : public Inst_t {
  uint8_t dReg, sReg, Rdn;

public:
  SDPADD_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    Rdn = inCode & 0b111;
    dReg = ((inCode >> 4) & 0b1000) | (inCode & 0b111); // DN:Rdn
    sReg = (inCode >> 3) & 0b1111; // Rm
  }
  virtual void Print() {
    printf("%x: add ", addr);
    printReg(Rdn);
    printf(", ");
    printReg(sReg);
  }
  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(dReg);
    printf(" + ");
    pReg(sReg);
    printf(";\n");
    updateSR(dReg);
    printf("\n");
  };
};

class SDPMOV_t : public Inst_t {
  uint8_t dReg, sReg;

public:
  SDPMOV_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    dReg = ((inCode >> 4) & 0b1000) | (inCode & 0b111);
    sReg = (inCode >> 3) & 0b1111;
  }
  virtual void Print() {
    printf("%x: mov ", addr);
    printReg(dReg);
    printf(", ");
    printReg(sReg);
  }
  virtual void romeoFuncContent() {
    if (dReg != sReg) {
      wReg(dReg);
      pReg(sReg);
      printf(";\n");
    }
  };
};

Inst_t *Inst_t::decodeThumb2(const uint32_t inAddr, const uint16_t inCode) {
  const uint16_t primOpCode = (inCode >> 11) & 0b11;
  uint16_t secondOpCode;
  switch (primOpCode) {
  case 0:
    if (inCode & (1 << 10)) {
      secondOpCode = (inCode >> 8) & 0b11;
      switch (secondOpCode) {
      case 0:
      	return new SDPADD_t(inAddr, inCode);
      	break;
      case 2: 
        return new SDPMOV_t(inAddr, inCode);
        break;
      case 3:
        if (inCode & (1 << 7))
          return new BLX_t(inAddr, inCode);
        else
          return new BX_t(inAddr, inCode);
        break;
      }
      printf("Unsupported special data processing inst: %d\n", secondOpCode);
      printf("Dealing with instruction @%d, code %d (primOpCode: %d, secondOpCode: %d) \n", inAddr, inCode, primOpCode, secondOpCode);
    } else {
      secondOpCode = (inCode >> 6) & 0b1111;
      switch (secondOpCode) {
      case 9:
        return new RSB_t(inAddr, inCode);
        break;
      case 10:
        return new CMPR_t(inAddr, inCode);
      default:
        printf("Unsupported data processing inst: %d\n", secondOpCode);
      }
    }
    break;
  case 1:
    return new LDRPC_t(inAddr, inCode);
    break;
    // case 2:
    //   break;
    // case 3:
    //   break;
  }
  printf("Unsupported instruction bits 12-11: %d\n", primOpCode);
  printf("Instruction %b @ |0x%.8x|", inCode, inAddr);
  return NULL;
}

/* Decode 5 */

class ADDTOPC_t : public Inst_t {
  uint8_t dReg, imm8;

public:
  ADDTOPC_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm8 = inCode & 0b11111111;
    dReg = (inCode >> 8) & 0b111;
  }
  virtual void Print() { printf("%x: adr r%d, pc, #%d", addr, dReg, imm8); }
};

class ADDTOSP_t : public Inst_t {
  uint8_t dReg, imm8;

public:
  ADDTOSP_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm8 = inCode & 0b11111111;
    dReg = (inCode >> 8) & 0b111;
  }

  virtual void Print() { printf("%x: add r%d, sp, #%d", addr, dReg, imm8); }

  virtual void romeoFuncContent() {
    wReg(dReg);
    pReg(13);
    printf(" + %d;\n", imm8);
  };
};

class SUBSP_t : public Inst_t {
  uint16_t imm7;

public:
  SUBSP_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm7 = (inCode & 0b1111111) << 2;
  }
  virtual void Print() { printf("%x: sub sp, #%d", addr, imm7); }
  
  virtual void romeoFuncContent() {
    wReg(13);
    pReg(13);
    printf(" - %d;\n", imm7);
    updateSR(13);
    printf("\n");
  };
};

class ADDSP_t : public Inst_t {
  uint16_t imm7;

public:
  ADDSP_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm7 = (inCode & 0b1111111) << 2;
  }
  virtual void Print() { printf("%x: add sp, #%d", addr, imm7); }
  
  virtual void romeoFuncContent() {
    wReg(13);
    pReg(13);
    printf(" + %d;\n", imm7);
    updateSR(13);
    printf("\n");
  };
};

class PUSHLIST_t : public Inst_t {
  uint16_t sRegList;

public:
  PUSHLIST_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    sRegList = (inCode & 0b11111111) | ((inCode & 0b100000000) << 6);
  }
  virtual void Print() {
    printf("%x: push {", addr);
    uint16_t rn = 0, rl = sRegList;
    bool first = true;
    while (rl != 0) {
      if (rl & 1) {
        if (first)
          first = false;
        else
          printf(", ");
        printReg(rn);
      }
      rl >>= 1;
      rn++;
    }
    printf("}");
  }

  virtual uint8_t memAccessCount() {
    uint16_t accessCount = 0, rl = sRegList;
    while (rl != 0) {
      if (rl & 1) {
        accessCount++;
      }
      rl >>= 1;
    }
    return accessCount;
  }

  virtual void romeoFuncContent() {
    uint16_t regList = sRegList;
    uint8_t regNum = 0;
    uint8_t regCount = 0;
    while (regList != 0) {
      if (regList & 1) {
        regCount++;
        printf("  memWrite(mem, ");
        pReg(13);
        printf(" - %d, ", regCount * 4);
        pReg(regNum);
        printf(");\n");
      }
      regList >>= 1;
      regNum++;
    }
    wReg(13);
    pReg(13);
    printf(" - %d;\n", regCount * 4);
  };
};

class POPLIST_t : public Inst_t {
  uint16_t dRegList;
  uint32_t mTargetIdTaken;

public:
  POPLIST_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr), mTargetIdTaken(0) {
    dRegList = (inCode & 0b11111111) | ((inCode & 0b100000000) << 7);
  }
  virtual void Print() {
    printf("%x: pop {", addr);
    uint16_t rn = 0, rl = dRegList;
    bool first = true;
    while (rl != 0) {
      if (rl & 1) {
        if (first)
          first = false;
        else
          printf(", ");
        printReg(rn);
      }
      rl >>= 1;
      rn++;
    }
    printf("}");
  }

  virtual uint8_t memAccessCount() {
    uint16_t accessCount = 0, rl = dRegList;
    while (rl != 0) {
      if (rl & 1) {
        accessCount++;
      }
      rl >>= 1;
    }
    return accessCount;
  }

  virtual void romeoFuncContent() {
    uint16_t regList = dRegList;
    uint8_t regNum = 15;
    uint8_t regCount = 0;
    while (regList != 0) {
      if (regList & (1 << 15)) {
        wReg(regNum);
        printf("memRead(mem, ");
        pReg(13);
        printf(" + %d);\n", regCount * 4);
        regCount++;
      }
      regList <<= 1;
      regNum--;
    }
    wReg(13);
    pReg(13);
    printf(" + %d;\n", regCount * 4);
  };
  virtual bool isFuncReturn() { return (dRegList & (1 << 15)) != 0; }
  virtual void setTargetIdTaken(const uint32_t inTargetId) {
    if ((dRegList & (1 << 15)) != 0)
      mTargetIdTaken = inTargetId;
  }
  virtual uint32_t targetIdTaken() {
    if ((dRegList & (1 << 15)) != 0)
      return mTargetIdTaken;
    else
      return 0;
  }
};

class NOP_t : public Inst_t {

public:
  NOP_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
  }
  virtual void Print() { printf("%x: nop", addr); }
  
  /*virtual void romeoFuncContent() {
  };*/
};

Inst_t *Inst_t::decodeThumb5(const uint32_t inAddr, const uint16_t inCode) {
  if (inCode & (1 << 12)) {
    uint16_t codop = (inCode >> 8) & 0b1111;
    switch (codop) {
    case 0b0000: 
        if (inCode & (1 << 7)) {
            return new SUBSP_t(inAddr, inCode);
        }
        else {
            return new ADDSP_t(inAddr, inCode);
        }
    case 0b0100:
    case 0b0101:
      return new PUSHLIST_t(inAddr, inCode);
      break;
    case 0b1100:
    case 0b1101:
      return new POPLIST_t(inAddr, inCode);
      break;
    case 0b1111:
      switch (inCode & 0b11111111) {
        case 0b00000000:
          return new NOP_t(inAddr, inCode);
          break;
        default:
          printf("Unsupported Miscellaneous instruction (if-then and hints):  %b @ |0x%.8x| \n", inCode, inAddr);
      }
      break;
    default:
      printf("Unsupported Miscellaneous instruction : %b @ |0x%.8x| \n", inCode, inAddr);
    }
  } else {
    if (inCode & (1 << 11))
      return new ADDTOSP_t(inAddr, inCode);
    else
      return new ADDTOPC_t(inAddr, inCode);
  }
  return NULL;
}

/* Decode 3  */

class STOREWORDimm_t : public Inst_t {
  uint8_t sReg, iReg, imm5;

public:
  STOREWORDimm_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm5 = (inCode >> 6) & 0b11111;
    sReg = inCode & 0b111;
    iReg = (inCode >> 3) & 0b111;
  }

  virtual void Print() { printf("%x: str.w r%d, [r%d, #%d]", addr, sReg, iReg, imm5 << 2); }

  virtual uint8_t memAccessCount() { return 1; }

  virtual void romeoFuncContent() {
    printf("  memWrite(mem, ");
    pReg(iReg);
    printf(" + %d, ", imm5 << 2);
    pReg(sReg);
    printf(");\n");
  }
};

class LOADWORDimm_t : public Inst_t {
  uint8_t dReg, iReg, imm5;

public:
  LOADWORDimm_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm5 = (inCode >> 6) & 0b11111;
    dReg = inCode & 0b111;
    iReg = (inCode >> 3) & 0b111;
  }

  virtual void Print() { printf("%x: ldr.w r%d, [r%d, #%d]", addr, dReg, iReg, imm5 << 2); }

  virtual uint8_t memAccessCount() { return 1; }

  virtual void romeoFuncContent() {
    wReg(dReg);
    printf("memRead(mem, ");
    pReg(iReg);
    printf(" + %d);\n", imm5 << 2);
  }
};

class STOREBYTEimm_t : public Inst_t {
  uint8_t sReg, iReg, imm5;

public:
  STOREBYTEimm_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm5 = (inCode >> 6) & 0b11111;
    sReg = inCode & 0b111;
    iReg = (inCode >> 3) & 0b111;
  }
  virtual uint8_t memAccessCount() { return 1; }
  virtual void Print() { printf("%x: str.b r%d, [r%d, #%d]", addr, sReg, iReg, imm5); }
};

class LOADBYTEimm_t : public Inst_t {
  uint8_t dReg, iReg, imm5;

public:
  LOADBYTEimm_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    imm5 = (inCode >> 6) & 0b11111;
    dReg = inCode & 0b111;
    iReg = (inCode >> 3) & 0b111;
  }
  virtual uint8_t memAccessCount() { return 1; }
  virtual void Print() { printf("%x: ldr.b r%d, [r%d, #%d]", addr, dReg, iReg, imm5); }
};

Inst_t *Inst_t::decodeThumb3(const uint32_t inAddr, const uint16_t inCode) {
  uint16_t codop = (inCode >> 11) & 0b11;
  switch (codop) {
  case 0b00:
    return new STOREWORDimm_t(inAddr, inCode);
    break;
  case 0b01:
    return new LOADWORDimm_t(inAddr, inCode);
    break;
  case 0b10:
    return new STOREBYTEimm_t(inAddr, inCode);
    break;
  case 0b11:
    return new LOADBYTEimm_t(inAddr, inCode);
    break;
  }
  return NULL;
}

/* Decode 6 */

class CONDBR_t : public Inst_t {
  int8_t imm8;
  uint32_t mTransitionIdTaken;
  uint32_t mTargetIdTaken;

public:
  CONDBR_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr), mTargetIdTaken(0) { imm8 = inCode & 0b11111111; }
  virtual uint32_t branchAddress() { return addr + (int16_t)(imm8 * 2) + 4; }
  virtual bool isCondBranch() { return true; }
  virtual void setTransitionIdTaken(const uint32_t inTransitionId) { mTransitionIdTaken = inTransitionId; }
  virtual uint32_t transitionIdTaken() { return mTransitionIdTaken; }
  virtual void setTargetIdTaken(const uint32_t inTargetId) { mTargetIdTaken = inTargetId; }
  virtual uint32_t targetIdTaken() { return mTargetIdTaken; }

  void PrintOffset() { printf("%x", addr + (int16_t)(imm8 * 2) + 4); }
};

class BNE_t : public CONDBR_t {
public:
  BNE_t(const uint32_t inAddr, const uint16_t inCode) : CONDBR_t(inAddr, inCode) {}
  virtual void Print() {
    printf("%x: bne.n ", addr);
    PrintOffset();
  }
  virtual const char *guard() { return "((st[$any].regs.sr & Zmask) #noteq Zmask)"; }
};

class BLT_t : public CONDBR_t {
public:
    BLT_t(const uint32_t inAddr, const uint16_t inCode) : CONDBR_t(inAddr, inCode) {}
    virtual void Print() {
        printf("%x: blt.n ", addr);
        PrintOffset();
    }
    virtual const char *guard() { return "(st[$any].regs.sr &  Nmask) #eqeq Nmask"; }
};

class BLE_t : public CONDBR_t {
public:
  BLE_t(const uint32_t inAddr, const uint16_t inCode) : CONDBR_t(inAddr, inCode) {}
  virtual void Print() {
    printf("%x: ble.n ", addr);
    PrintOffset();
  }
  virtual const char *guard() { return "((st[$any].regs.sr & Zmask) #eqeq Zmask) || ((st[$any].regs.sr & Nmask) #eqeq Nmask)"; }
};

class BEQ_t : public CONDBR_t {
public:
  BEQ_t(const uint32_t inAddr, const uint16_t inCode) : CONDBR_t(inAddr, inCode) {}
  virtual void Print() {
    printf("%x: beq.n ", addr);
    PrintOffset();
  }
  virtual const char *guard() { return "((st[$any].regs.sr & Zmask) #eqeq Zmask)"; }
};

class BLS_t : public CONDBR_t {
public:
    BLS_t(const uint32_t inAddr, const uint16_t inCode) : CONDBR_t(inAddr, inCode) {}
    virtual void Print() {
      printf("%x: bls.n ", addr);
      PrintOffset();
    }
    virtual const char *guard() { return "((st[$any].regs.sr & Zmask) #eqeq Zmask) || ((st[$any].regs.sr & Cmask) #noteq Cmask)"; }
};

class BGE_t : public CONDBR_t {
public:
  BGE_t(const uint32_t inAddr, const uint16_t inCode) : CONDBR_t(inAddr, inCode) {}
  virtual void Print() {
    printf("%x: bge.n ", addr);
    PrintOffset();
  }
  virtual const char *guard() { return "((st[$any].regs.sr & Zmask) #eqeq Zmask) || ((st[$any].regs.sr & Nmask) #noteq Nmask)"; }
};

class STMIA_t : public Inst_t {
  uint8_t iReg;
  uint16_t sRegList;

public:
  STMIA_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    iReg = (inCode >> 8) & 0b111;
    sRegList = (inCode & 0b11111111) | ((inCode & 0b100000000) << 6);
  }
  virtual void Print() {
    printf("%x: stmia r%d!, {", addr, iReg);
    uint16_t rn = 0, rl = sRegList;
    bool first = true;
    while (rl != 0) {
      if (rl & 1) {
        if (first)
          first = false;
        else
          printf(", ");
        switch (rn) {
        case 13:
          printf("sp");
          break;
        case 14:
          printf("lr");
          break;
        case 15:
          printf("pc");
          break;
        default:
          printf("r%d", rn);
        }
      }
      rl >>= 1;
      rn++;
    }
    printf("}");
  }

  virtual uint8_t memAccessCount() {
    uint16_t accessCount = 0, rl = sRegList;
    while (rl != 0) {
      if (rl & 1) {
        accessCount++;
      }
      rl >>= 1;
    }
    return accessCount;
  }

  virtual void romeoFuncContent() {
    uint16_t regList = sRegList;
    uint8_t regNum = 0;
    uint8_t regCount = 0;
    while (regList != 0) {
      if (regList & 1) {
        printf("  memWrite(mem,");
        pReg(iReg);
        printf(" + %d, ", regCount * 4);
        pReg(regNum);
        printf(");\n");
        regCount++;
      }
      regList >>= 1;
      regNum++;
    }
    wReg(iReg);
    pReg(iReg);
    printf(" + %d;\n", regCount * 4);
  };
};

class LDMIA_t : public Inst_t {
  uint8_t iReg;
  uint16_t sRegList;

public:
  LDMIA_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr) {
    iReg = (inCode >> 8) & 0b111;
    sRegList = (inCode & 0b11111111) | ((inCode & 0b100000000) << 6);
  }

  virtual uint8_t memAccessCount() {
    uint16_t accessCount = 0, rl = sRegList;
    while (rl != 0) {
      if (rl & 1) {
        accessCount++;
      }
      rl >>= 1;
    }
    return accessCount;
  }

  virtual void Print() {
    printf("%x: ldmia r%d!, {", addr, iReg);
    uint16_t rn = 0, rl = sRegList;
    bool first = true;
    while (rl != 0) {
      if (rl & 1) {
        if (first)
          first = false;
        else
          printf(", ");
        switch (rn) {
        case 13:
          printf("sp");
          break;
        case 14:
          printf("lr");
          break;
        case 15:
          printf("pc");
          break;
        default:
          printf("r%d", rn);
        }
      }
      rl >>= 1;
      rn++;
    }
    printf("}");
  }
};

Inst_t *Inst_t::decodeThumb6(const uint32_t inAddr, const uint16_t inCode) {
  if (inCode & (1 << 12)) {
    switch ((inCode >> 8) & 0b1111) {
    case 0b0000:
      return new BEQ_t(inAddr, inCode);
      break;
    case 0b0001:
      return new BNE_t(inAddr, inCode);
      break;
    case 0b1001:
      return new BLS_t(inAddr, inCode);
      break;
    case 0b1010:
      return new BGE_t(inAddr, inCode);
      break;
    case 0b1011:
      return new BLT_t(inAddr, inCode);
    case 0b1101:
      return new BLE_t(inAddr, inCode);
      break;
    case 0b1111:
      break;
    default:
        printf("Unsupported operation: %b @ |0x%.8x|\n", inCode, inAddr);
    }
  } else {
    if (inCode & (1 << 11))
      return new LDMIA_t(inAddr, inCode);
    else
      return new STMIA_t(inAddr, inCode);
  }
  return NULL;
}

class BA_t : public Inst_t {
  int16_t imm11;
  uint32_t mTargetIdTaken;

public:
  BA_t(const uint32_t inAddr, const uint16_t inCode) : Inst_t(inAddr), mTargetIdTaken(0) { imm11 = inCode & 0b11111111111; }
  virtual bool isUncondBranch() { return true; }
  virtual uint32_t branchAddress() { return addr + 4 + imm11 * 2; }
  virtual void Print() { printf("%x: b.n %x", addr, branchAddress()); }
  virtual void setTargetIdTaken(const uint32_t inTargetId) { mTargetIdTaken = inTargetId; }
  virtual uint32_t targetIdTaken() { return mTargetIdTaken; }
};

Inst_t *Inst_t::decodeThumb7(const uint32_t inAddr, const uint16_t inCode) {
  if (((inCode >> 11) & 0b11) == 0)
    return new BA_t(inAddr, inCode);
  else {
  }
  printf("32 bit instruction tagged as 16 bits instruction :%d\n", inCode);
  return NULL;
}

Inst_t *Inst_t::decodeThumb(const uint32_t inAddr, const uint16_t inCode) {
  // extract the 3 most significant bits
  const uint16_t mostSig3Bits = inCode >> 13;
  switch (mostSig3Bits) {
  case 0:
    return decodeThumb0(inAddr, inCode);
    break;
  case 1:
    return decodeThumb1(inAddr, inCode);
    break;
  case 2:
    return decodeThumb2(inAddr, inCode);
    break;
  case 3:
    return decodeThumb3(inAddr, inCode);
    break;
  // case 4:
  //   return decodeThumb4(inAddr, inCode);
  //   break;
  case 5:
    return decodeThumb5(inAddr, inCode);
    break;
  case 6:
    return decodeThumb6(inAddr, inCode);
    break;
  case 7:
    return decodeThumb7(inAddr, inCode);
    break;
  default:
    printf("Unsupported instruction 3 higher bits: %d\n", mostSig3Bits);
  }
  return NULL;
}

class BL_t : public Inst_t {
  uint32_t offset;
  uint32_t mTargetIdTaken;

public:
  BL_t(const uint32_t inAddr, const uint32_t inCode) : Inst_t(inAddr), mTargetIdTaken(0) {
    uint32_t S = (inCode >> 26) & 1;
    uint32_t J1 = (inCode >> 13) & 1;
    uint32_t J2 = (inCode >> 11) & 1;
    uint32_t I1 = J1 == S;
    uint32_t I2 = J2 == S;
    uint32_t imm10 = (inCode >> 16) & 0b1111111111;
    uint32_t imm11 = inCode & 0b11111111111;
    uint32_t lowPart = (I1 << 23) | (I2 << 22) | (imm10 << 12) | (imm11 << 1);
    if (S) {
      offset = 0xFF000000 | lowPart;
    } else {
      offset = lowPart;
    }
  }

  virtual bool isFuncCall() { return true; }
  virtual uint32_t branchAddress() { return addr + 4 + offset; }
  virtual void Print() { printf("%x: bl %x", addr, addr + 4 + offset); }
  virtual void setTargetIdTaken(const uint32_t inTargetId) { mTargetIdTaken = inTargetId; }
  virtual uint32_t targetIdTaken() { return mTargetIdTaken; }
};

class MUL_t : public Inst_t {
    uint8_t nReg, dReg, mReg;

public:
    MUL_t(const uint32_t inAddr, const uint32_t inCode) : Inst_t(inAddr) {
      nReg = ((inCode >> 16) & 0b1111);
      dReg = ((inCode >> 8) & 0b1111);
      mReg = ((inCode) & 0b1111);
    }

    virtual void Print() { printf("%x: mul r%d, r%d, r%d", addr, dReg, nReg, mReg); }

    virtual void romeoFuncContent() {
      wReg(dReg);
      pReg(nReg);
      printf("*");
      pReg(mReg);
      printf(";\n");
      updateSR(dReg);
      printf("\n");
    };
};

class SDIV_t : public Inst_t {
    uint8_t nReg, dReg, mReg;

public:
    SDIV_t(const uint32_t inAddr, const uint32_t inCode) : Inst_t(inAddr) {
      nReg = ((inCode >> 16) & 0b1111);
      dReg = ((inCode >> 8) & 0b1111);
      mReg = ((inCode) & 0b1111);
    }

    virtual void Print() { printf("%x: sdiv r%d, r%d, r%d", addr, dReg, nReg, mReg); }

    virtual void romeoFuncContent() {
      wReg(dReg);
      pReg(nReg);
      printf("/");
      pReg(mReg);
      printf(";\n");
      updateSR(dReg);
      printf("\n");
    };
};

class ADDimmediate_t : public Inst_t { // encoding T3 A7.7.3
    uint8_t nReg, dReg, imm3, i;
    uint16_t imm8;
    uint32_t imm32;

public:
    ADDimmediate_t(const uint32_t inAddr, const uint32_t inCode) : Inst_t(inAddr) {
      nReg = ((inCode >> 16) & 0b1111);
      dReg = ((inCode >> 8) & 0b1111);
      i = ((inCode >> 26) & 0b1);
      imm3 = ((inCode >> 12) & 0b111);
      imm8 = ((inCode) & 0b11111111);

      imm32 = (i << 3 << 8) | (imm3 << 8 ) | imm8;
    }

    virtual void Print() { printf("%x: add.w r%d, r%d, #%d", addr, dReg, nReg, imm32); }

    virtual void romeoFuncContent() {
      wReg(dReg);
      pReg(nReg);
      printf(" + %d;\n", imm32);
      updateSR(dReg);
      printf("\n");
    };
};

Inst_t *Inst_t::decodeDataProcessing(const uint32_t inAddr, const uint32_t inCode) {
  const uint8_t op = ((inCode >> 16 >> 4) & 0b11111);
  const uint8_t rn = ((inCode >> 16) & 0b1111);
  const uint8_t rd = ((inCode >> 8) & 0b1111);
  const uint8_t releventop = ((op >> 1) & 0b1111);

  switch (releventop) {
    case 0b1000:
      if (rd == 0b1111) {
        printf("Unsupported data processing operation 1 (%b @ |0x%.8x|)\n", inCode, inAddr);
      } else {
        return new ADDimmediate_t(inAddr, inCode);
        break;
      }
      break;
    default:
      printf("Unsupported data processing operation 2 (%b @ |0x%.8x|)\n", inCode, inAddr);
      break;
  }
  return NULL;
}

Inst_t *Inst_t::decodeARM32(const uint32_t inAddr, const uint32_t inCode) {
  const uint32_t codop = ((inCode >> 27) & 0b11); // op1 in ARM documentation
  //  printf("%x, %x\n", inCode, codop);
  uint32_t subCodop;
  switch (codop) {
    case 2: { // op1 = 10
      uint8_t op = ((inCode >> 15) & 0b1); // op in ARM documentation
      if (op == 1) {
        subCodop = ((inCode >> 13) & 0b10) | ((inCode >> 12) & 0b01);
        switch (subCodop) {
          case 3:
            return new BL_t(inAddr, inCode);
            break;
          default:
            printf("Unsupported operation (%b @ |0x%.8x|)\n", inCode, inAddr);
            break;
        }
      } else if (op == 0) {
        const uint8_t partcodOp2 = ((inCode >> 26) & 0b1);
        switch (partcodOp2) {
          case 0:
            return Inst_t::decodeDataProcessing(inAddr, inCode);
            break;
          case 1:
            printf("Unsupported operation (%b @ |0x%.8x|)\n", inCode, inAddr);
            break;
          default:
            printf("Unsupported operation (%b @ |0x%.8x|)\n", inCode, inAddr);
            break;
        }
      } else { printf("Unsupported operation (%b @ |0x%.8x|)\n", inCode, inAddr); }
  }
  case 3: {
    const uint32_t codOp2 = ((inCode >> 20) & 0b1111111);
    if ((!(codOp2 >> 6)) && (codOp2 >> 4 & 0b11) && !(codOp2 & 0b1)) { //codeOp2 = 0110xxx
      const uint32_t subCodOp1 = ((inCode >> 20) & 0b111);
      const uint32_t subCodOp2 = ((inCode >> 4) & 0b11);
      const uint32_t subCodOpRa = ((inCode >> 12) & 0b1111);
      if (subCodOp1 == 0b000 && subCodOp2 == 0b00 && subCodOpRa == 0b1111) { //mul
        return new MUL_t(inAddr, inCode);
      }
    } else if ((!(codOp2 >> 6)) && (codOp2 >> 3 & 0b111)) { //codeOp2 = 0111xxx
      const uint32_t subCodOp1 = ((inCode >> 20) & 0b111);
      const uint32_t subCodOp2 = ((inCode >> 4) & 0b1111);
      if(subCodOp1 == 0b001 && subCodOp2 == 0b1111) { //sdiv
        return new SDIV_t(inAddr, inCode);
      }
      }
      else {
        printf("Unsupported operation: %b @ |0x%.8x| (2)\n", inCode, inAddr);
      }
    break;
  }
  default:
    printf("Unsupported operation: %b @ |0x%.8x|\n", inCode, inAddr);
  }
  return NULL;
}

void generatePlace(FILE *prog, Inst_t *inst, uint32_t depth) {
  fprintf(prog, "<place id=\"%d\" identifier=\"INST%x\" label=\"INST%x\" initialMarking=\"0\" eft=\"0\" lft=\"0\">\n",
          inst->placeId(), inst->address(), inst->address());
  fprintf(prog, "    <graphics color=\"0\">\n");
  fprintf(prog, "        <position x=\"%.1f\" y=\"%.1f\"/>\n", depth * 200 + 151.0, 90 * inst->placeId() + 61.0);
  fprintf(prog, "        <deltaLabel deltax=\"50\" deltay=\"-5\"/>\n");
  fprintf(prog, "    </graphics>\n    <scheduling gamma=\"0\" omega=\"0\"/>\n</place>\n");
}

void lowGenerateTransition(FILE *prog, Inst_t *inst, uint32_t depth, const bool condBr = false, const bool taken = false) {
  float offsetX = 0.0;
  float offsetY = 0.0;
  uint32_t transitionId = inst->transitionId();
  char const *suffix;
  if (condBr) {
    if (taken) {
      offsetX = -1.0;
      offsetY = -1.0;
      transitionId = inst->transitionIdTaken();
      suffix = "_T";
    } else {
      suffix = "_NT";
    }
  } else {
    suffix = "";
  }

  fprintf(prog,
          "<transition id=\"%d\" identifier=\"I%x%s\" label=\"I%x%s\" eft=\"0\" lft=\"0\" speed=\"1\" cost=\"0\" unctrl=\"0\" "
          "obs=\"1\"",
          transitionId, inst->address(), suffix, inst->address(), suffix);
  if (condBr) {
    if (taken) {
      fprintf(prog, " guard=\"%s && (doFetch[$any] == 1)\">\n", inst->guard());
    } else {
      fprintf(prog, " guard=\"!(%s) && (doFetch[$any] == 1)\">\n", inst->guard());
    }
  } else {
    //    fprintf(prog, " guard=\"\">\n");
    fprintf(prog, " guard=\"doFetch[$any] #eqeq 1\">\n");
  }
  fprintf(prog, "    <graphics color=\"0\">\n");
  fprintf(prog, "        <position x=\"%.1f\" y=\"%.1f\"/>\n", depth * 200 + 151.0 + offsetX * 100,
          90 * inst->placeId() + 106.0 + offsetY * 45);
  fprintf(prog, "        <deltaLabel deltax=\"25\" deltay=\"0\"/>\n");
  fprintf(prog, "        <deltaGuard deltax=\"20\" deltay=\"-20\"/>\n");
  fprintf(prog, "        <deltaUpdate deltax=\"130\" deltay=\"0\"/>\n");
  fprintf(prog, "        <deltaSpeed deltax=\"-20\" deltay=\"5\"/>\n");
  fprintf(prog, "        <deltaCost deltax=\"-20\" deltay=\"5\"/>\n");
  fprintf(prog, "    </graphics>\n");
  fprintf(prog, "    <update><![CDATA[isHit[$any] = inst%x(st[$any],mem[$any]);\ndoFetch[$any] = 0;\nac[$any] = %d;]]></update>\n",
          inst->address(), inst->memAccessCount());
  fprintf(prog, "</transition>\n");
}

void generateTransition(FILE *prog, Inst_t *inst, uint32_t depth) {
  if (inst->isCondBranch()) {
    lowGenerateTransition(prog, inst, depth, true, false);
    lowGenerateTransition(prog, inst, depth, true, true);
  } else {
    lowGenerateTransition(prog, inst, depth);
  }
}

bool isStopAddress(const uint32_t stop, vector<uint32_t> &stopAddresses) {
  for (auto i = stopAddresses.begin(); i != stopAddresses.end(); ++i) {
    if (*i == stop)
      return true;
  }
  return false;
}

void generatePlaces(FILE *prog, vector<Inst_t *> &program, vector<Word_t *> &words, const uint32_t startAddress,
                    vector<uint32_t> &stopAddresses, uint32_t depth = 0) {
  for (auto i = program.begin(); i != program.end(); ++i) {
    if ((*i)->address() >= startAddress) {
      generatePlace(prog, *i, depth);
      generateTransition(prog, *i, depth);
      if ((*i)->isFuncCall()) {
        bool found = false;
        for (auto j = program.begin(); j != program.end(); ++j) {
          //          printf("Comparing target %x with instruction %x ... ", (*i)->branchAddress(), (*j)->address());
          if ((*i)->branchAddress() == (*j)->address()) {
            found = true;
            //            printf("found\n");
            generatePlaces(prog, program, words, (*i)->branchAddress(), stopAddresses, depth + 1);
          }
        }
        if (!found) {
          printf("BL target %x not in program\n", (*i)->branchAddress());
          exit(1);
        }
      }
      if (isStopAddress((*i)->address(), stopAddresses))
        break;
    }
  }
}

void genUpArc(FILE *prog, uint32_t place, uint32_t transition) {
  fprintf(
    prog,
    "    <arc place=\"%d\" transition=\"%d\" type=\"PlaceTransition\" weight=\"1\" tokenColor=\"-1\"  inhibitingCondition=\"\">\n",
    place, transition);
  fprintf(prog, "        <nail xnail=\"0\" ynail=\"0\"/>\n");
  fprintf(prog, "        <graphics  color=\"0\"></graphics>\n");
  fprintf(prog, "   </arc>\n");
}

void genDownArc(FILE *prog, uint32_t place, uint32_t transition, float Xnail = 0.0, float Ynail = 0.0) {
  fprintf(
    prog,
    "    <arc place=\"%d\" transition=\"%d\" type=\"TransitionPlace\" weight=\"1\" tokenColor=\"-1\"  inhibitingCondition=\"\">\n",
    place, transition);
  fprintf(prog, "        <nail xnail=\"%.1f\" ynail=\"%.1f\"/>\n", Xnail, Ynail);
  fprintf(prog, "        <graphics  color=\"0\"></graphics>\n");
  fprintf(prog, "   </arc>\n");
}

void generateArcs(FILE *prog, vector<Inst_t *> &program, vector<Word_t *> &words, const uint32_t startAddress) {
  for (auto i = program.begin(); i != program.end(); ++i) {
    // arc from place to transition
    genUpArc(prog, (*i)->placeId(), (*i)->transitionId());
    if ((*i)->isCondBranch()) {
      genUpArc(prog, (*i)->placeId(), (*i)->transitionIdTaken());
      genDownArc(prog, (*(i + 1))->placeId(), (*i)->transitionId());
      genDownArc(prog, (*i)->targetIdTaken(), (*i)->transitionIdTaken());
    } else if ((*i)->isUncondBranch()) {
      genDownArc(prog, (*i)->targetIdTaken(), (*i)->transitionId(), 500.0, 90 * (*i)->placeId() + 536.0);
    } else if ((*i)->isFuncCall()) {
      genDownArc(prog, (*i)->targetIdTaken(), (*i)->transitionId(), 500.0, 90 * (*i)->placeId() + 536.0);
    } else if ((*i)->isFuncReturn()) {
      genDownArc(prog, (*i)->targetIdTaken(), (*i)->transitionId(), 100.0, 90 * (*i)->placeId() - 536.0);
    } else {
      genDownArc(prog, (*(i + 1))->placeId(), (*i)->transitionId());
    }
  }
}

void generatePN(vector<Inst_t *> &program, vector<Word_t *> &words, vector<uint32_t> &stopAddresses) {
  FILE *prog = fopen("program.xml", "w");
  filesystem::path dir = filesystem::current_path();
  // printf("%s\n", dir.c_str());
  fprintf(prog, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  fprintf(prog, "<romeo version=\"Romeo v3.8.4-rc1\"></romeo>\n");
  fprintf(prog, "<TPN name=\"%s/%s\">\n", dir.c_str(), "program.xml");

  generatePlaces(prog, program, words, 0x8000, stopAddresses);
  generateArcs(prog, program, words, 0x8000);

  fprintf(prog, "<timedCost>-1</timedCost>\n");
  fprintf(prog, "<nbTokenColor>2</nbTokenColor>\n");
  fprintf(prog, "<declaration><![CDATA[// insert here the state variables declarations\n");
  fprintf(prog, "// and possibly some code to initialize them\n");
  fprintf(prog, "// using C-like syntax\n\n");
  fprintf(prog, "// insert here your type definitions using C-like syntax\n\n");
  fprintf(prog, "// insert here your function definitions\n");
  fprintf(prog, "// using C-like syntax]]>\n</declaration>\n\n");
  fprintf(prog, "<project nbinput=\"0\" openinput=\"0\" nbinclude=\"0\">\n");
  // fprintf(prog, "    <include id=\"1\" file=\"declarations.c\"/>\n");
  // fprintf(prog, "    <include id=\"2\" file=\"instructions.c\"/>\n");
  fprintf(prog, "</project>\n\n");

  fprintf(prog, "<preferences>\n");
  fprintf(prog, "    <colorPlace c0=\"SkyBlue2\" c1=\"gray\" c2=\"cyan\" c3=\"green\" c4=\"yellow\" c5=\"brown\"/>\n");
  fprintf(prog, "    <colorTransition c0=\"yellow\" c1=\"gray\" c2=\"cyan\" c3=\"green\" c4=\"SkyBlue2\" c5=\"brown\"/>\n");
  fprintf(prog, "    <colorArc c0=\"black\" c1=\"gray\" c2=\"blue\" c3=\"#beb760\" c4=\"#be5c7e\" c5=\"#46be90\"/>\n");
  fprintf(prog, "</preferences>\n");
  fprintf(prog, "</TPN>\n");

  fclose(prog);
}

uint32_t idFromAddress(vector<Inst_t *> &program, uint32_t inAddr) {
  for (auto i = program.begin(); i != program.end(); ++i) {
    // if (inAddr == 0x8070) {
    //   printf("Lookup inst ");
    //   (*i)->Print();
    //   printf("\n");
    // }
    if ((*i)->address() == inAddr) {
      return (*i)->placeId();
    }
  }
  return 0;
}

void computeTargetId(vector<Inst_t *> &program, vector<uint32_t> &stopAddresses, uint32_t startAddress, uint32_t returnId = -1) {
  for (auto i = program.begin(); i != program.end(); ++i) {
    if ((*i)->address() >= startAddress && (*i)->targetIdTaken() == 0) {
      (*i)->setReachable(true);
      if ((*i)->isFuncCall()) {
        (*i)->setTargetIdTaken(idFromAddress(program, (*i)->branchAddress()));
        Inst_t *nextInst = *(i + 1);
        computeTargetId(program, stopAddresses, (*i)->branchAddress(), nextInst->placeId());
      } else if ((*i)->isUncondBranch()) {
        // (*i)->Print();
        // printf(" Branche addr: %x / Id from addr : %d", (*i)->branchAddress(), idFromAddress(program, (*i)->branchAddress()));
        (*i)->setTargetIdTaken(idFromAddress(program, (*i)->branchAddress()));
        // printf(" *** %d\n", (*i)->targetIdTaken());
      } else if ((*i)->isCondBranch()) {
        (*i)->setTargetIdTaken(idFromAddress(program, (*i)->branchAddress()));
      } else if ((*i)->isFuncReturn()) {
        (*i)->setTargetIdTaken(returnId);
        break;
      }
    }
  }
}

void genProgData(vector<Inst_t *> &program) {
  printf("const program_t prog = {\n");
  for (auto i = program.begin(); i != program.end(); ++i) {
    if (i != program.begin())
      printf(", ");
    printf("%d", (*i)->memAccessCount());
  }
  printf("\n};\n\n");
}

void genFuncs(vector<Inst_t *> &program) {
  for (auto i = program.begin(); i != program.end(); ++i)
    (*i)->romeoFunc();
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: extract <stop address> [, <stop address>]\n");
    return 1;
  }

  vector<uint32_t> stopAddresses;
  for (uint32_t i = 1; i < argc; i++) {
    uint32_t stopAddress = strtol(argv[1], NULL, 0);
    stopAddresses.push_back(stopAddress);
  }

  char *buffer = NULL;
  size_t bufLen = 0;
  ssize_t lineLen;
  vector<Inst_t *> program;
  vector<Word_t *> words;

  while ((lineLen = getline(&buffer, &bufLen, stdin)) != -1) {
    char delim[] = ":";
    char *type = strtok(buffer, delim);
    uint32_t addr = strtol(strtok(NULL, delim), NULL, 16);
    uint32_t inst = strtol(strtok(NULL, delim), NULL, 16);
    Inst_t *decodedInst;
    Word_t *inlineWord;
    switch (type[0]) {
    case 't':
      decodedInst = Inst_t::decodeThumb(addr, inst);
      if (decodedInst != NULL) {
        program.push_back(decodedInst);
        // decodedInst->Print();
        // if (decodedInst->isLDRPC())
        //   printf("(%x)", decodedInst->targetWord());
      }
      // printf("    %s/%d/%d\n", type, addr, inst);
      break;
    case 'w':
      inlineWord = new Word_t(addr, inst);
      words.push_back(inlineWord);
      break;
    case 'a':
      decodedInst = Inst_t::decodeARM32(addr, inst);
      if (decodedInst != NULL) {
        program.push_back(decodedInst);
        // decodedInst->Print();
      }
      // printf("    %s/%d/%d\n", type, addr, inst);
      break;
    }
  }

  for (auto i = program.begin(); i != program.end(); ++i)
    if ((*i)->isLDRPC()) {
      for (auto j = words.begin(); j != words.end(); ++j)
        if ((*i)->targetWord() == (*j)->addr) {
          (*i)->setImmByPC((*j)->value);
        }
    }

  //  genProgData(program);
  genFuncs(program);

  uint32_t placeId = 1;
  uint32_t transitionId = 1;
  for (auto i = program.begin(); i != program.end(); ++i) {
    (*i)->setPlaceId(placeId);
    placeId++;
    (*i)->setTransitionId(transitionId);
    transitionId++;
    if ((*i)->isCondBranch()) {
      (*i)->setTransitionIdTaken(transitionId);
      transitionId++;
    }
  }

  computeTargetId(program, stopAddresses, 0x8000);

  // for (auto i = program.begin(); i != program.end(); ++i) {
  //   if ((*i)->isReachable()) {
  //     printf("(%d) ", (*i)->placeId());
  //     (*i)->Print();
  //     if ((*i)->targetIdTaken() != 0)
  //       printf(" *** -> %d", (*i)->targetIdTaken());
  //     printf("\n");
  //   }
  // }
  generatePN(program, words, stopAddresses);

  return 0;
}
