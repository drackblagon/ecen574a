#pragma once

#include <iostream>
#include <vector>
#include "obj.h"
#include "operation.h"
#include "netlist.h"

namespace std {
	
	void computeTimeFrame(vector<operation> * operations) {
		for (auto& ops : *operations) {
			ops.width = (ops.alapTime - ops.asapTime) + 1;
			cout << "Time Frame: " << ops.width << endl;
		}
	}

	void calcAsap(vector<operation> * operations) {
		int maxTime;
		bool done = false;
		bool good = false;
		int cnt = 0;

		cout << endl;

		while (!done) {
			done = true;
			for (auto& ops : * operations) {
				good = true;
				maxTime = 0;
				if (ops.asapTime == 0) {
					if (ops.parents.size() - 1 == 0) {
						ops.asapTime = 1;
						cout << "No Parent Operation: " << ops.op_line << endl;
						cout << "Asap Time: " << ops.asapTime << endl;
						cout << endl;
					}
					else {
						cout << "Multiple Parent Operation: " << ops.op_line << endl;
						for (auto& parent : ops.parents) {
							if (parent->asapTime == 0) {
								good = false;
								done = false;
								cout << "0 Parent ASAP Time: " << parent->asapTime << endl;
								cout << endl;
								break;
							}
						}
						if (good) {
							for (auto& par : ops.parents) {
								cout << "Parent: " << par->op_line << endl;
								cout << "Parent ASAP Time: " << par->asapTime << endl;
								if (par->asapTime > maxTime) {
									maxTime = par->asapTime;
								}
							}
							ops.asapTime = maxTime + 1;
							cout << "Asap Time: " << ops.asapTime << endl;
							cout << endl;
						}
					}
				}
			}
		}
	}

	void calcAlap(vector<operation> * operations, int latency) {
		int maxTime;
		bool done = false;
		bool good = false;
		int cnt = 0;

		cout << endl;

		while (!done) {
			done = true;
			for (auto& ops : * operations) {
				good = true;
				maxTime = latency + 1;
				if (ops.alapTime == 0) {
					if (ops.children.size() == 0) {
						ops.alapTime = latency;
						cout << "No Children Operation: " << ops.op_line << endl;
						cout << "Alap Time: " << ops.alapTime << endl;
						cout << endl;
					}
					else {
						cout << "Multiple Children Operation: " << ops.op_line << endl;
						for (auto& child : ops.children) {
							if (child->alapTime == 0) {
								good = false;
								done = false;
								cout << "0 Child Alap Time: " << child->alapTime << endl;
								cout << endl;
								break;
							}
						}
						if (good) {
							for (auto& child : ops.children) {
								cout << "Child: " << child->op_line << endl;
								cout << "Child Alap Time: " << child->alapTime << endl;
								if (child->alapTime < maxTime) {
									maxTime = child->alapTime;
								}
							}
							ops.alapTime = maxTime - 1;
							cout << "alap Time: " << ops.alapTime << endl;
							cout << endl;
						}
					}
				}
			}
		}
	}

	void calcTypeDist(vector<operation> * operations) {

	}
}


