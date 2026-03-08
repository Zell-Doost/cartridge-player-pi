#include "cpu.h"

typedef void (*instruct)(CPU*, CMem*);

//General Instructions

void _adc(CPU* cpu, uint8_t num) 
{
    uint16_t temp = cpu->a + num + LSB(cpu->p);
    uint8_t oldAcc = cpu->a;
    cpu->a = temp;
    ARITH_SREG_ADC(cpu->a, temp);
    SREG_OVF(oldAcc, num, temp);
}

void _and(CPU* cpu, uint8_t num) 
{
    cpu->a &= num;
    LOGIC_SREG(cpu->a);
}

void _asl(CPU* cpu, uint8_t* reg) 
{
    SREG_CAR_LS(*reg);
    *reg <<= 1;
    LOGIC_SREG(*reg);
}

void _cmp(CPU* cpu, uint8_t num) 
{
    uint16_t temp = cpu->a - num;
    ARITH_SREG_SBC(temp & 0xFF, temp);
}

void _cpx(CPU* cpu, uint8_t num) 
{
    uint16_t temp = cpu->x - num;
    ARITH_SREG_SBC(temp & 0xFF, temp);
}

void _cpy(CPU* cpu, uint8_t num) 
{
    uint16_t temp = cpu->y - num;
    ARITH_SREG_SBC(temp & 0xFF, temp);
}

void _dec(CPU* cpu, uint8_t* reg) 
{
    (*reg)--;
    LOGIC_SREG(*reg);
}

void _eor(CPU* cpu, uint8_t num) 
{
    cpu->a ^= num;
    LOGIC_SREG(cpu->a);
}

void _inc(CPU* cpu, uint8_t* reg) 
{
    (*reg)++;
    LOGIC_SREG(*reg);
}

void _lda(CPU* cpu, uint8_t num) 
{
    cpu->a = num;
    LOGIC_SREG(cpu->a);
}

void _ldx(CPU* cpu, uint8_t num) 
{
    cpu->x = num;
    LOGIC_SREG(cpu->x);
}

void _ldy(CPU* cpu, uint8_t num) 
{
    cpu->y = num;
    LOGIC_SREG(cpu->y);
}

void _lsr(CPU* cpu, uint8_t* reg) 
{
    SREG_CAR_RS(*reg);
    *reg >>= 1;
    LOGIC_SREG(*reg);
}

void _ora(CPU* cpu, uint8_t num) 
{
    cpu->a |= num;
    LOGIC_SREG(cpu->a);
}

void _rol(CPU* cpu, uint8_t* reg) 
{
    uint8_t car = LSB(cpu->p);
    SREG_CAR_LS(*reg);
    *reg <<= 1;
    *reg |= car;
    LOGIC_SREG(*reg);
}

void _ror(CPU* cpu, uint8_t* reg) 
{
    uint8_t car = LSB(cpu->p);
    SREG_CAR_RS(*reg);
    *reg >>= 1;
    *reg |= (car << 7);
    LOGIC_SREG(*reg);
}

void _sbc(CPU* cpu, uint8_t num) 
{
    uint16_t temp = cpu->a + ~num + LSB(cpu->p);
    uint8_t oldAcc = cpu->a;
    cpu->a = temp;
    ARITH_SREG_SBC(cpu->a, temp);
    SREG_OVF(oldAcc, ~num, temp);
}

void _sta(CPU* cpu, uint8_t* reg) 
{
    *reg = cpu->a;
}

void _stx(CPU* cpu, uint8_t* reg) 
{
    *reg = cpu->x;
}

void _sty(CPU* cpu, uint8_t* reg) 
{
    *reg = cpu->y;
}

void _bit(CPU* cpu, uint8_t reg)
{
    cpu->p = (cpu->p & 0x3D) | (reg & 0xC0) | ((uint8_t)!(cpu->a & reg) << 1);
}

void _push(CPU* cpu, CMem* cmem, uint8_t reg)
{
    CUR_STK = reg;
    cpu->s--;
}

uint8_t _pull(CPU* cpu, CMem* cmem)
{
    cpu->s++;
     return CUR_STK;
}

