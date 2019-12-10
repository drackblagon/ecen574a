#pragma once

#include <iostream>

namespace std {

class obj {
public:
    enum variableTypeEnum {
        Int1, Int2, Int8, Int16, Int32, Int64,
        UInt1, UInt2, UInt8, UInt16, UInt32, UInt64,
        UNKNOWN
    };

    enum objTypeEnum {
        INPUT,
        OUTPUT,
        REGISTER,
        WIRE,
    };

    variableTypeEnum vType;
    objTypeEnum objType;
    string name;

    static variableTypeEnum getTypeFromString(string str) {
        variableTypeEnum t = UNKNOWN;

        if (str.compare("Int1") == 0) {
            return Int1;
        }
        else if (str.compare("Int2") == 0) {
            t = Int2;
        }
        else if (str.compare("Int8") == 0) {
            t = Int8;
        }
        else if (str.compare("Int16") == 0) {
            t = Int16;
        }
        else if (str.compare("Int32") == 0) {
            t = Int32;
        }
        else if (str.compare("Int64") == 0) {
            t = Int64;
        }
        else if (str.compare("UInt1") == 0) {
            t = UInt1;
        }
        else if (str.compare("UInt2") == 0) {
            t = UInt2;
        }
        else if (str.compare("UInt8") == 0) {
            t = UInt8;
        }
        else if (str.compare("UInt16") == 0) {
            t = UInt16;
        }
        else if (str.compare("UInt32") == 0) {
            t = UInt32;
        }
        else if (str.compare("UInt64") == 0) {
            t = UInt64;
        }
        return t;
    }
    
    static int getSize(variableTypeEnum t) {
        int size = 0;
        
        switch (t) {
            case Int1:
            case UInt1:
                size = 1;
                break;
            case Int2:
            case UInt2:
                size = 2;
                break;
            case Int8:
            case UInt8:
                size = 8;
                break;
            case Int16:
            case UInt16:
                size = 16;
                break;
            case Int32:
            case UInt32:
                size = 32;
                break;
            case Int64:
            case UInt64:
                size = 64;
                break;
            case UNKNOWN:
                break;
        }
        return size;
    }
    
    int getSize() {
        return getSize(vType);
    }
    
    static bool isSigned(variableTypeEnum t) {
        switch (t) {
            case Int1:
            case Int2:
            case Int8:
            case Int16:
            case Int32:
            case Int64:
                return true;
            case UInt1:
            case UInt2:
            case UInt8:
            case UInt16:
            case UInt32:
            case UInt64:
                return false;
            case UNKNOWN:
                return false;
        }
		return false;
    }
    
    bool isSigned() {
        return isSigned(vType);
    }
    
    void getConcatString(int outputSize, stringstream &strStream) {
        if (outputSize > getSize()) {
            int numBitsExtended = outputSize - getSize();
            if (isSigned()) {
                strStream << "{ {" << numBitsExtended << "{" << name << "[" << getSize() - 1 << "]}}, " << name << "[" << getSize() - 1 << ":0] }";
            }
            else {
                strStream << "{" << numBitsExtended << "'b0, " << name << "}";
            }
            string signedStr = (isSigned()) ? "1" : "0";
        }
        else if (outputSize < getSize()) {
            if (outputSize == 1) {
                strStream << name << "[0]";
            }
            else {
                strStream << name << "[" << outputSize - 1 << ":0]";
            }
        }
        else {
            strStream << name;
        }
    }
    
    void getInstanceString(const string typeName, stringstream &strStream) {
        int size = getSize(vType);
        string signedStr = (isSigned())? " signed " : "";
        if (size == 1) {
            strStream << "\t" << typeName << signedStr << " " << name << ";" << endl;
        }
        else {
            strStream << "\t" << typeName << signedStr << " [" << size - 1 << ":0] " << name << ";" << endl;
        }
    }

    void print() {
        cout << "\t\tname: " << name << endl;
        cout << "\t\t\tvType: " << stringify(vType) << endl;
        cout << "\t\t\tioType: " << stringify(objType) << endl;
    }

private:
    string stringify (variableTypeEnum vType) {
        switch (vType) {
            case Int1:
                return "Int1";
            case Int2:
                return "Int2";
            case Int8:
                return "Int8";
            case Int16:
                return "Int16";
            case Int32:
                return "Int32";
            case Int64:
                return "Int64";

            case UInt1:
                return "UInt1";
            case UInt2:
                return "UInt2";
            case UInt8:
                return "UInt8";
            case UInt16:
                return "UInt16";
            case UInt32:
                return "UInt32";
            case UInt64:
                return "UInt64";

            default:
                return "UNKNOWN";
        }
    }
    
    string stringify (objTypeEnum oType) {
        switch (oType) {
            case INPUT:
                return "INPUT";
            case OUTPUT:
                return "OUTPUT";
            case REGISTER:
                return "REGISTER";
            case WIRE:
                return "WIRE";

            default:
                return "UNKNOWN";
        }
    }
};

}
