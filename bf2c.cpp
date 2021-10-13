#include <iostream>
#include <fstream>
#include <list>
#include <locale>
//#include <stdexcept>
//#include <cstdio>
//#include <cstdlib> 
#include <ctime> 
#include <sstream>
#include <string>
#include <vector>

#define VERSION "0.1.0"

using namespace std;

enum Operation {
    MEM_ADD,
    MEM_DEC,
    PTR_ADD,
    PTR_DEC,
    PRINT_CHAR,
    LOOP_START,
    LOOP_END,
};

list<Operation> operations = {};
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
"        printf(\"Error allocating memory!\");\n"
"        return 1;\n"
"    }\n"
"    int mem_ptr = 0;\n"
"    /* GENERATED CODE START */\n"
"#BRAINFUCK#"
"    /* GENERATED CODE END  */\n"
"    free(memory);\n"
"    return 0;\n"
"}\n";

string out_bin_file = "./a.out";
string out_source_file;

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
string get_output_file() {
    if (!out_source_file.empty()) {
        return out_source_file;
    }
    ostringstream file;
    srand((unsigned)time(0)); 
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
    string tmp_file_path = get_output_file();
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
    command.append(out_bin_file);
    command.append(" ");
    cout << "running '" << command << "'\n";
    command.append("2>&1");
    FILE *pPipe = popen(command.c_str(), "r");
    if (pPipe == nullptr) {
        cerr << "Error: No c compiler (cc) found! Be sure to have a c compiler installed and in your path!";
        return "";
    }

    array<char, 1024> buffer;
    string result;

    while (!feof(pPipe)) {
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
    for(int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg.rfind("--bin-output", 0) != string::npos) {
            string option = arg.substr(12);
            if (!option.empty()) {
                cout << option << "\n";
                out_bin_file = option;
            } else if (i < (argc - 1)) {
                out_bin_file = argv[i+1];
                cout << out_bin_file << "\n";             
            } else {
                cerr << "Invalid argument! '--bin-output' needs a file path!\n";
                return 1;
            }
            continue;
        } else if (arg.rfind("-bo", 0) != string::npos) {
            string option = arg.substr(3);
            if (!option.empty()) {
                cout << option << "\n";
                out_bin_file = option;
            } else if (i < (argc - 1)) {
                out_bin_file = argv[i+1];
                cout << out_bin_file << "\n";              
            } else {
                cerr << "Invalid argument! '-bo' needs a file path!\n";
                return 1;
            }
            continue;
        } else if (arg.rfind("--output", 0) != string::npos) {
            string option = arg.substr(8);
            if (!option.empty()) {
                cout << option << "\n";
                out_bin_file = option;
            } else if (i < (argc - 1)) {
                out_bin_file = argv[i+1];
                cout << out_bin_file << "\n";             
            } else {
                cerr << "Invalid argument! '--output' needs a file path!\n";
                return 1;
            }
            continue;
        } else if (arg.rfind("-o", 0) != string::npos) {
            string option = arg.substr(2);
            if (!option.empty()) {
                cout << option << "\n";
                out_source_file = option;
            } else if (i < (argc - 1)) {
                out_source_file = argv[i+1];
                cout << out_source_file << "\n";           
            } else {
                cerr << "Invalid argument! '-o' needs a file path!\n";
                return 1;
            }
            continue;
        } else if (arg.rfind("--help", 0) != string::npos) {
            cout << "bf2c <file> [options]\n";
            cout << "Options:\n";
            cout << "-o, --output <file>\t\twrites the transpiled C code to <file>\n";
            #ifdef __unix__
            cout << "-bo, --bin-output <file>\tcompiles the transpiled C code using the system's c compiler (cc) to <file>\n";
            #endif
            cout << "--help <file>\t\t\tshows this text\n";
            cout << "--version <file>\t\tshows bf2c version info\n";
            cout << "--license <file>\t\tshows license info\n";
            return 0;
        } else if (arg.rfind("--version", 0) != string::npos) {
            cout << "brainfuck to c - bf2c v" << VERSION << "\n";
            cout << "Copyright (C) 2021 ico277\n";
            cout << "This software is licensed under the MIT license.\n";
            cout << "For more info see 'bf2c --license'\n";
            return 0;
        } else if (arg.rfind("--license", 0) != string::npos) {
            cout << 
            "This project is licensed under the MIT License\n"
            "\n"
            "Copyright (c) 2021 ico277\n"
            "\n"
            "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
            "of this software and associated documentation files (the \"Software\"), to deal\n"
            "in the Software without restriction, including without limitation the rights\n"
            "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
            "copies of the Software, and to permit persons to whom the Software is\n"
            "furnished to do so, subject to the following conditions:\n"
            "\n"
            "The above copyright notice and this permission notice shall be included in all\n"
            "copies or substantial portions of the Software.\n"
            "\n"
            "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
            "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
            "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
            "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
            "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
            "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
            "SOFTWARE.\n";
            return 0;
        }
    }

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

    cout << "Compiling " << argv[1] << " ...\n";

    for(char& c : brainfuck) {
        switch (c) {
            case '+':
                //operations.push_back("++memory[mem_ptr];");
                operations.push_back(Operation::MEM_ADD);
                break;
            case '-':
                //operations.push_back("--memory[mem_ptr];");
                operations.push_back(Operation::MEM_DEC);
                break;
            case '>':
                //operations.push_back("++mem_ptr;");
                operations.push_back(Operation::PTR_ADD);
                break;
            case '<':
                //operations.push_back("--mem_ptr;");
                operations.push_back(Operation::PTR_DEC);
                break;
            case '[':
                //operations.push_back("while (memory[mem_ptr] != 0) {");
                operations.push_back(Operation::LOOP_START);
                break;
            case ']':
                //operations.push_back("}");
                operations.push_back(Operation::LOOP_END);
                break;
            case '.':
                //operations.push_back("putchar(memory[mem_ptr]);");
                operations.push_back(Operation::PRINT_CHAR);
                break;
            default:
                cerr << "Syntax error: Invalid char '" << c << "'!\n";
                //return 1;
                break;
        }
    }
    string generated_code;
    //for (string& str : operations) {
    //    generated_code += "    " + str + "\n";
    //}
    int loops = 1;
    for (Operation& op : operations) {
        for (int i = 1; i < loops; i++) {
            generated_code += "    ";
        }
        switch (op) {
            case Operation::MEM_ADD:
                generated_code += "    ++memory[mem_ptr];\n";
                break;
            case Operation::MEM_DEC:
                generated_code += "    --memory[mem_ptr];\n";
                break;
            case Operation::PTR_ADD:
                generated_code += "    ++mem_ptr;\n";
                break;
            case Operation::PTR_DEC:
                generated_code += "    --mem_ptr;\n";
                break;
            case Operation::PRINT_CHAR:
                generated_code += "    putchar(memory[mem_ptr]);\n";
                break;
            case Operation::LOOP_START:
                generated_code += "    while (memory[mem_ptr] != 0) {\n";
                ++loops;
                break;
            case Operation::LOOP_END:
                generated_code += "}\n";
                --loops;
                break;
        }
    }
    generated_code = replace(template_file, "#BRAINFUCK#", generated_code);
    #ifdef __unix__
    int code;
    cout << compile_c(generated_code, code) << "\n";
    if (code != 0) {
        cerr << "There was an error compiling!\n";
        return 1;
    }
    #else
    cout << generated_code << "\n";
    #endif
}
