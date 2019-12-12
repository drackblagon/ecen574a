//
//  convert.cpp
//  hlsyn
//
//  Created by Zach Halvorsen on 10/30/19.
//

#include <iostream>
#include <vector>
#include <string>
#include <iterator>

#include "convert.h"
#include "obj.h"
#include "operation.h"
#include "netlist.h"
#include "critical.h"
#include "fds.h"


using namespace std;

typedef enum {
    PARSE_SUCCESS = SUCCESS,
    PARSE_ENDIF,
    PARSE_FAILURE
} parseOutput;

int parseFileSection(stringstream &in, netlist& nl, operation *ifOp = nullptr);
int getTokensFromLine(string line, vector<string> &outTokens);
parseOutput parseLine(vector<string> tokens, netlist &nl, stringstream &in, string line, operation *ifOp);


int convertToVerilog(stringstream &in, stringstream &out, string outFileName, int maxCycles) {
    int result = FAILURE;
    netlist nl;
	double pathDelay = 0.0;

    if (parseFileSection(in, nl) != SUCCESS) {
        return result;
    }

//    nl.print();
    if (nl.setupNodeTree() != SUCCESS) {
        cout << "ERROR: setting up relationship tree" << endl;
        return result;
    }
    
    nl.printNodeTree();

	calcAsap(&nl.operations);

	if (!nl.latencyGood(maxCycles)) {
		cout << "ERROR: Latency Constraint too small" << endl;
		return result;
	}

	calcAlap(&nl.operations, maxCycles);
	computeTimeFrame(&nl.operations);
	calcTypeDist(&nl.operations, maxCycles);
	calcForce(&nl.operations, maxCycles);
	printScheduledTime(&nl.operations);

    if (nl.getVerilog(out, outFileName) != SUCCESS) {
        cout << "ERROR: converting to verilog" << endl;
        return result;
    }

    result = SUCCESS;

    return result;
}

int parseFileSection(stringstream &in, netlist& nl, operation * ifOp)
{
    int result = FAILURE;
    string line;
    
    while(getline(in, line)) {
			
        vector<string> tokens;
        if (getTokensFromLine(line, tokens) != SUCCESS) {
            cout << "ERROR: Failed to clean up line" << endl;
            return result;
        }
        if (tokens.empty()) {
            continue;
        }
        parseOutput parseOut = PARSE_FAILURE;
        if ((parseOut = parseLine(tokens, nl, in, line, ifOp)) == PARSE_FAILURE) {
            cout << "ERROR: Failed to parse line:" << endl << "\t" << line << endl;
            return result;
        }
        if (parseOut == PARSE_ENDIF) {
            if (ifOp == nullptr) {
                cout << "ERROR: Reached the end of an if statement without starting one" << endl;
                return result;
            }
            return SUCCESS;
        }
    }
    
    result = SUCCESS;
    return result;
}

int getTokensFromLine(string line, vector<string> &outTokens)
{
    outTokens.clear();
    if (line.find_first_not_of(" \t\n\v\f\r") == string::npos) {
        //This line is only whitespace
        return SUCCESS;
    }
    if ((line.length() >= 2) && (line[0] == '/') && (line[1] == '/')) {
        //This line is a comment
        return SUCCESS;
    }
    
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    
    vector<string> tokens(beg, end);
    
    // Ignore comments
    for(int i = 0; i < tokens.size(); ++i) {
        int pos;
        if ((pos = tokens.at(i).find("//")) != string::npos) {
            vector<string>::iterator nth = tokens.begin() + i;
            if (pos > 0) {
                tokens.at(i) = tokens.at(i).substr(0, pos);
                if (nth != tokens.end()) {
                    nth++;
                }
            }
            tokens.erase(nth, tokens.end());
            break;
        }
    }
    
    for (auto& t: tokens) {
        outTokens.push_back(t);
    }
    return SUCCESS;
}

parseOutput parseLine(vector<string> tokens, netlist &nl, stringstream &in, string line, operation *ifOp)
{
	cout << "Line: " << line << endl;

    parseOutput result = PARSE_FAILURE;
    if (tokens.at(0).compare("input") == 0) {
        if (nl.addObject(obj::INPUT, tokens) != SUCCESS) {
            cout << "ERROR: adding obj to inputs" << endl;
            return result;
        }
    }
    else if (tokens.at(0).compare("output") == 0) {
        if (nl.addObject(obj::OUTPUT, tokens) != SUCCESS) {
            cout << "ERROR: adding obj to outputs" << endl;
            return result;
        }
    }
    else if (tokens.at(0).compare("variable") == 0) {
        if (nl.addObject(obj::REGISTER, tokens) != SUCCESS) {
            cout << "ERROR: adding obj to registers" << endl;
            return result;
        }
    }
    else if (tokens.at(0).compare("if") == 0) {
		if (nl.addIfOperation(tokens, line) != SUCCESS) {
            return result;
        }
		
       if (parseFileSection(in, nl, &nl.operations.back()) != SUCCESS) {
            return result;
       }
    }
    else if (tokens.at(0).compare("}") == 0) {
        //TODO: Check for else
        streampos oldPos = in.tellg();
        while(getline(in, line)) {
            vector<string> tokens;
            if (getTokensFromLine(line, tokens) != SUCCESS) {
                cout << "ERROR: Failed to clean up line" << endl;
                return result;
            }
            if (tokens.empty()) {
                oldPos = in.tellg();
                continue;
            }
            
            if (tokens.at(0).compare("else") == 0) {
                if (tokens.size() != 2) {
                    cout << "Could not find opening bracket for else" << endl;
                    return result;
                }
                if (tokens.at(1) != "{") {
                    cout << "Could not find opening bracket for else" << endl;
                    return result;
                }
                return PARSE_SUCCESS;
            }
        }
        in.seekg(oldPos);
        return PARSE_ENDIF;
    }
    else if (tokens.at(0).compare("else") == 0) {
        cout << "ERROR: Found else without if" << endl;
        return result;
    }
    
    else {
        if (nl.addOperation(tokens, line) != SUCCESS) {
            cout << "ERROR: adding operation" << endl;
            return result;
        }

        if (ifOp != nullptr) {
            cout << "Adding parent/child relationship for " << line << endl;
            operation * lastOp = &nl.operations.back();
            lastOp->parents.push_back(ifOp);
            // TODO: This is failing. Need to fix this
			ifOp->children.push_back(lastOp);
        }
    }
    
    result = PARSE_SUCCESS;
    return result;
}
