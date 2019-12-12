#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <tuple>
#include "obj.h"
#include "operation.h"
#include "netlist.h"

namespace std {
	enum resource {
		mult,
		addSub,
		divMod,
		logic
	};

	vector <vector<double>> prob;

	void computeTimeFrame(vector<operation> * operations) {
		for (auto& ops : *operations) {
			ops.width = (ops.alapTime - ops.asapTime) + 1;
			ops.prob = 1.0 / ops.width;
//			cout << "OP : " << ops.op_line << endl;
//			cout << "Time Frame: " << ops.width << endl;
		}
	}

	int getResource(operation * op) {
		if (!op->getType().compare("MUL")) {
			return mult;
		}
		else if (!op->getType().compare("ADD") || !op->getType().compare("SUB")) {
			return addSub;
		}
		else if (!op->getType().compare("DIV") || !op->getType().compare("MOD")) {
			return divMod;
		}
		else {
			return logic;
		}
	}

	void calcAsap(vector<operation> * operations) {
		int maxTime;
		int finalTime;
		bool done = false;
		bool good = false;
		int cnt = 0;

		//cout << endl;

		while (!done) {
			done = true;
			for (auto& ops : * operations) {
				good = true;
				maxTime = 0;
				finalTime = 0;
				if (ops.asapTime == 0) {
					if (ops.parents.size() - 1 == 0) {
						ops.asapTime = 1;
						//cout << "No Parent Operation: " << ops.op_line << endl;
						//cout << "Asap Time: " << ops.asapTime << endl;
						//cout << endl;
					}
					else {
						//cout << "Multiple Parent Operation: " << ops.op_line << endl;
						for (auto& parent : ops.parents) {
							if (parent->asapTime == 0) {
								good = false;
								done = false;
								//cout << "0 Parent ASAP Time: " << parent->asapTime << endl;
								//cout << endl;
								break;
							}
						}
						if (good) {
							for (auto& par : ops.parents) {
								//cout << "Parent: " << par->op_line << endl;
								//cout << "Parent ASAP Time: " << par->asapTime << endl;
								if (!par->getType().compare("MUL")) {
									finalTime = par->asapTime + 1;
								}
								else if (!par->getType().compare("DIV") || !par->getType().compare("MOD")) {
									finalTime = par->asapTime + 2;
								}
								else {
									finalTime = par->asapTime;
								}
								if (finalTime > maxTime) {
									maxTime = finalTime;
								}
							}
							ops.asapTime = maxTime + 1;
							//cout << "Asap Time: " << ops.asapTime << endl;
							//cout << endl;
						}
					}
				}
			}
		}
	}

	void calcAlap(vector<operation> * operations, int latency) {
		int maxTime;
		int finalTime;
		bool done = false;
		bool good = false;
		int cnt = 0;

		//cout << endl;

		while (!done) {
			done = true;
			for (auto& ops : * operations) {
				good = true;
				maxTime = latency + 1;
				finalTime = latency;
				if (ops.alapTime == 0) {
					if (ops.children.size() == 0) {
						if (!ops.getType().compare("MUL")) {
							finalTime = latency - 1;
						}
						else if (!ops.getType().compare("DIV") || !ops.getType().compare("MOD")) {
							finalTime = latency - 2;
						}
						else {
							finalTime = latency;
						}
						ops.alapTime = finalTime;
						//cout << "No Children Operation: " << ops.op_line << endl;
						//cout << "Alap Time: " << ops.alapTime << endl;
						//cout << endl;
					}
					else {
						//cout << "Multiple Children Operation: " << ops.op_line << endl;
						for (auto& child : ops.children) {
							if (child->alapTime == 0) {
								good = false;
								done = false;
								//cout << "0 Child Alap Time: " << child->alapTime << endl;
								//cout << endl;
								break;
							}
						}
						if (good) {
							for (auto& child : ops.children) {
								//cout << "Child: " << child->op_line << endl;
								//cout << "Child Alap Time: " << child->alapTime << endl;
								if (child->alapTime < maxTime) {
									maxTime = child->alapTime;
								}
							}
							if (!ops.getType().compare("MUL")) {
								finalTime = maxTime - 2;
							}
							else if (!ops.getType().compare("DIV") || !ops.op_line.compare("MOD")) {
								finalTime = maxTime - 3;
							}
							else {
								finalTime = maxTime - 1;
							}
							ops.alapTime = finalTime;
							//cout << "alap Time: " << ops.alapTime << endl;
							//cout << endl;
						}
					}
				}
			}
		}
	}

