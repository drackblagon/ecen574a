#pragma once
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <iterator>
#include <stdbool.h>
#include <math.h>
#include "netlist.h"
#include "operation.h"
#include "obj.h"

double finalCritPath = 0.0;
int itCnt = 0;

namespace std {

vector<vector<double>> lut{ { 2.616, 2.644, 2.879, 3.061, 3.602, 3.966  },
                            { 2.704, 3.713, 4.924, 5.638, 7.270, 9.566  },
                            { 3.024, 3.412, 4.890, 5.569, 7.253, 9.566  },
                            { 2.438, 3.651, 7.453, 7.811,12.395, 15.354 },
                            { 3.031, 3.934, 5.949, 6.256, 7.264, 8.416  },
                            { 4.083, 4.115, 4.815, 5.623, 8.079, 8.766  },
                            { 3.644, 4.007, 5.178, 6.460, 8.819,11.095  },
                            { 3.614, 3.980, 5.152, 6.549, 8.565,11.220  },
                            { 0.619, 2.144,15.439,33.093,86.312,243.233 },
                            { 0.758, 2.149,16.078,35.563,88.142,250.583 },
                            { 1.792, 2.218, 3.111, 3.471, 4.347, 6.200  },
                            { 1.792, 2.218, 3.108, 3.701, 4.685, 6.503  } };

enum width {one, two, eight, sixteen, thirtytwo, sixtyfour};
 
double getCritPath() {
	return finalCritPath;
}

width getDataWidth(operation op) {
	int numWidth = 0;
	width dataWidth;

	if (op.getType().compare("COMP") == 0) {
		for (auto& input : op.inputs) {
			if (input->getSize() > numWidth) {
				numWidth = input->getSize();
			}
		}
	}
	else {
		numWidth = op.output->getSize();
	}

	if (numWidth == 1) {
		dataWidth = one;
	}
	else if (numWidth == 2) {
		dataWidth = two;
	}
	else if (numWidth == 8) {
		dataWidth = eight;
	}
	else if (numWidth == 16) {
		dataWidth = sixteen;
	}
	else if (numWidth == 32) {
		dataWidth = thirtytwo;
	}
	else {
		dataWidth = sixtyfour;
	}

	return dataWidth;
}

void calcCritPath(obj input, vector<operation> ops, vector<obj> outputs, double pathDelay, bool reg) {
	bool nextReg = false;
	double delay = 0.0;
	vector<operation> operations;
	operation nextOp;
	obj newInput;

	for (auto& op : ops) {
		for (auto& opIn : op.inputs) {
			if (opIn->name.compare(input.name) == 0)
			{
//				cout << "MATCH" << endl;
//				cout << "Operator " << cnt << ": " << op.getType() << endl;
//				cout << "Net Input: " << input.name << endl;
//				cout << "Op Input : " << opIn->name << endl;
				operations.push_back(op);
			}
		}
	}

//	cout << "Operations for Input " << input.name << ":" << endl;
//	for (auto& curOps : operations) {
//		cout << curOps.getType() << endl;
//	}

	if (reg) {
		if (pathDelay > finalCritPath) {
//			cout << "REGISTER REACHED: " << input.name << endl;
			if (pathDelay > finalCritPath) {
				finalCritPath = pathDelay;
			}
			pathDelay = 0.0;
		}
	}
	else {
		for (auto& output : outputs) {
			if (input.name.compare(output.name) == 0) {
//				cout << "OUTPUT REACHED: " << input.name << endl;
				if (pathDelay > finalCritPath) {
					finalCritPath = pathDelay;
				}
//				cout << "Critical Path Delay: " << getCritPath() << endl;
			}
		}
	}


	while (operations.size() > 0) {
//		cout << "Current Input: " << input.name << endl;
//		cout << operations.size() << endl;
		nextOp = operations.at(0);
		newInput = *nextOp.output;
		delay = lut.at(nextOp.getEnum()).at(getDataWidth(nextOp));
//		cout << nextOp.getType() << " Data Width " << pow(2, getDataWidth(nextOp) + 1) << endl;
//		cout << "Op Delay: " << delay << endl;
//		cout << "Current Path Delay: " << pathDelay << endl;
		pathDelay = pathDelay + delay;
//		cout << "Updated Path Delay: " << pathDelay << endl;
//		cout << endl;
		operations.erase(operations.begin());
		if (nextOp.getEnum() == 0) {
			nextReg = true;
		}
		else {
			nextReg = false;
		}
		calcCritPath(newInput, ops, outputs, pathDelay, nextReg);
		pathDelay = pathDelay - delay;
	}

//	cout << input.name << " DONE" << endl;

	return;
}

}