//void _branch(CPU* cpu, bool cond)
//{
//    if (cond) {
//        cpu->c += REL_JMP;
//    }
//}




void brk(CPU* cpu, CMem* cmem)
{
    cpu->c += 2;
    _push(cpu, cmem, cpu->c >> 8);
    _push(cpu, cmem, cpu->c);
    _push(cpu, cmem, STS_CON | 0x30);
    STS_CON |= 0x04;
    cpu->c = BRK_VECT;
}

void ora_x_ind(CPU* cpu, CMem* cmem)
{
    _ora(cpu, X_IND_CON);
}

void ora_zpg(CPU* cpu, CMem* cmem)
{
    _ora(cpu, ZPG_CON);
}

void asl_zpg(CPU* cpu, CMem* cmem)
{
    _asl(cpu, ZPG_REG);
}

void php(CPU* cpu, CMem* cmem)
{
    uint8_t pushedStack = STS_CON | 0x30;
    _push(cpu, cmem, pushedStack);
}

void ora_imm(CPU* cpu, CMem* cmem)
{
    _ora(cpu, IMMEDIATE);
}

void asl_a(CPU* cpu, CMem* cmem)
{
    _asl(cpu, ACC_REG);
}

void ora_abs(CPU* cpu, CMem* cmem)
{
    _ora(cpu, ABS_CON);
}

void asl_abs(CPU* cpu, CMem* cmem)
{
    _asl(cpu, ABS_REG);
}

void bpl_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BPL_COND)
        REL_JMP;
}

void ora_ind_y(CPU* cpu, CMem* cmem)
{
    _ora(cpu, IND_Y_CON);
}

void ora_zpg_x(CPU* cpu, CMem* cmem)
{
    _ora(cpu, ZPG_X_CON);
}

void asl_zpg_x(CPU* cpu, CMem* cmem)
{
    _asl(cpu, ZPG_X_REG);
}

void clc(CPU* cpu, CMem* cmem)
{
    cpu->p &= 0xFE;
}

void ora_abs_y(CPU* cpu, CMem* cmem)
{
    _ora(cpu, ABS_Y_CON);
}

void ora_abs_x(CPU* cpu, CMem* cmem)
{
    _ora(cpu, ABS_X_CON);
}

void asl_abs_x(CPU* cpu, CMem* cmem)
{
    _asl(cpu, ABS_X_REG);
}

void jsr_abs(CPU* cpu, CMem* cmem)
{
    //Subtract 1 in lo to account for the program counter increment after the function is run. Can optimise by removing and adding PC_INC everywhere else if necessary.
    uint16_t lo = IMMEDIATE; 
    uint16_t hi = IMMEDIATE;
    _push(cpu, cmem, cpu->c >> 8);
    _push(cpu, cmem, cpu->c);
    cpu->c = (lo | (hi << 8)) - 1;
}

void and_x_ind(CPU* cpu, CMem* cmem)
{
    _and(cpu, X_IND_CON);
}

void bit_zpg(CPU* cpu, CMem* cmem)
{
    _bit(cpu, ZPG_CON);
}

void and_zpg(CPU* cpu, CMem* cmem)
{
    _and(cpu, ZPG_CON);
}

void rol_zpg(CPU* cpu, CMem* cmem)
{
    _rol(cpu, ZPG_REG);
}

void plp(CPU* cpu, CMem* cmem)
{
    //_pull(cpu, cmem, STS_REG);
    //cpu->s++;
    cpu->p = (cpu->p & 0x30) | (_pull(cpu, cmem) & 0xCF);

}

void and_imm(CPU* cpu, CMem* cmem)
{
    _and(cpu, IMMEDIATE);
}

void rol_a(CPU* cpu, CMem* cmem)
{
    _rol(cpu, ACC_REG);
}

void bit_abs(CPU* cpu, CMem* cmem)
{
    _bit(cpu, ABS_CON);
}

void and_abs(CPU* cpu, CMem* cmem)
{
    _and(cpu, ABS_CON);
}

void rol_abs(CPU* cpu, CMem* cmem)
{
    _rol(cpu, ABS_REG);
}

void bmi_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BMI_COND)
        REL_JMP;
}

