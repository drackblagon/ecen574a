#pragma once

#include <iostream>

namespace std {

class operation {
public:
    enum operatorEnum {
        REG,
        ADD,
        SUB,
        MUL,
        COMP_GT,
        COMP_LT,
        COMP_EQ,
        MUX2x1,
        SHR,
        SHL,
        DIV,
        MOD,
        INC,
        DEC,
        UNKNOWN
    };
    
	int asapTime;
	int alapTime;
	int width;
	double prob;
	int scheduleTime;
	bool visited;

    vector<obj *> inputs;
    obj *output;
    operatorEnum op;
    bool hasSigned;
    string op_line;
    
    vector<operation *> parents;
    vector<operation *> children;
    
    static operatorEnum getTypeFromString(string str) {
        operatorEnum t = UNKNOWN;
        
        if (str.compare("+") == 0) {
            return ADD;
        }
        else if (str.compare("-") == 0) {
            t = SUB;
        }
        else if (str.compare("*") == 0) {
            t = MUL;
        }
        else if (str.compare(">") == 0) {
            t = COMP_GT;
        }
        else if (str.compare("<") == 0) {
            t = COMP_LT;
        }
        else if (str.compare("==") == 0) {
            t = COMP_EQ;
        }
        else if (str.compare("?") == 0) {
            t = MUX2x1;
        }
        else if (str.compare(">>") == 0) {
            t = SHR;
        }
        else if (str.compare("<<") == 0) {
            t = SHL;
        }
        else if (str.compare("/") == 0) {
            t = DIV;
        }
        else if (str.compare("%") == 0) {
            t = MOD;
        }
        return t;
    }
    
    void getOutputVerilog(stringstream &strStream) {
        static int runningCount = 0;
        int sizeBiggestIn = 0;
        string signedStr = (hasSignedVar())? "S" : "";
        int dataWidth = output->getSize();
        if(stringify(op) == "COMP") {
            dataWidth = (inputs.at(0)->getSize() >= inputs.at(1)->getSize())? inputs.at(0)->getSize(): inputs.at(1)->getSize();
        }
        strStream << "\t" << signedStr << stringify(op) << " #(.DATAWIDTH(" << dataWidth << ")) " << stringify(op) << "_" << runningCount++ << "(";
        switch (op) {
            case REG:
                inputs.at(0)->getConcatString(output->getSize(), strStream);
                strStream << ", " << output->name << ", Clk, Rst";
                break;
            case COMP_GT:
                sizeBiggestIn = (inputs.at(0)->getSize() >= inputs.at(1)->getSize())? inputs.at(0)->getSize(): inputs.at(1)->getSize();
                for (auto& o: inputs) {
                    o->getConcatString(sizeBiggestIn, strStream);
                    strStream << ", ";
                }
                strStream << output->name << ", , ";
                break;
            case COMP_LT:
                sizeBiggestIn = (inputs.at(0)->getSize() >= inputs.at(1)->getSize())? inputs.at(0)->getSize(): inputs.at(1)->getSize();
                for (auto& o: inputs) {
                    o->getConcatString(sizeBiggestIn, strStream);
                    strStream << ", ";
                }
                strStream << ", " << output->name << ", ";
                break;
            case COMP_EQ:
                sizeBiggestIn = (inputs.at(0)->getSize() >= inputs.at(1)->getSize())? inputs.at(0)->getSize(): inputs.at(1)->getSize();
                for (auto& o: inputs) {
                    o->getConcatString(sizeBiggestIn, strStream);
                    strStream << ", ";
                }
                strStream << ", , " << output->name;
                break;
            case MUX2x1:
                inputs.at(2)->getConcatString(output->getSize(), strStream);
                strStream << ", ";
                inputs.at(1)->getConcatString(output->getSize(), strStream);
                strStream << ", ";
                inputs.at(0)->getConcatString(1, strStream);
                strStream << ", " << output->name;
                break;
            case ADD:
            case SUB:
            case MUL:
            case SHR:
            case SHL:
            case DIV:
            case MOD:
            case INC:
            case DEC:
                for (auto& o: inputs) {
                    o->getConcatString(output->getSize(), strStream);
                    strStream << ", ";
                }
                strStream << output->name;
                break;
            case UNKNOWN:
                break;
        }
        strStream << ");" << endl;
    }
    
    void print() {
        cout << "\t\top_line: " << op_line << endl;
        cout << "\t\t\toutput name: " << output->name << endl;
        for(auto& o: inputs) {
            cout << "\t\t\tinput name: " << o->name << endl;
        }
        cout << "\t\t\top: " << stringify(op) << endl;
        cout << "\t\t\thasSigned: " << stringify(hasSigned) << endl;
    }

	string getType() {
		return stringify(this->op);
	}

	operatorEnum getEnum() {
		switch (this->op) {
			case REG:
			case ADD:
			case SUB:
			case MUL:
				return this->op;
			case COMP_GT:
			case COMP_LT:
			case COMP_EQ:
				return COMP_GT;
			case MUX2x1:
				return COMP_LT;
			case SHR:
				return COMP_EQ;
			case SHL:
				return MUX2x1;
			case DIV:
				return SHR;
			case MOD:
				return SHL;
			case INC:
				return DIV;
			case DEC:
				return MOD;
			default:
				return UNKNOWN;
		}
	}

	/*bool compareOp(operation op2) {
		bool result;
		for (auto& ops_1 : this->inputs) {
			for (auto& ops_2 : op2.inputs) {
				if (ops_1->name.compare(ops_2->name) == 0) {
					result = true;
					break;
				}
				else {
					result = false;
				}
			}
		}
		return result;
	}*/
    
private:
    int getMaxVarSize() {
        int max = 0;
        for (auto &o: inputs) {
            if (o->getSize() > max) {
                max = o->getSize();
            }
        }
        if (output->getSize() > max) {
            max = output->getSize();
        }
        return max;
    }
    
    bool hasSignedVar () {
        for(auto& o: inputs) {
            if(o->isSigned()) {
                return true;
            }
        }
        return output->isSigned();
    }
    string stringify (operatorEnum oEnum) {
        switch (oEnum) {
            case REG:
                return "REG";
            case ADD:
                return "ADD";
            case SUB:
                return "SUB";
            case MUL:
                return "MUL";
            case COMP_GT:
            case COMP_LT:
            case COMP_EQ:
                return "COMP";
            case MUX2x1:
                return "MUX2x1";
            case SHR:
                return "SHR";
            case SHL:
                return "SHL";
            case DIV:
                return "DIV";
            case MOD:
                return "MOD";
            case INC:
                return "INC";
            case DEC:
                return "DEC";
                
            default:
                return "UNKNOWN";
        }
    }
    string stringify (bool b) {
        if (b) {
            return "TRUE";
        }
        else {
            return "FALSE";
        }
    }
};

}
