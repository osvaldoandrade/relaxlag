#ifndef SOLVER_H
#define	SOLVER_H

#include <vector>
#include <string>
#include <map>
#include <time.h>
#include "problem.h"
#include "result.h"

using namespace std;

class Solver {
public:
    Solver();
    Solver(const Solver& orig);
    virtual ~Solver();
    Result solve(int type, Problem* problem);
private:
    void init(vector<double> solution, vector<double> &u, vector<double> &v);
    void update(double &pi, const vector<double> &lagSolution, vector<double> &u, vector<double> &v, double upperBound, double lowerBound);
    void reset(vector<double> &u, vector<double> &v);
    void print(const vector<double> &solution, const vector<double> &u, const vector<double> &v);
    bool isBetterPrimal(double value);
    bool isBetterDual(double value);
    double findLowerBound1(const vector<double> &lagSolution, vector<double> &solution);
    double findLowerBound2(const vector<double> &lagSolution, vector<double> &solution);
    double findLowerBound3(const vector<double> &lagSolution, vector<double> &solution);
    bool isFeasable(const vector<double> &sol);
    double findUpperBoundRelax1(vector<double> &lagSolution, vector<double> &u, vector<double> &v);
    double findUpperBoundRelax2(vector<double> &lagSolution, vector<double> &u, vector<double> &v);
    double findUpperBoundRelax3(vector<double> &lagSolution, vector<double> &u, vector<double> &v);
    double evaluate(const vector<double> &solution);
    double evaluateLagrangean(const vector<double> &solution, const vector<double> &u, const vector<double> &v);

    int getNVariables();
    int getNRelaxedConstraints();
    int getX(int i, int j);
    int getLine(int index, int j);


    double m_dual_bound;
    double m_primal_bound;
    int m_type;
    int m_max_iter;
    int m_timeout;
    int m_pi;
    int m_PI;
    int m_k;
    Problem* m_problem;
};

#endif	/* SOLVER_H */