void and_ind_y(CPU* cpu, CMem* cmem)
{
    _and(cpu, IND_Y_CON);
}

void and_zpg_x(CPU* cpu, CMem* cmem)
{
    _and(cpu, ZPG_X_CON);
}

void rol_zpg_x(CPU* cpu, CMem* cmem)
{
    _rol(cpu, ZPG_X_REG);
}

void sec(CPU* cpu, CMem* cmem)
{
    cpu->p |= 0x01;
}

void and_abs_y(CPU* cpu, CMem* cmem)
{
    _and(cpu, ABS_Y_CON);
}

void and_abs_x(CPU* cpu, CMem* cmem)
{
    _and(cpu, ABS_X_CON);
}

void rol_abs_x(CPU* cpu, CMem* cmem)
{
    _rol(cpu, ABS_X_REG);
}

void rti(CPU* cpu, CMem* cmem)
{
    plp(cpu, cmem);
    cpu->c = _pull(cpu, cmem);
    cpu->c |= (uint16_t)_pull(cpu, cmem) << 8;
    cpu->c--;
}

void eor_x_ind(CPU* cpu, CMem* cmem)
{
    _eor(cpu, X_IND_CON);
}

void eor_zpg(CPU* cpu, CMem* cmem)
{
    _eor(cpu, ZPG_CON);
}

void lsr_zpg(CPU* cpu, CMem* cmem)
{
    _lsr(cpu, ZPG_REG);
}

void pha(CPU* cpu, CMem* cmem)
{
    _push(cpu, cmem, ACC_CON);
}

void eor_imm(CPU* cpu, CMem* cmem)
{
    _eor(cpu, IMMEDIATE);
}

void lsr_a(CPU* cpu, CMem* cmem)
{
    _lsr(cpu, ACC_REG);
}

void jmp_abs(CPU* cpu, CMem* cmem)
{
    //Subtract 1 in lo to account for the program ounter increment after the function is run. Can optimise by removing and adding PC_INC everywhere else if necessary.
    //uint16_t lo = IMMEDIATE; 
    //uint16_t hi = IMMEDIATE;
    uint8_t lo = IMMEDIATE; 
    uint8_t hi = IMMEDIATE; 
    cpu->c = (lo | (hi << 8)) - 1;
}

//void jmp_abs(CPU* cpu, CMem* cmem)
//{
//    // Use uint16_t to force 16-bit overflow behavior (65535 + 1 = 0)
//    uint16_t current_pc = (uint16_t)cpu->c;
//
//    // Fetch bytes using explicit 16-bit wrapping for the address
//    uint8_t lo = cmem->all[(uint16_t)(current_pc + 1)];
//    uint8_t hi = cmem->all[(uint16_t)(current_pc + 2)];
//
//    // Combine into the 16-bit target
//    uint16_t target = (uint16_t)(lo | (hi << 8));
//
//    // Update PC with the "minus one" compensation, 
//    // ensuring it wraps if target is 0x0000
//    cpu->c = (uint16_t)(target - 1);
//}

void eor_abs(CPU* cpu, CMem* cmem)
{
    _eor(cpu, ABS_CON);
}

void lsr_abs(CPU* cpu, CMem* cmem)
{
    _lsr(cpu, ABS_REG);
}

void bvc_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BVC_COND)
        REL_JMP;
}

void eor_ind_y(CPU* cpu, CMem* cmem)
{
    _eor(cpu, IND_Y_CON);
}

void eor_zpg_x(CPU* cpu, CMem* cmem)
{
    _eor(cpu, ZPG_X_CON);
}

void lsr_zpg_x(CPU* cpu, CMem* cmem)
{
    _lsr(cpu, ZPG_X_REG);
}

void cli(CPU* cpu, CMem* cmem)
{
    cpu->p &= 0xFB;
}

void eor_abs_y(CPU* cpu, CMem* cmem)
{
    _eor(cpu, ABS_Y_CON);
}

void eor_abs_x(CPU* cpu, CMem* cmem)
{
    _eor(cpu, ABS_X_CON);
}

void lsr_abs_x(CPU* cpu, CMem* cmem)
{
    _lsr(cpu, ABS_X_REG);
}

