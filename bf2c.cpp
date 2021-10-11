#include <iostream>
#include <fstream>
#include <list>
#include <locale>
#include <stdexcept>
#include <cstdio>
#include <cstdlib> 
#include <ctime> 
#include <sstream>

using namespace std;

list<string> operations = {};
string template_file = 
"/* This File was generated using BF2C */\n"
"\n"
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"\n"
"#define MEMORY_SIZE 30000 // 30KB\n"
"\n"
"int main() {\n"
"    int* memory = malloc(sizeof(int) * MEMORY_SIZE);\n"
"    if (!memory) {\n"
"        printf(\"Error: Couldn't allocate memory!\");\n"
"        return 1;\n"
"    }\n"
"    int mem_ptr = 0;\n"
"\n"
"#BRAINFUCK#\n"
"\n"
"    return 0;\n"
"}\n";

string replace(std::string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}


string clean_brainfuck(string bf) {
    bf = replace(bf, " ", "");
    bf = replace(bf, "\n", "");
    return bf;
}
#ifdef __unix__

string get_tmp_file() {
    srand((unsigned)time(0)); 
    ostringstream file;
    
    #ifdef __unix__
    file << "/tmp/";
    #endif

    #ifdef _WIN32
    file << ".\\";
    #endif

    for(int i = 0; i < 10; i++) {
        file << (rand() % 9);
    }
    file << ".c";
    return file.str();
}

string compile_c(string code, int& exit_code) {
    exit_code = 0;
    ofstream tmp_file;
    string tmp_file_path = get_tmp_file();
    tmp_file.open(tmp_file_path);
    if (!tmp_file.is_open()) {
        cerr << "Error: Couldn't write transpiled brainfuck to temporary file! (Creating temporary file '" << tmp_file_path << "' failed)";
        return "";
    }
    if (!tmp_file.write(code.c_str(), code.length())) {
        cerr << "Error: Couldn't write transpiled brainfuck to temporary file! (Error writing to temporary file)";
        return "";
    }
    string command = "cc ";
    command.append(tmp_file_path);
    command.append(" -o ");
    command.append("./a.out ");
    cout << "running '" << command << "'\n";
    command.append("2>&1");
    FILE *pPipe = popen(command.c_str(), "r");
    if (pPipe == nullptr)
    {
        cerr << "Error: No c compiler (cc) found! Be sure to have a c compiler installed and in your path!";
        return "";
    }

    array<char, 1024> buffer;
    string result;

    while (!feof(pPipe))
    {
        auto bytes = fread(buffer.data(), 1, buffer.size(), pPipe);
        result.append(buffer.data(), bytes);
    }

    string sh_not_found = "sh: 1:";
    if (result.rfind(sh_not_found) != string::npos) {
        cerr << "Error: No c compiler (cc) found! Be sure to have a c compiler installed and in your path!";
        return "";
    }

    exit_code = pclose(pPipe);
    return result;
}
#endif

int main(int argc, char** argv) {
    string brainfuck; //= "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";
    if (argc > 1) {
        ifstream file;
        file.open(argv[1]);
        if (file.is_open()) {
            string line;
            while (std::getline(file, line)) {
                brainfuck += line + "\n";
            }
            file.close();
        } else {
            cerr << "Error: Error reading file!\n";
            return 1;
        }
    } else {
        cerr << "Error: You need to specify a file to transpile!\n";
        return 1;
    }

    brainfuck = clean_brainfuck(brainfuck);

    for(char& c : brainfuck) {
        switch (c) {
            case '+':
                operations.push_back("++memory[mem_ptr];");
                break;
            case '-':
                operations.push_back("--memory[mem_ptr];");
                break;
            case '>':
                operations.push_back("++mem_ptr;");
                break;
            case '<':
                operations.push_back("--mem_ptr;");
                break;
            case '[':
                operations.push_back("\nwhile (memory[mem_ptr] != 0) {");
                break;
            case ']':
                operations.push_back("\n}");
                break;
            case '.':
                operations.push_back("putchar(memory[mem_ptr]);");
                break;
            default:
                cerr << "Syntax error: Invalid char '" << c << "'!\n";
                return 1;
                break;
        }
    }
    string generated_code;
    for (string& str : operations) {
        generated_code += "    " + str + "\n";
    }
    generated_code = replace(template_file, "#BRAINFUCK#", generated_code);
    #ifdef __unix__
    int code;
    cout << compile_c(generated_code, code) << "\n";
    if (code != 0) {
        cerr << "There was an error compiling!\n";
        return 1;
    }
    #endif
    //cout << generated_code << "\n";
}