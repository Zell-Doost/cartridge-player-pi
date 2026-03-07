#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t all[65536];
} Mem;

typedef struct {
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t s;
    uint8_t p;
    uint16_t c;
} CPU;

void run_instruction(CPU* cpu, Mem* mem);

#define INC_PC cpu->c++

#define MSB(reg) (reg & 0x80)
#define LSB(reg) (reg & 0x01)

#define SREG_ZERO(reg) (cpu->p = (reg) ? cpu->p & 0xFD : cpu->p | 0x02)
#define SREG_NEG(reg) (cpu->p = MSB(reg) ? cpu->p | 0x80 : cpu->p & 0x7F)
#define SREG_CAR_LS(reg) (cpu->p = (MSB(reg) ? cpu->p | 0x01 : cpu->p & 0xFE))
#define SREG_CAR_RS(reg) (cpu->p = LSB(reg) ? cpu->p | 0x01 : cpu->p & 0xFE)
#define SREG_CAR_ADC(reg) (cpu->p = (reg & 0x100) ? cpu->p | 0x01 : cpu->p & 0xFE)
#define SREG_CAR_SBC(reg) (cpu->p = (reg & 0x100) ? cpu->p & 0xFE : cpu->p | 0x01)
#define SREG_OVF(a1, a2, sum) (cpu->p = ((a1 ^ sum) & (a2 ^ sum) & 0x80) ? cpu->p | 0x40 : cpu->p & 0xBF)

#define LOGIC_SREG(reg) (SREG_ZERO(reg), SREG_NEG(reg)) //Also used in shifting, loading and transfers
#define ARITH_SREG_ADC(reg, reg16) (SREG_ZERO(reg), SREG_NEG(reg), SREG_CAR_ADC(reg16))
#define ARITH_SREG_SBC(reg, reg16) (SREG_ZERO(reg), SREG_NEG(reg), SREG_CAR_SBC(reg16))

//OPTIMISE COMBINING 2 BYTES USING A UNION (ENDIAN-DEPENDANT)


#define ABS_ADDR ({                         \
        uint8_t lo = mem->all[++cpu->c];    \
        uint16_t hi = mem->all[++cpu->c];   \
        lo | (hi << 8);                     \
        })
#define ABS_X_ADDR ({                        \
        uint8_t lo = mem->all[++cpu->c];    \
        uint16_t hi = mem->all[++cpu->c];   \
        ((lo | (hi << 8)) + cpu->x) & 0xFFFF;\
        })
#define ABS_Y_ADDR ({                        \
        uint8_t lo = mem->all[++cpu->c];    \
        uint16_t hi = mem->all[++cpu->c];   \
        ((lo | (hi << 8)) + cpu->y) & 0xFFFF;\
        })

#define X_IND_ADDR ({                                                   \
        uint8_t lo = mem->all[0xFF & (mem->all[++cpu->c] + cpu->x)];    \
        uint16_t hi = mem->all[0xFF & (mem->all[cpu->c] + cpu->x + 1)]; \
        lo | (hi << 8);                                           \
        })
#define IND_Y_ADDR ({                                               \
        uint8_t lo = mem->all[0xFF & (mem->all[++cpu->c])];        \
        uint16_t hi = mem->all[0xFF & (mem->all[cpu->c] + 1)];     \
        ((lo | (hi << 8)) + cpu->y) & 0xFFFF;                       \
        })
#define REL_JMP ({                                \
    int8_t offset = mem->all[cpu->c];           \
    cpu->c += offset;                             \
})



#define CUR_STK mem->all[cpu->s + 0x100]
#define ACC_CON cpu->a
#define STS_CON cpu->p
#define IMMEDIATE mem->all[++cpu->c] 
#define ZPG_CON mem->all[mem->all[++cpu->c]]
#define ZPG_X_CON mem->all[(mem->all[++cpu->c] + cpu->x) & 0x00FF]
#define ZPG_Y_CON mem->all[(mem->all[++cpu->c] + cpu->y) & 0x00FF]
#define ABS_CON mem->all[ABS_ADDR]
#define ABS_X_CON mem->all[ABS_X_ADDR]
#define ABS_Y_CON mem->all[ABS_Y_ADDR]
#define X_IND_CON mem->all[X_IND_ADDR]
#define IND_Y_CON mem->all[IND_Y_ADDR]
#define IND_CON