	void initProbVector(int latency) {
		prob.resize(4);
		for (int i = 0; i < 4; i++) {
			prob[i].resize(latency + 1);
			for (int j = 0; j < latency + 1; j++) {
				prob[i][j] = 0.0;
			}
		}
	}

	void calcTypeDist(vector<operation> * operations, int latency) {
		initProbVector(latency);

		for (auto& op : * operations) {
			for (int i = op.asapTime; i <= op.alapTime; ++i) {
				if (!op.getType().compare("MUL")) {
					prob[mult][i] += op.prob;
				}
				else if (!op.getType().compare("ADD") || !op.getType().compare("SUB")) {
					prob[addSub][i] += op.prob;
				}
				else if (!op.getType().compare("DIV") || !op.getType().compare("MOD")) {
					prob[divMod][i] += op.prob;
				}
				else {
					prob[logic][i] += op.prob;
				}
			}
		}
		

	}

	void printScheduledTime(vector<operation> * operations) {
		for (auto& ops : * operations) {
			cout << "Operation: " << ops.op_line << endl;
			cout << "Scheduled Time: " << ops.scheduleTime << endl;
			cout << endl;
		}
	}

	void printDistribution()
	{
		for (unsigned int i = 0; i <= 3; ++i)
		{
			switch (i)
			{
			case mult: cout << "Multi" << endl; break;
			case addSub: cout << "Add/Sub" << endl; break;
			case divMod: cout << "Div/Mod" << endl; break;
			case logic: cout << "Logic" << endl; break;
			default: break;
			}

			for (unsigned int col = 0; col <= 1; ++col)
			{
				cout << "\t";
				for (unsigned int row = 1; row < prob[i].size(); ++row)
				{
					if (col == 0)
						cout << row << "\t";
					else
						cout << setprecision(2) << prob[i][row] << "\t";
				}
				cout << endl;
			}
			cout << endl << endl;
		}
	}

	void resetFlag(vector<operation> * operations)
	{
		for (auto& ops : * operations)
		{
			ops.visited = false;
		}
	}

	tuple<int, double> forceDir(vector<operation> * operations, operation * op, int cycle, bool first, bool Successor)
	{
		tuple<int, double> tempForce(-1, 0.0);
		tuple<int, double> forceSum(-1, 10000.0);
		double probability;
		int timing = 0;
		int resourceVector = getResource(op);


		op->visited = true;

		if ((cycle < op->asapTime || op->alapTime < cycle) && !first)
		{
			tuple<int, double> empty(0, 0.0);
			return empty;
		}

		for (int cycle = op->asapTime; cycle <= op->alapTime; ++cycle)
		{
			if (first)
				resetFlag(operations);
			for (int k = 0; k < prob[resourceVector].size(); ++k)
			{
				if (!first)
				{
					if (Successor && (k <= cycle))
					{
						probability = 0.0;
					}
					else if (!Successor && (k >= cycle))
					{
						probability = 0.0;
					}
					else if (cycle == k)
						probability = 1;
					else
						probability = 0.0;
				}
				else if (cycle == k)
					probability = 1;
				else
					probability = 0.0;
				get<1>(tempForce) += prob[resourceVector][k] * (probability - op->prob);
			}

			for (int i = 0; i < op->parents.size(); ++i)
			{
				if (!op->parents[i]->visited)
					get<1>(tempForce) += get<1>(forceDir(operations, op->parents[i], cycle, false, false));
			}
			for (int i = 0; i < op->children.size(); ++i)
			{
				if (!op->children[i]->visited)
					get<1>(tempForce) += get<1>(forceDir(operations, op->children[i], cycle, false, true));
			}
			if (get<1>(tempForce) < get<1>(forceSum))
			{
				get<1>(forceSum) = get<1>(tempForce);
				if (first)
					get<0>(forceSum) = cycle;
			}
		}
		return forceSum;
	}

	void calcForce(vector<operation> * operations, int latency) {
		for (auto& ops : *operations) {
			ops.scheduleTime = get<0>(forceDir(operations, &ops, 0, true, true));
			ops.asapTime = ops.scheduleTime;
			ops.alapTime = ops.scheduleTime;
			ops.prob = 1.0;
			printDistribution();
			calcTypeDist(operations, latency);
			printDistribution();
		}
	}

}


