#include "Subroutines.h"



string DecToHex(int n)
{
    stringstream ss;
    ss << setfill('0') << setw(8) << hex << n;
    return ss.str();
}

string GetRegisterName(int regNum)
{
    static const string registerNames[] =
    {
        "x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
        "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
        "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4",
        "t5", "t6"
    };

    if (regNum >= 0 && regNum < 32)
    {
        return registerNames[regNum];
    }
    else
    {
        return "invalid";
    }
}

string DecodeMachineCode(string machineCode)
{
    unsigned int instr;
    stringstream ss;
    ss << hex << machineCode;
    ss >> instr;

    unsigned int opcode = instr & 0x7F;
    switch (opcode) 
    {
    case 0x03: // lw
    {
        unsigned int rd = (instr >> 7) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x2) 
        {
            return "Invalid instruction";
        }
        unsigned int rs1 = (instr >> 15) & 0x1F;
        int imm = static_cast<int>(instr >> 20);
        return "lw " + GetRegisterName(rd) + ", " + to_string(imm) + "(" + GetRegisterName(rs1) + ")";
    }
    case 0x23: // sw
    {
        unsigned int rs2 = (instr >> 20) & 0x1F;
        unsigned int rs1 = (instr >> 15) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x2) 
        {
            return "Invalid instruction";
        }
        int imm = static_cast<int>(instr >> 25 << 5 | instr >> 7 & 0x1F);
        return "sw " + GetRegisterName(rs2) + ", " + to_string(imm) + "(" + GetRegisterName(rs1) + ")";
    }
    case 0x33: // or
    {
        unsigned int rd = (instr >> 7) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x6) 
        {
            return "Invalid instruction";
        }
        unsigned int rs1 = (instr >> 15) & 0x1F;
        unsigned int rs2 = (instr >> 20) & 0x1F;
        return "or " + GetRegisterName(rd) + ", " + GetRegisterName(rs1) + ", " + GetRegisterName(rs2);
    }
    case 0x13: // addi
    {
        unsigned int rd = (instr >> 7) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x0) 
        {
            return "Invalid instruction";
        }
        unsigned int rs1 = (instr >> 15) & 0x1F;
        int imm = static_cast<int>(instr >> 20);
        return "addi " + GetRegisterName(rd) + ", " + GetRegisterName(rs1) + ", " + to_string(imm);
    }
    case 0x6F: // jal
    {
        unsigned int rd = (instr >> 7) & 0x1F;
        int imm = static_cast<int>(instr >> 21 << 1); // imm[20|10:1|11|19:12]
        return "jal " + GetRegisterName(rd) + ", " + to_string(imm);
    }
    case 0x63: // beq
    {
        unsigned int rs1 = (instr >> 15) & 0x1F;
        unsigned int rs2 = (instr >> 20) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x0) 
        {
            return "Invalid instruction";
        }
        int imm = static_cast<int>(instr >> 25 << 5 | instr >> 8 & 0xF << 1 | instr >> 7 & 0x1 << 11); // imm[12|10:5|4:1|11]
        return "beq " + GetRegisterName(rs1) + ", " + GetRegisterName(rs2) + ", " + to_string(imm);
    }
    default:
        return "Invalid instruction";
    }
}
