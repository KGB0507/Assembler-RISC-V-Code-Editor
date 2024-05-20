#include "Subroutines.h"


using namespace std;


//Information about this Program Build. 
#define VERSION "Version 1.0"
#define AUTHOR "Copyright (c) Kirill Belozerov, 2021-2024. All Rights Reserved."

#define DEBUG 1.0


#ifdef DEBUG
#define UNDERCONSTR "Under construction"
#define TESTCOMMAND "beq x4 x4 0"
#endif 





int main()
{
    cout << "Assembler RISC-V Code Editor " << VERSION << std::endl;
    cout << AUTHOR << std::endl;
#ifdef DEBUG
    cout << UNDERCONSTR << std::endl << std::endl;
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
#endif

    if (choice == '1') 
    {
        cout << "Enter filename: ";

#ifndef DEBUG
        cin >> filename;
#else
        filename = "example.asm";
#endif  
    }
    else if (choice == '2') 
    {
        cout << "Enter filename: ";

#ifndef DEBUG
        cin >> filename;
#else
        filename = "example.asm";
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
#endif     

        if (choice == '1') 
        {
            cout << "Enter line: ";
            //cin >> line;
            cin.ignore();
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
                machineCode = DecToHex(((imm & 0xFE0) << 25) | (rs1 << 15) | (rs2 << 20) | (funct3 << 12) | ((imm & 0x01F) << 7) | opcode);
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