void rts(CPU* cpu, CMem* cmem)
{
    cpu->c = _pull(cpu, cmem);
    cpu->c |= (uint16_t)_pull(cpu, cmem) << 8;
}

void adc_x_ind(CPU* cpu, CMem* cmem)
{
    _adc(cpu, X_IND_CON);
}

void adc_zpg(CPU* cpu, CMem* cmem)
{
    _adc(cpu, ZPG_CON);
}

void ror_zpg(CPU* cpu, CMem* cmem)
{
    _ror(cpu, ZPG_REG);
}

void pla(CPU* cpu, CMem* cmem)
{
    ACC_CON = _pull(cpu, cmem);
    LOGIC_SREG(ACC_CON);
}

void adc_imm(CPU* cpu, CMem* cmem)
{
    _adc(cpu, IMMEDIATE);
}

void ror_a(CPU* cpu, CMem* cmem)
{
    _ror(cpu, ACC_REG);
}

void jmp_ind(CPU* cpu, CMem* cmem)
{
    uint16_t addr = (cmem->all[(uint16_t)(cpu->c + 2)] << 8) | cmem->all[(uint16_t)(cpu->c + 1)];
    uint16_t lo = cmem->all[addr];
    uint16_t hi = cmem->all[((addr & 0x00FF) == 0x00FF) ? (addr & 0xFF00) : (addr + 1)]; 
    cpu->c = ((hi << 8) | lo) - 1;
}


void adc_abs(CPU* cpu, CMem* cmem)
{
    _adc(cpu, ABS_CON);
}

void ror_abs(CPU* cpu, CMem* cmem)
{
    _ror(cpu, ABS_REG);
}

void bvs_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BVS_COND)
        REL_JMP;
}

void adc_ind_y(CPU* cpu, CMem* cmem)
{
    _adc(cpu, IND_Y_CON);
}

void adc_zpg_x(CPU* cpu, CMem* cmem)
{
    _adc(cpu, ZPG_X_CON);
}

void ror_zpg_x(CPU* cpu, CMem* cmem)
{
    _ror(cpu, ZPG_X_REG);
}

void sei(CPU* cpu, CMem* cmem)
{
    cpu->p |= 0x04;
}

void adc_abs_y(CPU* cpu, CMem* cmem)
{
    _adc(cpu, ABS_Y_CON);
}

void adc_abs_x(CPU* cpu, CMem* cmem)
{
    _adc(cpu, ABS_X_CON);
}

void ror_abs_x(CPU* cpu, CMem* cmem)
{
    _ror(cpu, ABS_X_REG);
}

void sta_x_ind(CPU* cpu, CMem* cmem)
{
    _sta(cpu, X_IND_REG);
}

void sty_zpg(CPU* cpu, CMem* cmem)
{
    _sty(cpu, ZPG_REG);
}

void sta_zpg(CPU* cpu, CMem* cmem)
{
    _sta(cpu, ZPG_REG);
}

void stx_zpg(CPU* cpu, CMem* cmem)
{
    _stx(cpu, ZPG_REG);
}

void dey(CPU* cpu, CMem* cmem)
{
    _dec(cpu, Y_REG);
}

void txa(CPU* cpu, CMem* cmem)
{
    cpu->a = cpu->x;
    LOGIC_SREG(cpu->a);
}

void sty_abs(CPU* cpu, CMem* cmem)
{
    _sty(cpu, ABS_REG);
}

void sta_abs(CPU* cpu, CMem* cmem)
{
    _sta(cpu, ABS_REG);
}

void stx_abs(CPU* cpu, CMem* cmem)
{
    _stx(cpu, ABS_REG);
}

void bcc_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BCC_COND)
        REL_JMP;
}

void sta_ind_y(CPU* cpu, CMem* cmem)
{
    _sta(cpu, IND_Y_REG);
}

void sty_zpg_x(CPU* cpu, CMem* cmem)
{
    _sty(cpu, ZPG_X_REG);
}

void sta_zpg_x(CPU* cpu, CMem* cmem)
{
    _sta(cpu, ZPG_X_REG);
}

