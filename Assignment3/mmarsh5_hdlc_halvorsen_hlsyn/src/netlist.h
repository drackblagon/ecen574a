#pragma once

#include <iostream>
#include "obj.h"
#include "operation.h"

namespace std {

class netlist {
public:
    vector<obj> inputs;
    vector<obj> outputs;
    vector<obj> registers;
    vector<obj> wires;
    vector<operation> operations;
    operation topLevelOp;
	int stateCnt;

	bool latencyGood(int latency) {
		int max = 0;
		for (auto& op : this->operations) {
			if (op.asapTime > max) {
				max = op.asapTime;
			}
		}

		if (max > latency) {
			return false;
		}
		else {
			return true;
		}
	}
    
    obj *getObj (string name) {
        if (name.empty()) {
            cout << "ERROR: name is empty" << endl;
            return nullptr;
        }
        for (auto& oTemp: inputs) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        for (auto& oTemp: outputs) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        for (auto& oTemp: registers) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        for (auto& oTemp: wires) {
            if (name.compare(oTemp.name) == 0) {
                return &oTemp;
            }
        }
        
        return nullptr;
    }
    
	void setStateCnt() {
		int cnt = 0;
		for (auto& op : this->operations) {
			if (op.scheduleTime > cnt) {
				cnt = op.scheduleTime;
			}
		}
		this->stateCnt = cnt + 2;
	}

    int addObject(obj::objTypeEnum objType, vector<string> tokens) {
        obj o;
        
        if (tokens.size() < 3) {
            cout << "ERROR: no more tokens" << endl;
            return FAILURE;
        }
        
        tokens.erase(tokens.begin());
        obj::variableTypeEnum vType = obj::getTypeFromString(tokens.at(0));
        if (vType == obj::UNKNOWN) {
            cout << "ERROR: Unknown variable type found: " << tokens.at(0) << endl;
            return FAILURE;
        }
        tokens.erase(tokens.begin());
        
        vector<obj> *vec;
        if (objType == obj::INPUT) {
            vec = &inputs;
        }
        else if (objType == obj::OUTPUT) {
            vec = &outputs;
        }
        else if (objType == obj::REGISTER) {
            vec = &registers;
        }
        else {
            vec = &wires;
        }
        
        for(auto& s: tokens) {
            size_t loc;
            if ((loc = s.find_first_of(",")) != string::npos) {
                s = s.substr(0, loc);
            }
            
            o.name = s;
            o.objType = objType;
            o.vType = vType;
            if (getObj(o.name) != nullptr) {
                cout << "ERROR: obj " << o.name << " is already defined" << endl;
                return FAILURE;
            }
            vec->push_back(o);
        }
        
        return SUCCESS;
    }
    
