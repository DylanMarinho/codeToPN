const int Npos = 0;
const int Zpos = 1;
const int Cpos = 2;
const int Vpos = 3;

// const int Zmask = 2;
// const int Nmask = 1;
const int Zmask = 1 << Zpos;
const int Nmask = 1 << Npos;

const int r0 = 1;
const int r1 = 1 << 1;
const int r2 = 1 << 2;
const int r3 = 1 << 3;
const int r4 = 1 << 4;
const int r5 = 1 << 5;
const int r6 = 1 << 6;
const int r7 = 1 << 7;
const int r8 = 1 << 8;
const int r9 = 1 << 9;
const int r10 = 1 << 10;
const int r11 = 1 << 11;
const int r12 = 1 << 12;
const int r13 = 1 << 13;
const int r14 = 1 << 14;
const int r15 = 1 << 15;
const int sp = r13;
const int lr = r14;
const int pc = r15;

const int dataStart = 98432;

typedef uint8_t instruction_t;

typedef instruction_t[100] program_t;

typedef struct {
  int[16] r;
  int sr;
} registers_t;

/*
 * cache line info:
 *  bit 31 is valid bit
 *  bits 30-0 are tags
 */
const int valid = 1 << 31;
const int tagMask = (1 << 31) - 1;
typedef int[16] cache_t;

typedef struct {
  int[30] a;
} mem_t;

typedef struct {
  registers_t regs;
  cache_t ICache;
} core_t;

typedef core_t[1] state_t;

initially {
  // state of the system
  state_t st;
  mem_t[1] mem;
  // = 1 if a cache access is a hit
  int[1] isHit = {0};
  // access count of an instruction
  int[1] ac = {0};
  //
  uint8_t[2] doFetch = {0, 0};
  //
  uint8_t lockBus = 0;
  uint8_t accessCount = 0;
  //
  uint16_t prologAccessCount = 40;

  initCache(st[0].ICache);
  // initCache(st[1].ICache);

  st[0].regs.r[13] = dataStart + 100;
  // st[1].regs.r[13] = dataStart + 560;

  int i;
  for (i = 0; i < 13; i++) {
    st[0].regs.r[i] = i;
    //    st[1].regs.r[i] = 20 + i;
  }
  st[0].regs.sr = 0;
  //  st[1].regs.sr = 0;
}

void initCache(cache_t &cache) {
  int i;
  for (i = 0; i < 16; i++) {
    cache[i] = 0; // not valid
  }
}

void updateSR(registers_t &regs, uint64_t val, uint32_t op1, uint32_t op2) {
  if (val == 0) {
    regs.sr = regs.sr | (1 << Zpos);
  } else {
    regs.sr = regs.sr & ~(1 << Zpos);
  }
  if ((val >> 31) & 1 == 1) {
    regs.sr = regs.sr | (1 << Npos);
  } else {
    regs.sr = regs.sr & ~(1 << Npos);
  }
  if ((val >> 32) & 1 == 1) {
    regs.sr = regs.sr | (1 << Cpos);
  } else {
    regs.sr = regs.sr & ~(1 << Cpos);
  }
  /* reset V flag */
  uint32_t status = regs.sr & ~(1 << Vpos);
  if ((op1 >> 31) & 1 == (op2 >> 31) & 1) {
    /* signs of operands are the same */
    if ((op1 >> 31) & 1 != (val >> 31) & 1) {
      /* but signs of result differs, it is an overflow */
      status = status | (1 << Vpos);
    }
  }
  regs.sr = status;
}

/* Does an access to a cache and return 0 if hit and 1 if miss */
int cacheAccess(cache_t &cache, int addr) {
  int line = (addr >> 5) & 15;
  int tag = addr >> 9 & tagMask;
  int result;
  if ((cache[line] & valid) != 0 && (cache[line] & tagMask == tag)) {
    result = 1;
  } else {
    result = 0;
    cache[line] = valid | (tag & tagMask);
  }
  return result;
}

int memRead(mem_t &mem, uint32_t address) {
  return mem.a[(address - dataStart) / 4];
}
int memWrite(mem_t &mem, uint32_t address, int data) {
  mem.a[(address - dataStart) / 4] = data;
}
