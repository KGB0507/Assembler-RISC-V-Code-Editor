#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <iostream>



using namespace std;


//Information about this Program Build. 
#define VERSION "Version 1.0"
#define AUTHOR "Copyright (c) Kirill Belozerov, 2021-2024. All Rights Reserved."

//#define DEBUG 1.0


#ifdef DEBUG
#define UNDERCONSTR "Under construction"
#define TESTCOMMAND "beq x4 x4 0"
#endif 



map<string, int> regMap =
{
    {"x0", 0}, {"zero", 0}, {"x1", 1}, {"ra", 1}, {"x2", 2}, {"sp", 2},
    {"x3", 3}, {"gp", 3}, {"x4", 4}, {"tp", 4}, {"x5", 5}, {"t0", 5},
    {"x6", 6}, {"t1", 6}, {"x7", 7}, {"t2", 7}, {"x8", 8}, {"s0", 8},
    {"x9", 9}, {"s1", 9}, {"x10", 10}, {"a0", 10}, {"x11", 11}, {"a1", 11},
    {"x12", 12}, {"a2", 12}, {"x13", 13}, {"a3", 13}, {"x14", 14}, {"a4", 14},
    {"x15", 15}, {"a5", 15}, {"x16", 16}, {"a6", 16}, {"x17", 17}, {"a7", 17},
    {"x18", 18}, {"s2", 18}, {"x19", 19}, {"s3", 19}, {"x20", 20}, {"s4", 20},
    {"x21", 21}, {"s5", 21}, {"x22", 22}, {"s6", 22}, {"x23", 23}, {"s7", 23},
    {"x24", 24}, {"s8", 24}, {"x25", 25}, {"s9", 25}, {"x26", 26}, {"s10", 26},
    {"x27", 27}, {"s11", 27}, {"x28", 28}, {"t3", 28}, {"x29", 29}, {"t4", 29},
    {"x30", 30}, {"t5", 30}, {"x31", 31}, {"t6", 31}
};

string DecToHex(int n)
{
    stringstream ss;
    ss << setfill('0') << setw(8) << hex << n;
    return ss.str();
}

string GetRegisterName(int regNum)
{
    /*
    static const string registerNames[] =
    {
        "x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
        "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3",
        "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4",
        "t5", "t6"
    };*/
    
    static const string registerNames[] =
    {
        "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9",
        "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x19",
        "x20", "x21", "x22", "x23", "x24", "x25", "x26", "x27", "x28", "x29",
        "x30", "x31"
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
        bool isNegative = imm & (1 << 11);
        if (isNegative) 
        {
            imm = (~imm + 1) & 0xFFF;
        }

        return "lw " + GetRegisterName(rd) + ", " + (isNegative ? "-" : "") + to_string(imm) + "(" + GetRegisterName(rs1) + ")";
    }
    case 0x23: // sw ?
    {
        unsigned int rs2 = (instr >> 20) & 0x1F;
        unsigned int rs1 = (instr >> 15) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x2)
        {
            return "Invalid instruction";
        }
        int imm = static_cast<int>(instr >> 25 << 5 | instr >> 7 & 0x1F);
        bool isNegative = imm & (1 << 11);
        if (isNegative)
        {
            imm = (~imm + 1) & 0xFFF;
        }

        return "sw " + GetRegisterName(rs2) + ", " + (isNegative ? "-" : "") + to_string(imm) + "(" + GetRegisterName(rs1) + ")";
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
    case 0x13: // addi ?
    {
        unsigned int rd = (instr >> 7) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x0)
        {
            return "Invalid instruction";
        }
        unsigned int rs1 = (instr >> 15) & 0x1F;
        int imm = static_cast<int>(instr >> 20);
        bool isNegative = imm & (1 << 11);
        if (isNegative)
        {
            imm = (~imm + 1) & 0xFFF;
        }
        
        return "addi " + GetRegisterName(rd) + ", " + GetRegisterName(rs1) + ", " + (isNegative ? "-" : "") + to_string(imm);
    }
    case 0x6F: // jal 
    {
        unsigned int rd = (instr >> 7) & 0x1F;
        int imm = static_cast<int>(instr >> 21 << 1); // imm[20|10:1|11|19:12]
        bool isNegative = imm & (1 << 11);
        if (isNegative)
        {
            imm = (~imm + 1) & 0xFFF;
        }

        return "jal " + GetRegisterName(rd) + ", " + (isNegative ? "-" : "") + to_string(imm);
    }
    case 0x63: // beq ?
    {
        unsigned int rs1 = (instr >> 15) & 0x1F;
        unsigned int rs2 = (instr >> 20) & 0x1F;
        unsigned int funct3 = (instr >> 12) & 0x7;
        if (funct3 != 0x0)
        {
            return "Invalid instruction";
        }
        int imm = static_cast<int>(instr >> 31 << 12 | instr >> 25 << 5 | ((instr >> 8) & 0xF) << 1 | instr >> 7 & 0x1 << 11); // imm[12|10:5|4:1|11]
        bool isNegative = imm & (1 << 11);
        if (isNegative)
        {
            imm = (~imm + 1) & 0xFFF;
        }
        
        return "beq " + GetRegisterName(rs1) + ", " + GetRegisterName(rs2) + ", " + (isNegative ? "-" : "") + to_string(imm);
    }
    default:
        return "Invalid instruction";
    }
}



