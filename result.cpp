#include <string>
#include "result.h"
#include "problem.h"

using namespace std;

Result::Result() {
}

Result::Result(vector<double> solution, Problem* problem, long duration, long iterations, bool optimal, double dualBound, double primalBound) {
    this->m_solution = solution;
    this->m_duration = duration;
    this->m_optimun = optimal;
    this->m_dual_bound = dualBound;
    this->m_primal_bound = primalBound;
    this->m_problem = problem;
    this->m_debug = false;
    this->m_iterations = iterations;
}

Result::Result(const Result& orig) {
    this->m_duration = orig.m_duration;
    this->m_optimun = orig.m_optimun;
    this->m_solution = orig.m_solution;
    this->m_problem = orig.m_problem;
    this->m_debug = orig.m_debug;
    this->m_dual_bound = orig.m_dual_bound;
    this->m_primal_bound = orig.m_primal_bound;
    this->m_iterations = orig.m_iterations;
}

Result::~Result() {
}

void Result::setDebug(bool debug) {
    this->m_debug = debug;
}

ostream& operator<<(ostream &strm, Result & result) {
    strm.precision(6);

    if (!result.m_debug) {
	strm << result.m_dual_bound << endl << result.m_primal_bound << endl;

	for (int i = 0; i < result.m_problem->getNTasks(); i++) {
	    bool atribuida = false;
	    for (int j = 0; j < result.m_problem->getNAgents(); j++) { //para cada agente
		double value = result.m_solution[i * result.m_problem->getNAgents() + j];

		if (value) {
		    strm << j << endl;
		    atribuida = true;
		}
		if (!atribuida)
		    strm << -1 << endl;
	    }
	}
    }

    if (result.m_debug) {
	strm << fixed;
	strm << "L. Inferior: " << result.m_primal_bound;
	strm << " L. Superior: " << result.m_dual_bound;
	strm << " Erro: " << ((result.m_dual_bound - result.m_primal_bound) / result.m_primal_bound) * 100;
	strm << " Otimo: " << (((result.m_dual_bound - result.m_primal_bound) / result.m_primal_bound) * 100 < 0.01);
	strm << " Iter: " << result.m_iterations;
	strm << " Clock ticks: " << result.m_duration << endl;
    }

    return strm;
}