    int addOperation(vector<string> tokens, string op_line) {
        if (tokens.size() < 3) {
            cout << "ERROR: no more tokens" << endl;
            return FAILURE;
        }
        operation op;
        op.op_line = op_line;
        obj * o;
        if ((o = getObj(tokens.at(0))) == nullptr) {
            cout << "ERROR: could not find variable assigned to" << endl;
            return FAILURE;
        }
        op.output = o;
        
        if (tokens.at(1).compare("=") != 0) {
            cout << "ERROR: no assignment symbol (=) found" << endl;
            return FAILURE;
        }
        
        if ((o = getObj(tokens.at(2))) == nullptr) {
            cout << "ERROR: could not find first input variable" << endl;
            return FAILURE;
        }
        op.inputs.push_back(o);
        
        operation::operatorEnum oEnum;
        if (tokens.size() == 3) {
            oEnum = operation::REG;
        }
        else {
            oEnum = operation::getTypeFromString(tokens.at(3));
            if (oEnum == operation::UNKNOWN) {
                cout << "ERROR: unknown operation" << endl;
                return FAILURE;
            }
            
        }
        op.op = oEnum;
        
        
        switch (op.op) {
            case operation::REG:
                break;
            case operation::MUX2x1:
                if (tokens.size() != 7) {
                    cout << "ERROR: wrong number of arguments found" << endl;
                    return FAILURE;
                }
                
                if (tokens.at(5).compare(":") != 0) {
                    cout << "ERROR: no colon found in mux" << endl;
                    return FAILURE;
                }
                
                if ((o = getObj(tokens.at(4))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                op.inputs.push_back(o);
                
                if ((o = getObj(tokens.at(6))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                op.inputs.push_back(o);
                
                break;
                
            case operation::ADD:
                //Fall through
            case operation::SUB:
                if (tokens.size() != 5) {
                    cout << "ERROR: wrong number of arguments found" << endl;
                    return FAILURE;
                }
                if (tokens.at(4).compare("1") == 0) {
                    op.op = (op.op == operation::ADD)? operation::INC : operation::DEC;
                    break;
                }
                if ((o = getObj(tokens.at(4))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                op.inputs.push_back(o);
                break;
                
                
            case operation::MUL:
                //Fall through
            case operation::COMP_GT:
                //Fall through
            case operation::COMP_LT:
                //Fall through
            case operation::COMP_EQ:
                //Fall through
            case operation::SHR:
                //Fall through
            case operation::SHL:
                //Fall through
            case operation::DIV:
                //Fall through
            case operation::MOD:
                if (tokens.size() != 5) {
                    cout << "ERROR: wrong number of arguments found" << endl;
                    return FAILURE;
                }
                if ((o = getObj(tokens.at(4))) == nullptr) {
                    cout << "ERROR: could not find variable assigned to" << endl;
                    return FAILURE;
                }
                
                //TODO: still need to figure out a way to get if it is a signed operation
                op.inputs.push_back(o);
                break;
            default:
                return FAILURE;
        }
        operations.push_back(op);
        return SUCCESS;
    }
    
    int addIfOperation(vector<string> tokens, string op_line) {
        int result = FAILURE;
        
        if (tokens.size() != 5) {
            cout << "Wrong number of tokens for IF operation" << endl;
            return result;
        }
        if ((tokens.at(1) != "(") || (tokens.at(3) != ")")) {
            cout << "Could not find one of the parenthesis" << endl;
            return result;
        }
        if (tokens.at(4) != "{") {
            cout << "Could not find opening bracket" << endl;
            return result;
        }
        
        operation op;
        op.op_line = op_line;
        op.op = operation::IF;
        obj *o;
        
        if ((o = getObj(tokens.at(2))) == nullptr) {
            cout << "ERROR: could not find input variable" << endl;
            return FAILURE;
        }
        
        op.inputs.push_back(o);
        
        operations.push_back(op);
        
        result = SUCCESS;
        
        return result;
    }
    
    operation * getParent(obj *input) {
        for (auto& topInput: topLevelOp.inputs) {
            if (input == topInput) {
                return &topLevelOp;
            }
        }
        for (auto& op: operations) {
            if (input == op.output) {
                return &op;
            }
        }
        return nullptr;
    }
    
    int setupNodeTree () {
        for (auto& input: inputs) {
            topLevelOp.inputs.push_back(&input);
        }
        
        for (auto& op: operations) {
            if (&op == &topLevelOp) {
                continue;
            }
            for (auto &input: op.inputs) {
                operation *parent = getParent(input);
                if (parent != nullptr) {
                    bool alreadyAdded = false;
                    for (auto &p: op.parents) {
                        if (parent == p) {
                            alreadyAdded = true;
                        }
                    }
                    if (!alreadyAdded) {
                        op.parents.push_back(parent);
                    }
                    
                    alreadyAdded = false;
                    for (auto &c: parent->children) {
                        if (&op == c) {
                            alreadyAdded = true;
                        }
                    }
                    if (!alreadyAdded) {
                        parent->children.push_back(&op);
                    }
                }
            }
            if (op.parents.empty()) {
                cout << "This operation has no parents. Operation: " << op.op_line << endl;
                return FAILURE;
            }
        }
        return SUCCESS;
    }
    
    void printNode (operation *op, int depth) {
        int counter = 0;
        cout << "Depth " << depth << ".";
        for (int i = 0; i < depth; ++i) {
            cout << "\t";
        }
        cout << "op:" << op->op_line << endl;
        for (auto &child: op->children) {
            cout << "Depth " << depth << ".";
            for (int i = 0; i < depth; ++i) {
                cout << "\t";
            }
            cout << " child " << counter << endl;
            printNode(child, depth + 1);
            counter++;
        }
    }
    
    void printNodeTree () {
        printNode(&topLevelOp, 0);
    }

	void printStates(stringstream &output) {
		for (int i = 1; i <= this->stateCnt-2; i++) {
			output << "\t\t\t" << i << " : begin" << endl;
			for (auto& op : operations) {
				if (op.scheduleTime == i) {
					output << "\t\t\t\t" << op.op_line << ";" << endl;
				}
			}
			output << "\t\t\tend" << endl;
		}
		
	}
    
    int getVerilog(stringstream &output, string outFileName) {
        string moduleName = outFileName;
		setStateCnt();
        int pos;
		int finalState = this->stateCnt - 1;
        int lastSlashPos;
        if ((lastSlashPos = outFileName.find_last_of("/")) != string::npos) {
            if (lastSlashPos > 0) {
                moduleName = moduleName.substr(lastSlashPos + 1, moduleName.length() - 1);
            }
        }
        
        if ((pos = moduleName.find(".")) != string::npos) {
            if (pos > 0) {
                moduleName = moduleName.substr(0, pos);
            }
        }
        
        while (!isLetter(moduleName[0])) {
            moduleName = moduleName.substr(1, moduleName.length() - 1);
        }
        
        output << "module " << moduleName << " (Clk, Rst, Start, Done";
        
        for(auto& o: inputs) {
            output << ", ";
            output << o.name;
        }
        
        for(auto& o: outputs) {
            output << ", ";
            output << o.name;
        }
        
        output << ");" << endl;
        
        output << "\tinput Clk, Rst, Start;" << endl;
        output << "\toutput reg Done;" << endl;
        
        for(auto& o: inputs) {
            o.getInstanceString("input", output);
        }
        for(auto& o: outputs) {
            o.getInstanceString("output reg", output);
        }
        
        output << endl;
        
        for(auto& o: registers) {
            o.getInstanceString("reg", output);
        }
		output << "\treg [" << ceil(log2(this->stateCnt)) - 1 << ":0] State;" << endl;
        for(auto& o: wires) {
            o.getInstanceString("wire", output);
        }
        
        output << endl;

		output << "\talways @(posedge Clk) begin" << endl;
		output << "\t\tif (Rst) begin" << endl;

		for (auto& o : registers) {
			output << "\t\t\t" << o.name << " <= 0;" << endl;
		}
		for (auto& o : outputs) {
			output << "\t\t\t" << o.name << " <= 0;" << endl;
		}
		output << "\t\t\tState <= 0;" << endl;
		output << "\t\tend" << endl;
		output << "\t\telse begin" << endl;
		output << "\t\tcase (State)" << endl;
		output << "\t\t\t0 : begin" << endl;
	    output << "\t\t\t\tif (Start) State <= 1;" << endl;
		output << "\t\t\t\telse State <= 0;" << endl;
		output << "\t\t\tend" << endl;

		printStates(output);

		output << "\t\t\t" << finalState << " : begin" << endl;
		output << "\t\t\t\tDone <= 1;" << endl;
		output << "\t\t\t\tState <= 0;" << endl;
		output << "\t\t\tend" << endl;

		output << "\t\tendcase" << endl;
		output << "\t\tend" << endl;
		output << "\tend" << endl;
        
        /*for(auto& op: operations) {
            op.getOutputVerilog(output);
        }*/
        
        output << "endmodule" << endl;
        
        
        return SUCCESS;
    }

    void print() {
        cout << "netlist:" << endl;
        cout << "\tinputs:" << endl;
        for(auto& o: inputs) {
            o.print();
        }
        cout << "\toutputs:" << endl;
        for(auto& o: outputs) {
            o.print();
        }
        cout << "\tregisters:" << endl;
        for(auto& o: registers) {
            o.print();
        }
        cout << "\twires:" << endl;
        for(auto& o: wires) {
            o.print();
        }
        cout << "\toperations:" << endl;
        for(auto& ops: operations) {
            ops.print();
        }
    }
    
private:
    
    bool isLetter(char c) {
        if (c >= 'a' && c <= 'z') {
            return true;
        }
        else if (c >= 'A' && c <= 'Z') {
            return true;
        }
        else {
            return false;
        }
    }
};

}