void stx_zpg_y(CPU* cpu, CMem* cmem)
{
    _stx(cpu, ZPG_Y_REG);
}

void tya(CPU* cpu, CMem* cmem)
{
    cpu->a = cpu->y;
    LOGIC_SREG(cpu->a);
}

void sta_abs_y(CPU* cpu, CMem* cmem)
{
    _sta(cpu, ABS_Y_REG);
}

void txs(CPU* cpu, CMem* cmem)
{
    cpu->s = cpu->x;
}

void sta_abs_x(CPU* cpu, CMem* cmem)
{
    _sta(cpu, ABS_X_REG);
}

void ldy_imm(CPU* cpu, CMem* cmem)
{
    _ldy(cpu, IMMEDIATE);
}

void lda_x_ind(CPU* cpu, CMem* cmem)
{
    _lda(cpu, X_IND_CON);
}

void ldx_imm(CPU* cpu, CMem* cmem)
{
    _ldx(cpu, IMMEDIATE);
}

void ldy_zpg(CPU* cpu, CMem* cmem)
{
    _ldy(cpu, ZPG_CON);
}

void lda_zpg(CPU* cpu, CMem* cmem)
{
    _lda(cpu, ZPG_CON);
}

void ldx_zpg(CPU* cpu, CMem* cmem)
{
    _ldx(cpu, ZPG_CON);
}

void tay(CPU* cpu, CMem* cmem)
{
    cpu->y = cpu->a;
    LOGIC_SREG(cpu->y);
}

void lda_imm(CPU* cpu, CMem* cmem)
{
    _lda(cpu, IMMEDIATE);
}

void tax(CPU* cpu, CMem* cmem)
{
    cpu->x = cpu->a;
    LOGIC_SREG(cpu->x);
}

void ldy_abs(CPU* cpu, CMem* cmem)
{
    _ldy(cpu, ABS_CON);
}

void lda_abs(CPU* cpu, CMem* cmem)
{
    _lda(cpu, ABS_CON);
}

void ldx_abs(CPU* cpu, CMem* cmem)
{
    _ldx(cpu, ABS_CON);
}

void bcs_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BCS_COND)
        REL_JMP;
}

void lda_ind_y(CPU* cpu, CMem* cmem)
{
    _lda(cpu, IND_Y_CON);
}

void ldy_zpg_x(CPU* cpu, CMem* cmem)
{
    _ldy(cpu, ZPG_X_CON);
}

void lda_zpg_x(CPU* cpu, CMem* cmem)
{
    _lda(cpu, ZPG_X_CON);
}

void ldx_zpg_y(CPU* cpu, CMem* cmem)
{
    _ldx(cpu, ZPG_Y_CON);
}

void clv(CPU* cpu, CMem* cmem)
{
    cpu->p &= 0xBF;
}

void lda_abs_y(CPU* cpu, CMem* cmem)
{
    _lda(cpu, ABS_Y_CON);
}

void tsx(CPU* cpu, CMem* cmem)
{
    cpu->x = cpu->s;
    LOGIC_SREG(cpu->x);
}

void ldy_abs_x(CPU* cpu, CMem* cmem)
{
    _ldy(cpu, ABS_X_CON);
}

void lda_abs_x(CPU* cpu, CMem* cmem)
{
    _lda(cpu, ABS_X_CON);
}

void ldx_abs_y(CPU* cpu, CMem* cmem)
{
    _ldx(cpu, ABS_Y_CON);
}

void cpy_imm(CPU* cpu, CMem* cmem)
{
    _cpy(cpu, IMMEDIATE);
}

void cmp_x_ind(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, X_IND_CON);
}

void cpy_zpg(CPU* cpu, CMem* cmem)
{
    _cpy(cpu, ZPG_CON);
}

void cmp_zpg(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, ZPG_CON);
}

void dec_zpg(CPU* cpu, CMem* cmem)
{
    _dec(cpu ,ZPG_REG);
}

void iny(CPU* cpu, CMem* cmem)
{
    _inc(cpu, Y_REG);
}

void cmp_imm(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, IMMEDIATE);
}

void dex(CPU* cpu, CMem* cmem)
{
    _dec(cpu, X_REG);
}