#define ACC_REG &(ACC_CON)
#define X_REG &(cpu->x)
#define Y_REG &(cpu->y)
#define STS_REG &(STS_CON)
#define ZPG_REG &(ZPG_CON)
#define ZPG_X_REG &(ZPG_X_CON)
#define ZPG_Y_REG &(ZPG_Y_CON)
#define ABS_REG &(ABS_CON)
#define ABS_X_REG &(ABS_X_CON)
#define ABS_Y_REG &(ABS_Y_CON)
#define X_IND_REG &(X_IND_CON)
#define IND_Y_REG &(IND_Y_CON)

#define BCC_COND !(cpu->p & 0x01)
#define BCS_COND cpu->p & 0x01
#define BEQ_COND cpu->p & 0x02
#define BMI_COND cpu->p & 0x80
#define BNE_COND !(cpu->p & 0x02)
#define BPL_COND !(cpu->p & 0x80)
#define BVC_COND !(cpu->p & 0x40)
#define BVS_COND cpu->p & 0x40

//Subtract 1 in BRK_VECT to account for PC_INC after all instructions.
#define BRK_VECT (mem->all[0xFFFE] | ((uint16_t)mem->all[0xFFFF] << 8)) - 1

//cpu instruction function declarations
//void brk(CPU* cpu);
//void ora_x_ind(CPU* cpu);
//void ora_zpg(CPU* cpu);
//void asl_zpg(CPU* cpu);
//void php(CPU* cpu);
//void ora_imm(CPU* cpu);
//void asl_a(CPU* cpu);
//void ora_abs(CPU* cpu);
//void asl_abs(CPU* cpu);
//void bpl_rel(CPU* cpu);
//void ora_ind_y(CPU* cpu);
//void ora_zpg_x(CPU* cpu);
//void asl_zpg_x(CPU* cpu);
//void clc(CPU* cpu);
//void ora_abs_y(CPU* cpu);
//void ora_abs_x(CPU* cpu);
//void asl_abs_x(CPU* cpu);
//void jsr_abs(CPU* cpu);
//void and_x_ind(CPU* cpu);
//void bit_zpg(CPU* cpu);
//void and_zpg(CPU* cpu);
//void rol_zpg(CPU* cpu);
//void plp(CPU* cpu);
//void and_imm(CPU* cpu);
//void rol_a(CPU* cpu);
//void bit_abs(CPU* cpu);
//void and_abs(CPU* cpu);
//void rol_abs(CPU* cpu);
//void bmi_rel(CPU* cpu);
//void and_ind_y(CPU* cpu);
//void and_zpg_x(CPU* cpu);
//void rol_zpg_x(CPU* cpu);
//void sec(CPU* cpu);
//void and_abs_y(CPU* cpu);
//void and_abs_x(CPU* cpu);
//void rol_abd_x(CPU* cpu);
//void rti(CPU* cpu);
//void eor_x_ind(CPU* cpu);
//void eor_zpg(CPU* cpu);
//void lsr_zpg(CPU* cpu);
//void pha(CPU* cpu);
//void eor_imm(CPU* cpu);
//void lsr_a(CPU* cpu);
//void jmp_abs(CPU* cpu);
//void eor_abs(CPU* cpu);
//void lsr_abs(CPU* cpu);
//void bvc_rel(CPU* cpu);
//void eor_ind_y(CPU* cpu);
//void eor_zpg_x(CPU* cpu);
//void lsr_zpg_x(CPU* cpu);
//void cli(CPU* cpu);
//void eor_abs_y(CPU* cpu);
//void eor_abs_x(CPU* cpu);
//void lsr_abs_x(CPU* cpu);
//void rts(CPU* cpu);
//void adc_x_ind(CPU* cpu);
//void adc_zpg(CPU* cpu);
//void ror_zpg(CPU* cpu);
//void pla(CPU* cpu);
//void adc_imm(CPU* cpu);
//void ror_a(CPU* cpu);
//void jmp_ind(CPU* cpu);
//void adc_abs(CPU* cpu);
//void ror_abs(CPU* cpu);
//void bvs_rel(CPU* cpu);
//void adc_ind_y(CPU* cpu);
//void adc_zpg_x(CPU* cpu);
//void ror_zpg_x(CPU* cpu);
//void sei(CPU* cpu);
//void adc_abs_y(CPU* cpu);
//void adc_abs_x(CPU* cpu);
//void ror_abs_x(CPU* cpu);
//void sta_x_ind(CPU* cpu);
//void sty_zpg(CPU* cpu);
//void sta_zpg(CPU* cpu);
//void stx_zpg(CPU* cpu);
//void dey(CPU* cpu);
//void txa(CPU* cpu);
//void sty_abs(CPU* cpu);
//void sta_abs(CPU* cpu);
//void stx_abs(CPU* cpu);
//void bcc_rel(CPU* cpu);
//void sta_ind_y(CPU* cpu);
//void sty_zpg_x(CPU* cpu);
//void sta_zpg_x(CPU* cpu);
//void stx_zpg_y(CPU* cpu);
//void tya(CPU* cpu);
//void sta_abs_y(CPU* cpu);
//void txs(CPU* cpu);
//void sta_abs_x(CPU* cpu);
//void ldy_imm(CPU* cpu);
//void lda_x_ind(CPU* cpu);
//void ldx_imm(CPU* cpu);
//void ldy_zpg(CPU* cpu);
//void lda_zpg(CPU* cpu);
//void ldx_zpg(CPU* cpu);
//void tay(CPU* cpu);
//void lda_imm(CPU* cpu);
//void tax(CPU* cpu);
//void ldy_abs(CPU* cpu);
//void lda_abs(CPU* cpu);
//void ldx_abs(CPU* cpu);
//void bcs_rel(CPU* cpu);
//void lda_ind_y(CPU* cpu);
//void ldy_zpg_x(CPU* cpu);
//void lda_zpg_x(CPU* cpu);
//void ldx_zpg_y(CPU* cpu);
//void clv(CPU* cpu);
//void lda_abs_y(CPU* cpu);
//void tsx(CPU* cpu);
//void ldy_abs_x(CPU* cpu);
//void lda_abs_x(CPU* cpu);
//void ldx_abs_y(CPU* cpu);
//void cpy_imm(CPU* cpu);
//void cmp_x_ind(CPU* cpu);
//void cpy_zpg(CPU* cpu);
//void cmp_zpg(CPU* cpu);
//void dec_zpg(CPU* cpu);
//void iny(CPU* cpu);
//void cmp_imm(CPU* cpu);
//void dex(CPU* cpu);
//void cpy_abs(CPU* cpu);
//void cmp_abs(CPU* cpu);
//void dec_abs(CPU* cpu);
//void bne_rel(CPU* cpu);
//void cmp_ind_y(CPU* cpu);
//void cmp_zpg_x(CPU* cpu);
//void dec_zpg_x(CPU* cpu);
//void cld(CPU* cpu);
//void cmp_abs_y(CPU* cpu);
//void cmp_abs_x(CPU* cpu);
//void dec_abs_x(CPU* cpu);
//void cpx_imm(CPU* cpu);
//void sbc_x_ind(CPU* cpu);
//void cpx_zpg(CPU* cpu);
//void sbc_zpg(CPU* cpu);
//void inc_zpg(CPU* cpu);
//void inx(CPU* cpu);
//void sbc_imm(CPU* cpu);
//void nop(CPU* cpu);
//void cpx_abs(CPU* cpu);
//void sbc_abs(CPU* cpu);
//void inc_abs(CPU* cpu);
//void beq_rel(CPU* cpu);
//void sbc_ind_y(CPU* cpu);
//void sbc_zpg_x(CPU* cpu);
//void inc_zpg_x(CPU* cpu);
//void sed(CPU* cpu);
//void sbc_abs_y(CPU* cpu);
//void sbc_abs_x(CPU* cpu);
//void inc_abs_x(CPU* cpu);
