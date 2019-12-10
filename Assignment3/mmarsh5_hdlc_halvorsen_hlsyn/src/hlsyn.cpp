#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>

#include "convert.h"

enum {
    INPUT_FILE_INDEX=1,
    MAX_CYCLES_INDEX,
    OUTPUT_FILE_INDEX,
    EXPECTED_NUM_ARGS
};

using namespace std;

int main(int argc, char *argv[]) {
    int result = 1;
    int maxCycles = 0;
    string line;
    ifstream inFile;
    ofstream outFile;
    string outFileName = "HLSM";
    
    stringstream in, out;
    
    if (argc != EXPECTED_NUM_ARGS)
    {
        cout << "ERROR: Wrong number of arguments! Expecting " << EXPECTED_NUM_ARGS << " but " << argc << " were entered" <<endl;
        goto FailedToParse;
    }
    
    inFile.open(argv[INPUT_FILE_INDEX]);
    if (!inFile.is_open()) {
        cout << "ERROR: Unable to open input file!" << endl;
        goto FailedToParse;
    }
    
    try {
        maxCycles = stoi(argv[MAX_CYCLES_INDEX]);
        if (maxCycles < 1) {
            cout << "ERROR: Unable to convert input cycles to a positive integer!" << endl;
            goto FailedToParse;
        }
    } catch (invalid_argument) {
        cout << "ERROR: Unable to convert input cycles to a positive integer!" << endl;
        goto FailedToParse;
    } catch (out_of_range) {
        cout << "ERROR: Unable to convert input cycles to a positive integer!" << endl;
        goto FailedToParse;
    }
    
    outFile.open(argv[OUTPUT_FILE_INDEX]);
    if (!outFile.is_open()) {
        cout << "ERROR: Unable to open output file!" << endl;
        goto FailedToParse;
    }
    
    in << inFile.rdbuf();
    if (convertToVerilog(in, out, outFileName, maxCycles) != SUCCESS) {
        cout << "ERROR: Failed to parse input file!" << endl;
        goto FailedToConvert;
    }
        
    outFile << out.str();
    
    result = 0;
    goto END;

FailedToParse:
    cout << endl << "Failed to parse input arguments" << endl << endl;
    cout << "Example usage:" << endl << "\thlsyn input.c 6 output.v" << endl << endl;
    goto END;
    
FailedToConvert:
    cout << "Failed to convert to verilog" << endl << endl;
    goto END;
    
END:
    if (inFile.is_open()) {
        inFile.close();
    }
    if (outFile.is_open()) {
        outFile.close();
    }
    return result;
}