void cpy_abs(CPU* cpu, CMem* cmem)
{
    _cpy(cpu, ABS_CON);
}

void cmp_abs(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, ABS_CON);
}

void dec_abs(CPU* cpu, CMem* cmem)
{
    _dec(cpu, ABS_REG);
}

void bne_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BNE_COND)
        REL_JMP;
}

void cmp_ind_y(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, IND_Y_CON);
}

void cmp_zpg_x(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, ZPG_X_CON);
}

void dec_zpg_x(CPU* cpu, CMem* cmem)
{
    _dec(cpu, ZPG_X_REG);
}

void cld(CPU* cpu, CMem* cmem)
{
    cpu->p &= 0xF7;
}

void cmp_abs_y(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, ABS_Y_CON);
}

void cmp_abs_x(CPU* cpu, CMem* cmem)
{
    _cmp(cpu, ABS_X_CON);
}

void dec_abs_x(CPU* cpu, CMem* cmem)
{
    _dec(cpu, ABS_X_REG);
}

void cpx_imm(CPU* cpu, CMem* cmem)
{
    _cpx(cpu, IMMEDIATE);
}

void sbc_x_ind(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, X_IND_CON);
}

void cpx_zpg(CPU* cpu, CMem* cmem)
{
    _cpx(cpu, ZPG_CON);
}

void sbc_zpg(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, ZPG_CON);
}

void inc_zpg(CPU* cpu, CMem* cmem)
{
    _inc(cpu, ZPG_REG);
}

void inx(CPU* cpu, CMem* cmem)
{
    _inc(cpu, X_REG);
}

void sbc_imm(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, IMMEDIATE);
}

void nop(CPU* cpu, CMem* cmem)
{
}

void cpx_abs(CPU* cpu, CMem* cmem)
{
    _cpx(cpu, ABS_CON);
}

void sbc_abs(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, ABS_CON);
}

void inc_abs(CPU* cpu, CMem* cmem)
{
    _inc(cpu, ABS_REG);
}

void beq_rel(CPU* cpu, CMem* cmem)
{
    INC_PC;
    if (BEQ_COND)
        REL_JMP;
}

void sbc_ind_y(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, IND_Y_CON);
}

void sbc_zpg_x(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, ZPG_X_CON);
}

void inc_zpg_x(CPU* cpu, CMem* cmem)
{
    _inc(cpu, ZPG_X_REG);
}

void sed(CPU* cpu, CMem* cmem)
{
    cpu->p |= 0x08;
}

void sbc_abs_y(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, ABS_Y_CON);
}

void sbc_abs_x(CPU* cpu, CMem* cmem)
{
    _sbc(cpu, ABS_X_CON);
}

void inc_abs_x(CPU* cpu, CMem* cmem)
{
    _inc(cpu, ABS_X_REG);
}




