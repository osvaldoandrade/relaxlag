#ifndef RESULT_H
#define	RESULT_H

#include <string>

#include "problem.h"

using namespace std;

class Result {
public:
    Result();
    Result(vector<double> solution, Problem* problem, long duration, long iterations, bool optimal, double dualBound, double primalBound);
    Result(const Result& orig);

    virtual ~Result();
    
    void setDebug(bool debug);
private:
    vector<double> m_solution;
    Problem* m_problem;
    int m_debug;
    long m_duration;
    long m_iterations;
    bool m_optimun;
    double m_dual_bound;
    double m_primal_bound;

    friend ostream& operator<<(ostream& os, Result &result);
};

#endif	/* RESULT_H */

