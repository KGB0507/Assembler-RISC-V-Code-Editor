#include <iostream>

using namespace std;


//Information about this Program Build. 
#define VERSION "Version 1.0"
#define AUTHOR "Copyright (c) Kirill Belozerov, 2021-2024. All Rights Reserved."

#define DEBUG 1.0


#ifdef DEBUG
#define UNDERCONSTR "Under construction"
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

    cout << "Text Editor\n";
    cout << "1. Create new file\n2. Open file\nChoice: ";
    cin >> choice;

    if (choice == '1') {
        cout << "Enter filename: ";
        cin >> filename;
    }
    else if (choice == '2') 
    {
        cout << "Enter filename: ";
        cin >> filename;
        ifstream file(filename);
        if (file.is_open()) 
        {
            while (getline(file, line)) 
            {
                text += line + "\n";
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
        cout << "1. Insert line\n2. Delete line\n3. Save and exit\nChoice: ";
        cin >> choice;

        if (choice == '1') 
        {
            cout << "Enter line: ";
            cin.ignore();
            getline(cin, line);
            text += line + "\n";
            cout << "Text:\n" << text;
        }
        else if (choice == '2') 
        {
            if (text.empty()) 
            {
                cout << "Text is empty\n";
            }
            else {
                cout << "Text:\n" << text;
                cout << "Enter line number to delete: ";
                int lineNum;
                cin >> lineNum;
                if (lineNum < 1 || lineNum > count(text.begin(), text.end(), '\n')) 
                {
                    cout << "Invalid line number\n";
                }
                else 
                {
                    int pos = 0;
                    for (int i = 1; i < lineNum; i++) 
                    {
                        pos = text.find('\n', pos) + 1;
                    }
                    int endPos = text.find('\n', pos);
                    if (endPos == -1) 
                    {
                        text.erase(pos);
                    }
                    else {
                        text.erase(pos, endPos - pos + 1);
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
                file << text;
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