instruct instruction[0x100] = {
    brk, 
    ora_x_ind, 
    NULL,
    NULL,
    NULL,
    ora_zpg, 
    asl_zpg, 
    NULL,
    php, 
    ora_imm, 
    asl_a, 
    NULL,
    NULL,
    ora_abs, 
    asl_abs, 
    NULL,
    bpl_rel, 
    ora_ind_y, 
    NULL,
    NULL,
    NULL,
    ora_zpg_x, 
    asl_zpg_x, 
    NULL,
    clc, 
    ora_abs_y, 
    NULL,
    NULL,
    NULL,
    ora_abs_x, 
    asl_abs_x, 
    NULL,
    jsr_abs, 
    and_x_ind, 
    NULL,
    NULL,
    bit_zpg, 
    and_zpg, 
    rol_zpg, 
    NULL,
    plp, 
    and_imm, 
    rol_a, 
    NULL,
    bit_abs, 
    and_abs, 
    rol_abs, 
    NULL,
    bmi_rel, 
    and_ind_y, 
    NULL,
    NULL,
    NULL,
    and_zpg_x, 
    rol_zpg_x, 
    NULL,
    sec, 
    and_abs_y, 
    NULL,
    NULL,
    NULL,
    and_abs_x, 
    rol_abs_x, 
    NULL,
    rti, 
    eor_x_ind, 
    NULL,
    NULL,
    NULL,
    eor_zpg, 
    lsr_zpg, 
    NULL,
    pha, 
    eor_imm, 
    lsr_a, 
    NULL,
    jmp_abs, 
    eor_abs, 
    lsr_abs, 
    NULL,
    bvc_rel, 
    eor_ind_y, 
    NULL,
    NULL,
    NULL,
    eor_zpg_x, 
    lsr_zpg_x, 
    NULL,
    cli, 
    eor_abs_y, 
    NULL,
    NULL,
    NULL,
    eor_abs_x, 
    lsr_abs_x, 
    NULL,
    rts, 
    adc_x_ind, 
    NULL,
    NULL,
    NULL,
    adc_zpg, 
    ror_zpg, 
    NULL,
    pla, 
    adc_imm, 
    ror_a, 
    NULL,
    jmp_ind, 
    adc_abs, 
    ror_abs, 
    NULL,
    bvs_rel, 
    adc_ind_y, 
    NULL,
    NULL,
    NULL,
    adc_zpg_x, 
    ror_zpg_x, 
    NULL,
    sei, 
    adc_abs_y, 
    NULL,
    NULL,
    NULL,
    adc_abs_x, 
    ror_abs_x, 
    NULL,
    NULL,
    sta_x_ind, 
    NULL,
    NULL,
    sty_zpg, 
    sta_zpg, 
    stx_zpg, 
    NULL,
    dey, 
    NULL,
    txa, 
    NULL,
    sty_abs, 
    sta_abs, 
    stx_abs, 
    NULL,
    bcc_rel, 
    sta_ind_y, 
    NULL,
    NULL,
    sty_zpg_x, 
    sta_zpg_x, 
    stx_zpg_y, 
    NULL,
    tya, 
    sta_abs_y, 
    txs, 
    NULL,
    NULL,
    sta_abs_x, 
    NULL,
    NULL,
    ldy_imm, 
    lda_x_ind, 
    ldx_imm, 
    NULL,
    ldy_zpg, 
    lda_zpg, 
    ldx_zpg, 
    NULL,
    tay, 
    lda_imm, 
    tax, 
    NULL,
    ldy_abs, 
    lda_abs, 
    ldx_abs, 
    NULL,
    bcs_rel, 
    lda_ind_y, 
    NULL,
    NULL,
    ldy_zpg_x, 
    lda_zpg_x, 
    ldx_zpg_y, 
    NULL,
    clv, 
    lda_abs_y, 
    tsx, 
    NULL,
    ldy_abs_x, 
    lda_abs_x, 
    ldx_abs_y, 
    NULL,
    cpy_imm, 
    cmp_x_ind, 
    NULL,
    NULL,
    cpy_zpg, 
    cmp_zpg, 
    dec_zpg, 
    NULL,
    iny, 
    cmp_imm, 
    dex, 
    NULL,
    cpy_abs, 
    cmp_abs, 
    dec_abs, 
    NULL,
    bne_rel, 
    cmp_ind_y, 
    NULL,
    NULL,
    NULL,
    cmp_zpg_x, 
    dec_zpg_x, 
    NULL,
    cld, 
    cmp_abs_y, 
    NULL,
    NULL,
    NULL,
    cmp_abs_x, 
    dec_abs_x, 
    NULL,
    cpx_imm, 
    sbc_x_ind, 
    NULL,
    NULL,
    cpx_zpg, 
    sbc_zpg, 
    inc_zpg, 
    NULL,
    inx, 
    sbc_imm, 
    nop, 
    NULL,
    cpx_abs, 
    sbc_abs, 
    inc_abs, 
    NULL,
    beq_rel, 
    sbc_ind_y, 
    NULL,
    NULL,
    NULL,
    sbc_zpg_x, 
    inc_zpg_x, 
    NULL,
    sed, 
    sbc_abs_y, 
    NULL,
    NULL,
    NULL,
    sbc_abs_x, 
    inc_abs_x, 
    NULL
};

void run_instruction(CPU* cpu, CMem* cmem)
{
    instruction[cmem->all[cpu->c]](cpu, cmem);
    INC_PC;
}