int main()
{
    cout << "Assembler RISC-V Code Editor " << VERSION << endl;
    cout << AUTHOR << endl;
#ifdef DEBUG
    cout << UNDERCONSTR << endl << endl;
#endif

    string filename, line, text = "";
    char choice;
    vector<string> lines;

    //cout << "Text Editor\n";
    cout << "\n\nSupported instructions: lw, sw, or, beq, jal, addi.\n";
    cout << "Do not use commas (',') and brackets ('(...)') for correct instruction decoding!\nExample: lw x6 -4 x9.\n\n";
    cout << "1. Create new file\n2. Open file\nChoice: ";

#ifndef DEBUG
    cin >> choice;
#else
    choice = '1';
    cout << endl;
#endif

    if (choice == '1') 
    {
        cout << "Enter filename: ";

#ifndef DEBUG
        cin >> filename;
#else
        filename = "2.asm";
        cout << endl;
#endif  
    }
    else if (choice == '2') 
    {
        cout << "Enter filename: ";

#ifndef DEBUG
        cin >> filename;
#else
        filename = "2.asm";
        cout << endl;
#endif 
        
        ifstream file(filename);
        if (file.is_open()) 
        {
            while (getline(file, line)) 
            {
                text += DecodeMachineCode(line) + "\n";
                lines.push_back(line);
            }
            file.close();
        }
        else 
        {
            cout << "Unable to open file\n";
            return 0;
        }
    }
    else 
    {
        cout << "Invalid choice\n";
        return 0;
    }

    cout << "Text:\n" << text;

    while (true) 
    {
        cout << "\n1. Insert line\n2. Delete line\n3. Save and exit\nChoice: ";

#ifndef DEBUG
        cin >> choice;
#else
        choice = '1';
        cout << endl;
#endif     

        if (choice == '1') 
        {
            cout << "Enter line: ";
            //cin >> line;
#ifndef DEBUG
            cin.ignore();
#endif
            getline(cin, line);

            // Разбиваем строку на инструкцию и операнды
            istringstream iss(line);
            string instr, op1, op2, op3;
            iss >> instr >> op1 >> op2 >> op3;

            // Транслируем инструкцию в машинный код
            int opcode, rd, rs1, rs2, funct3, funct7, imm;
            string machineCode;

            if (instr == "lw") 
            {
                opcode = 0x03;
                rd = regMap[op1];
                funct3 = 0x2;
                istringstream iss(op2);
                iss >> imm >> op3;
                rs1 = regMap[op3];
                machineCode = DecToHex(((imm & 0xFFF) << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode);
            }
            else if (instr == "sw") 
            {
                opcode = 0x23;
                rs2 = regMap[op1];
                funct3 = 0x2;
                istringstream iss(op2);
                iss >> imm >> op3;
                rs1 = regMap[op3];
                machineCode = DecToHex((((imm & 0xFE0) >> 5) << 25) | (rs1 << 15) | (rs2 << 20) | (funct3 << 12) | ((imm & 0x01F) << 7) | opcode);
            }
            else if (instr == "or") 
            {
                opcode = 0x33;
                rd = regMap[op1];
                rs1 = regMap[op2];
                rs2 = regMap[op3];
                funct3 = 0x6;
                funct7 = 0x0;
                machineCode = DecToHex((funct7 << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode);

            }
            else if (instr == "addi")
            {
                opcode = 0x13;
                rd = regMap[op1];
                rs1 = regMap[op2];
                istringstream iss(op3);
                iss >> imm;
                funct3 = 0x0;
                machineCode = DecToHex(((imm & 0xFFF) << 20) | (rs1 << 15) | (funct3 << 12) | (rd << 7) | opcode);
            }
            else if (instr == "jal")
            {
                opcode = 0x6F;
                rd = regMap[op1];
                istringstream iss(op2);
                iss >> imm;
                machineCode = DecToHex((((imm & 0x100000) >> 20) << 31) | (((imm & 0x7FE) >> 1) << 21) | (((imm & 0x800) >> 11) << 20) | (((imm & 0xFF000) >> 12) << 12) | (rd << 7) | opcode);
            }
            else if (instr == "beq") 
            {
                opcode = 0x63;
                rs1 = regMap[op1];
                rs2 = regMap[op2];
                istringstream iss(op3);
                iss >> imm;
                funct3 = 0x0;
                machineCode = DecToHex((((imm & 0x1000) >> 12) << 31) | (((imm & 0x07E0) >> 5) << 25) | (rs2 << 20) | (rs1 << 15) | (funct3 << 12) | (((imm & 0x01E) >> 1) << 8) | (((imm & 0x800) >> 11) << 7) | opcode); 
            }
            else 
            {
                cout << "Invalid instruction\n";
                continue;
            }

            //text += line + " - " + machineCode + "\n";
            lines.push_back(machineCode);
            text = "";
            for (string line : lines) 
            {
                text += DecodeMachineCode(line) + "\n";
            }

            cout << "Text:\n" << text;
        }
        else if (choice == '2') 
        {
            if (lines.empty()) 
            {
                cout << "Text is empty\n";
            }
            else {
                cout << "Text:\n" << text;
                cout << "Enter line number to delete: ";
                int lineNum;
                cin >> lineNum;
                if (lineNum < 1 || lineNum > lines.size()) 
                {
                    cout << "Invalid line number\n";
                }
                else 
                {
                    lines.erase(lines.begin() + lineNum - 1);
                    text = "";
                    for (string line : lines) 
                    {
                        text += DecodeMachineCode(line) + "\n";
                    }
                    cout << "Text:\n" << text;
                }
            }
        }
        else if (choice == '3') 
        {
            ofstream file(filename);
            if (file.is_open()) 
            {
                for (string line : lines) 
                {
                    file << line << "\n";
                }
                file.close();
                cout << "File saved successfully\n";
            }
            else 
            {
                cout << "Unable to save file\n";
            }
            break;
        }
        else
        {
            cout << "Invalid choice\n";
        }
    }

    return 0;
}
