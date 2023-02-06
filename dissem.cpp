//CS 530 Assignment 1
//Jack Cairel RED ID 823197473
//Erik Nurja  RED ID 823233990
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <iomanip>
#include <vector>
#include <sstream>
using namespace std;
ifstream obj_Stream;
ifstream sym_Stream;
//Declare maps for sym/lit tables and mnemonics, ints for registers and length of program
map<int, string> symTable;
map<int, string> litTable;
int baseReg;
int xReg;
int pcReg;
int progLen;
//Open file for output
ofstream lstOut("out.lst");

class Dissem{
public:
    void dissem();
    void load_Data(char *obj_File, char *sim_File);
private:
    vector<string> object_Storage;
    vector<string> symbol_Storage;
}
;
//Function to format the address being printed out, takes int of the current address
string printAddress(int address){
    string format;
    stringstream s;
    s << setfill('0')<< setw(4)<<hex<< address;
    format = s.str();
    for (int i = 0; i < format.length(); i++) {
        format[i] = toupper(format[i]);
    }
    return format;
}
//Function to translate header record lines
void headRecord(string hRecord){
    //H records have set format, read in starting address and program name
    lstOut<< hRecord.substr(9,4)<< "\t" << hRecord.substr(1,6) <<"\t START \t 0\n";
    //Read in program length
    progLen = stoi(hRecord.substr(13,6),nullptr,16);
}
//Function to translate end record lines
void endRecord(string eRecord){
    //Check for any final RESW instructions
    if (progLen != pcReg) {
        int resw = 0;
        for (pair<int, string> sym : symTable){
            if (pcReg < sym.first && sym.first < progLen) {
                resw = (sym.first - pcReg) / 3;
                lstOut<< printAddress(pcReg) << "\t" << symTable.at(pcReg) << "\t RESW \t" << dec<< resw << "\n";
                pcReg = sym.first;
            }
        }
        resw = (progLen - pcReg) / 3;
        if (symTable.find(pcReg) != symTable.end()) {
            lstOut<< printAddress(pcReg) << "\t" << symTable.at(pcReg) << "\t RESW \t" << dec<< resw << "\n";
        }
        }
    //Print out the set format
    lstOut<< "\t\t\t END \t";
    //Check for a start symbol tp print out
    int progStart = stoi(eRecord.substr(1,6), nullptr, 16);
    if (symTable.find(progStart) != symTable.end()){
        lstOut<< symTable.at(progStart) << "\n";
    }
    else {
        lstOut<< progStart << "\n";
    }
}
//Function to translate format 2 instructions, takes a string of the instruction name
//and the full opcode, and an int of the current address
void format2(string opName, string opCode){
    lstOut<< printAddress(pcReg) << "\t\t\t";
    lstOut<< opName << "\t";
    char r1 = opCode.at(2);
    char r2 = opCode.at(3);
    //Print register operand names
    switch (r1) {
        case '0':
            lstOut<< "A";
            break;
        case '1':
            lstOut<< "X";
            break;
        case '2':
            lstOut<< "L";
            break;
        case '3':
            lstOut<< "B";
            break;
        case '4':
            lstOut<< "S";
            break;
        case '5':
            lstOut<< "T";
            break;
        case '6':
            lstOut<< "F";
        default:
            break;
    }
    //Check if only one register operation
    if (opName == "CLEAR" || opName == "SVC"){  lstOut<< "\t";}
    //Print out 2nd register operand
    else{
        switch (r2) {
            case '0':
                lstOut<< ",A";
                break;
            case '1':
                lstOut<< ",X";
                break;
            case '2':
                lstOut<< ",L";
                break;
            case '3':
                lstOut<< ",B";
                break;
            case '4':
                lstOut<< ",S";
                break;
            case '5':
                lstOut<< ",T";
                break;
            case '6':
                lstOut<< ",F";
            default:
                break;
        }
    }
    lstOut<< opCode << "\n";
}
//Function to translate format 3/4 instructions, takes strings of the instruction name
//and the full instruction opcode, and ints of the flag bits and current address
void format34(string opName, string opCode,int nixbpe){
    if (opName == "RSUB") {
        lstOut<< printAddress(pcReg) << "\t\t RSUB \t\t" << opCode << "\n";
        return;
    }
    signed int targetAdd = 0x00;
    lstOut<< printAddress(pcReg) << "\t";
    //Check if address matches sym table, print symbol
    if (symTable.find(pcReg) != symTable.end()){
        lstOut<< symTable.at(pcReg) << "\t";
    }
    else{
        lstOut<< "\t\t";
    }
    //Check for extended format
    if (nixbpe & 0x01){
        lstOut<< "+";
        targetAdd = stoi((opCode.substr(3,5)),nullptr,16);
    }
    else{
        targetAdd = stoi((opCode.substr(3,3)),nullptr,16);
        //Check if disp is a negative number
        if (targetAdd & 0x800){
            targetAdd = targetAdd - 0x1000;
        }
    }
    //Print out instruction name
    lstOut << opName<< " \t";
    //Check flag bits, simple addressing
    if ((nixbpe & 0x20)&&(nixbpe & 0x10)) {
        //This skips over the other checks
    }//Indirect addressing
    else if (nixbpe & 0x20){
        lstOut<< "@";
    }//Immediate addressing
    else{
        lstOut<< "#";
    }//Base relative
    if (nixbpe & 0x04){
        targetAdd += baseReg;
    }//PC relative
    else if (nixbpe & 0x02){
        targetAdd += pcReg + 0x03;
    }//Indexed addressing
    if (nixbpe & 0x08){
        targetAdd += xReg;
    }
    //Check to print target address from Sym/Lit table
    if (targetAdd != 0 && symTable.find(targetAdd) != symTable.end()){
        lstOut<< (symTable.at(targetAdd));
    }
    else if (litTable.find(targetAdd) != litTable.end()){
        lstOut<< (litTable.at(targetAdd));
    }
    else if (nixbpe & 0x10){
        lstOut << targetAdd;
    }
    if (nixbpe & 0x08){
        lstOut<< ",X";
    }
    lstOut<< " \t" << opCode << "\n";
    //Set base register
    if (opName == "LDB") {
        baseReg = targetAdd;
        lstOut<< "\t\t\tBASE \t";
        if (symTable.find(targetAdd) != symTable.end()){
            lstOut<< symTable.at(targetAdd);
        }
        lstOut<< "\n";
    }
    //Set X register
    if (opName == "LDX") {
        xReg = targetAdd;
    }
    
}
//Function to translate text record lines
void textRecord(string tRecord){
    //Map to find mnemonics for format 3/4 instructions
    unordered_map<int, string> ops34 = {
        {0x18,"ADD"},{0x58,"ADDF"},{0x40,"AND"},
        {0x28,"COMP"},{0x88,"COMPF"},{0x24,"DIV"},{0x64,"DIVF"},
        {0xC4,"FIX"},{0xC0,"FLOAT"},{0xF4,"HIO"},{0x3C,"J"},{0x30,"JEQ"},
        {0x34,"JGT"},{0x38,"JLT"},{0x48,"JSUB"},{0x00,"LDA"},{0x68,"LDB"},{0x50,"LDCH"},
        {0x70,"LDF"},{0x08,"LDL"},{0x6C,"LDS"},{0x74,"LDT"},{0x04,"LDX"},{0xD0,"LPS"},
        {0x20,"MUL"},{0x60,"MULF"},{0xC8,"NORM"},{0x44,"OR"},{0xD8,"RD"},
        {0x4C,"RSUB"},{0xF0,"SIO"},{0xEC,"SSK"},{0x0C,"STA"},{0x78,"STB"},
        {0x54,"STCH"},{0x80,"STF"},{0xD4,"STI"},{0x14,"STL"},{0x7C,"STS"},
        {0xE8,"STSW"},{0x84,"STT"},{0x10,"STX"},{0x1C,"SUB"},{0x5C,"SUBF"},
        {0xE0,"TD"},{0xF8,"TIO"},{0x2C,"TIX"},{0xDC,"WD"}
    };
    //Map to find mnemonics for format 2 instructions
    unordered_map<int, string> ops2 = {
        {0x90,"ADDR"},{0xB4,"CLEAR"},{0xA0,"COMPR"},{0x9C,"DIVR"},{0x98,"MULR"},
        {0xAC,"RMO"},{0xA4,"SHIFTL"},{0xA8,"SHIFTR"},{0x94,"SUBR"},{0xB0,"SVC"},
        {0xB8,"TIXR"}
    };
    //T col 2-7 starting address
    int currAddress = stoi(tRecord.substr(1,6),nullptr,16);
    //Check for any RESW addresses
    if (currAddress != pcReg) {
        int resw = 0;
        for (pair<int, string> sym : symTable){
            if (pcReg < sym.first && sym.first < currAddress) {
                resw = (sym.first - pcReg) / 3;
                lstOut<< printAddress(pcReg) << "\t" << symTable.at(pcReg) << "\t RESW \t" << dec<< resw << "\n";
                pcReg = sym.first;
            }
        }
        resw = (currAddress - pcReg) / 3;
        if (symTable.find(pcReg) != symTable.end()) {
            lstOut<< printAddress(pcReg) << "\t" << symTable.at(pcReg) << "\t RESW \t" << dec<< resw << "\n";
        }
        pcReg = currAddress;
        }
    //T col 10 start of instructions
    int i = 9;
    string opCode;
    while (i < tRecord.size() && tRecord.substr(i,2) != "\r") {
        if (litTable.find(pcReg) != litTable.end()) {
            opCode = tRecord.substr(i,(litTable.at(pcReg).length() -4));
            lstOut<< "\t\t\t LTORG \n";
            lstOut<< printAddress(pcReg) << "\t\t\t * \t" << litTable.at(pcReg) <<
            "\t" << opCode << "\n";
            i += (litTable.at(pcReg).length() -4);
            pcReg += (litTable.at(pcReg).length() -4) / 2;
            continue;
        }
        int opName = stoi((tRecord.substr(i,2)),nullptr,16)  & 0xFC;
        //Check if instruction is format 3/4 or format 2
        if(ops34.find(opName) != ops34.end()){
            //Check next 6 flag bits for extended format
            int nixbpe = stoi(tRecord.substr(i+1,2),nullptr,16) & 0x3F;
            if (nixbpe & 0x01){
                opCode = tRecord.substr(i,8);
                format34(ops34.at(opName), opCode, nixbpe);
                i += 8;
                pcReg += 4;
            }
            else{
                opCode = tRecord.substr(i,6);
                format34(ops34.at(opName), opCode, nixbpe);
                i += 6;
                pcReg += 3;
            }
        }
        else if(ops2.find(opName) != ops2.end()){
            opCode = tRecord.substr(i,4);
            format2(ops2.at(opName), opCode);
            i += 4;
            pcReg += 2;
        }
        //Break if opcode not found
        else{ break;}
        
    }
}
int main(int argc, char *argv[])
{
    string F1;
    string F2;
    int F1gjat;
    int F2gjat;

    // checks for .obj and .sym file
    if (argc != 3)
    {
        cout << "Error: need .obj and .sym files" << endl;
        exit(1);
    }
    F1 = argv[1];
    F2 = argv[2];
    // find . in string name for checking if obj or sym file
    F1gjat = F1.find_last_of(".");
    F2gjat = F2.find_last_of(".");
    // check for correct file types
    if (F2.substr(F2gjat + 1) != "sym")
    {
        cout << "Error: need  .obj and .sym files" << endl;
        exit(1);
    }
    if (F1.substr(F1gjat + 1) != "obj")
    {
        cout << "Error: need  .obj and .sym files" << endl;
        exit(1);
    }
    ifstream objFile(argv[1], ios::in);
    ifstream symFile(argv[2], ios::in);
    if (!objFile)
    {
        cout << "Error: need  .obj and .sym files" << endl;
        exit(1);
    }
    if (objFile.peek() == istream::traits_type::eof())
    {
        cout << "File " << F1 << " is empty" << endl;
        exit(1);
    }
    if (!symFile)
    {
        cout << "Error: need  .obj and .sym files" << endl;
        exit(1);
    }
    if (symFile.peek() == istream::traits_type::eof())
    {
        cout << "File " << F2 << " is empty" << endl;
        ;
        exit(1);
    }
    cout << F1 << " and " << F2 << " have successfuly loaded" << endl;
    Dissem *dissem = new Dissem;
    dissem->load_Data(argv[1], argv[2]);
    dissem->dissem();
    cout << ".lst created" << endl;
    printAddress(pcReg);
}
void Dissem::load_Data(char *obj_File, char *sym_File)
{
    obj_Stream.open(obj_File);
    sym_Stream.open(sym_File);
    string file_Line;
    string trim;
    string whitespace = " \n\r\t\f\v";
    // get obj file line by line and store into vector
    while (obj_Stream.good())
    {
        getline(obj_Stream, file_Line);
        object_Storage.push_back(file_Line);
    }
    // get sym file line by line and store into vector
    while (sym_Stream.good())
    {
        getline(sym_Stream, file_Line);
        //Remove whitespace from beginning of line
        if (file_Line.find_first_not_of(whitespace) != string::npos) {
            trim = file_Line.substr(file_Line.find_first_not_of(whitespace));
            symbol_Storage.push_back(trim);
        }
        else{
            symbol_Storage.push_back(file_Line);
        }
    }

    //loop through stored symbols and store each value and name in symTable map
    int i = 2;
    string name;
    int value;
    string tempValue = "";
    for (i = 2; i < symbol_Storage.size() - 1; i++)
    {
        istringstream s1(symbol_Storage[i]);
        // while symbols stored is not null push into vector
        if (symbol_Storage[i][0] != (char)NULL)
        {
            //Reads the line into name and value strings
            s1>> name >>skipws >> tempValue;
            value = stoi(tempValue, NULL, 16);
            symTable.insert({value, name});
            
        }
        // else move ahead 3 indexes since symbols are stored in vector as 3 strings for name,value, and format flag
        else
        {
            i += 3;
            break;
        }
        obj_Stream.close();
        sym_Stream.close();
    }

    // loop through stored symbols and check for literals and push into litTable map
    for (;i < symbol_Storage.size(); i++)
    {
        istringstream s2(symbol_Storage[i]);
        int length = 0;
        s2>> name >>skipws >> length >>skipws >> tempValue;
        if (tempValue != "") {
            value = stoi(tempValue,nullptr,16);
        }
        //Makes sure there are values to load
        if (length != 0) {
            litTable.insert({value, name});
        }
    }
}
    void Dissem::dissem()
{
    // loop through object storage by eows for each record type
    for (int i = 0; i < object_Storage.size(); i++)
    {
        string currLine(object_Storage[i]);
        switch (currLine[0])
        {
        case 'H':
            headRecord(currLine);
            break;
        case 'T':
            textRecord(currLine);
            break;
        case 'M':
            break;
        case 'E':
            endRecord(currLine);
            break;
        default:
            break;
        }
    }
}
