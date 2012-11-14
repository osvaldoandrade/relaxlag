#include <numeric>
#include <limits>
#include <cmath>
#include <map>

#include "solver.h"
#include "knapsack.h"

Solver::Solver() {
}

Solver::Solver(const Solver& orig) {
    this->m_max_iter = orig.m_max_iter;
    this->m_dual_bound = orig.m_dual_bound;
    this->m_primal_bound = orig.m_primal_bound;
    this->m_timeout = orig.m_timeout;
    this->m_problem = orig.m_problem;
}

Solver::~Solver() {
}

Result Solver::solve(int type, Problem* problem) {
    m_dual_bound = numeric_limits<double>::max();
    m_primal_bound = numeric_limits<double>::min();

    this->m_type = type;
    this->m_problem = problem;

    //Config properties
    int MAX_ITER = 2000;
    double PI = 2;
    double MIN_PI = 0.001;
    double MAX_NOT_IMPROVED = 30;

    //Init variable 
    vector<double> u;
    vector<double> v;
    vector<double> lag_solution;
    vector<double> primal_solution;
    vector<double> best_solution;
    int nImproved = 0;

    init(lag_solution, u, v);

    int i;
    for (i = 0; i < MAX_ITER; i++) {

	double lagrangean_cost = 0.0;

	if (type == 1) {
	    lagrangean_cost = findUpperBoundRelax1(lag_solution, u, v);
	} else if (type == 2) {
	    lagrangean_cost = findUpperBoundRelax2(lag_solution, u, v);
	} else if (type == 3) {
	    lagrangean_cost = findUpperBoundRelax3(lag_solution, u, v);
	}

	if (isBetterDual(lagrangean_cost)) {
	    m_dual_bound = lagrangean_cost;
	    nImproved = 0;
	} else
	    nImproved++;

	double primal_cost = 0.0;
	if (type == 1)
	    primal_cost = findLowerBound1(lag_solution, primal_solution);
	if (type == 2)
	    primal_cost = findLowerBound2(lag_solution, primal_solution);
	if (type == 3)
	    primal_cost = findLowerBound3(lag_solution, primal_solution);


	if (isBetterPrimal(primal_cost)) {
	    m_primal_bound = primal_cost;
	    best_solution = primal_solution;
	}

	if (fabs(m_primal_bound - m_dual_bound) < 1e-5)
	    break;

	if (nImproved == MAX_NOT_IMPROVED) {
	    nImproved = 0;
	    PI /= 2.0;
	    if (PI < MIN_PI) {
		break;
	    }
	    reset(u, v);
	}

	update(PI, lag_solution, u, v, m_dual_bound, m_primal_bound);
    }

    return Result(best_solution, m_problem, clock(), i, false, m_dual_bound, m_primal_bound);
}

double Solver::findLowerBound1(const vector<double> &lagSolution, vector<double> &sol) {

    sol.assign(getNVariables(), 0);
    for (int i = 0; i < m_problem->getNTasks(); i++)
	for (int j = 0; j < m_problem->getNAgents(); j++)
	    sol[getX(i, j)] = lagSolution[getX(i, j)];

    if (isFeasable(lagSolution)) {
	return evaluate(lagSolution);
    }

    vector<double> r2(m_problem->getNTasks());
    vector<double> r3(m_problem->getNAgents());

    r2.assign(m_problem->getNTasks(), 0);
    r3.assign(m_problem->getNAgents(), 0);

    int m = m_problem->getNTasks();
    int n = m_problem->getNAgents();

    //Calculando custos da restrição 1 e 2
    for (int i = 0; i < m; i++) //para cada tarefa i
	for (int j = 0; j < n; j++) { // para cada agente j
	    r2[i] = r2[i] + sol[getX(i, j)];
	    r3[j] += sol[getX(i, j)] * m_problem->getLoad(i, j);
	}

    for (int i = 0; i < m; i++) {
	double custo = 1 - r2[i];
	int maior = 0;
	int atual = 0;

	if (custo >= 0) continue;

	for (int j = 0; j < n; j++) {
	    maior = m_problem->getGain(i, j) > m_problem->getGain(i, maior) ? j : maior;

	    if (sol[getX(i, j)]) {
		sol[getX(i, j)] = 0;
		atual = j;
		custo--;
	    }
	}
	if (m_problem->getGain(i, maior) > m_problem->getGain(i, atual)) {
	    sol[getX(i, maior)] = 1;
	    sol[getX(i, atual)] = 0;

	} else {
	    sol[getX(i, maior)] = 0;
	    sol[getX(i, atual)] = 1;
	}
    }


    // Recalcula os custos
    r2.assign(m_problem->getNTasks(), 0);
    r3.assign(m_problem->getNAgents(), 0);
    for (int i = 0; i < m; i++) //para cada tarefa i
	for (int j = 0; j < n; j++) { // para cada agente j
	    r2[i] = r2[i] + sol[getX(i, j)];
	    r3[j] += sol[getX(i, j)] * m_problem->getLoad(i, j);
	}

    for (int j = 0; j < r3.size(); j++) {
	while (m_problem->getCapacity(j) < r3[j]) {
	    int menor = -1;
	    for (int i = 0; i < m_problem->getNTasks(); i++)
		if (sol[getX(i, j)] == 1) {
		    if (menor == -1)
			menor = i;
		    else
			menor = (m_problem->getGain(i, j) < m_problem->getGain(menor, j)) ? i : menor;
		}

	    sol[getX(menor, j)] = 0;
	    r2[menor]--;
	    r3[j] -= m_problem->getLoad(menor, j);
	}
    }

    // Recalcula os custos
    r2.assign(m_problem->getNTasks(), 0);
    r3.assign(m_problem->getNAgents(), 0);
    for (int i = 0; i < m; i++) //para cada tarefa i
	for (int j = 0; j < n; j++) { // para cada agente j
	    r2[i] = r2[i] + sol[getX(i, j)];
	    r3[j] += sol[getX(i, j)] * m_problem->getLoad(i, j);
	}

    /**Tenta melhorar a solução, pois nesse ponto temos uma solução viável, no entanto pode ser fraca. Para cada 
     * linha, identificaremos se podemos setar "1" sem que a[i][j] estoure a b[j] e que a restrição um 
     * continue sendo satisfeita. */
    for (int i = 0; i < m; i++) {
	int melhor = -1;
	if (r2[i] == 0)
	    for (int j = 0; j < n; j++)
		melhor = (m_problem->getGain(i, j) > m_problem->getGain(i, melhor)) && (r3[j] + m_problem->getLoad(i, j) < m_problem->getCapacity(j)) ? j : melhor;

	if (melhor != -1) {
	    sol[getX(i, melhor)] = 1;
	    r2[i] += 1;
	    r3[melhor] += m_problem->getLoad(i, melhor);
	}
    }

    return evaluate(sol);
}

double Solver::findLowerBound2(const vector<double> &lagSolution, vector<double> &sol) {

    sol.assign(getNVariables(), 0);
    for (int i = 0; i < m_problem->getNTasks(); i++)
	for (int j = 0; j < m_problem->getNAgents(); j++)
	    sol[getX(i, j)] = lagSolution[getX(i, j)];

    if (isFeasable(lagSolution)) {
	return evaluate(lagSolution);
    }

    vector<double> r2(m_problem->getNTasks());
    vector<double> r3(m_problem->getNAgents());

    r2.assign(m_problem->getNTasks(), 0);
    r3.assign(m_problem->getNAgents(), 0);

    int m = m_problem->getNTasks();
    int n = m_problem->getNAgents();

    //Calculando custos da restrição 1 e 2
    for (int i = 0; i < m; i++) //para cada tarefa i
	for (int j = 0; j < n; j++) { // para cada agente j
	    r2[i] = r2[i] + sol[getX(i, j)];
	    r3[j] += sol[getX(i, j)] * m_problem->getLoad(i, j);
	}

    for (int i = 0; i < m; i++) {
	double custo = 1 - r2[i];
	int maior = 0;
	int atual = 0;

	if (custo >= 0) continue;

	for (int j = 0; j < n; j++) {
	    maior = m_problem->getGain(i, j) > m_problem->getGain(i, maior) ? j : maior;

	    if (sol[getX(i, j)]) {
		sol[getX(i, j)] = 0;
		atual = j;
		custo--;
	    }
	}
	if (m_problem->getGain(i, maior) > m_problem->getGain(i, atual)) {
	    sol[getX(i, maior)] = 1;
	    sol[getX(i, atual)] = 0;

	} else {
	    sol[getX(i, maior)] = 0;
	    sol[getX(i, atual)] = 1;
	}
    }

    // Recalcula os custos
    r2.assign(m_problem->getNTasks(), 0);
    r3.assign(m_problem->getNAgents(), 0);
    for (int i = 0; i < m; i++) //para cada tarefa i
	for (int j = 0; j < n; j++) { // para cada agente j
	    r2[i] = r2[i] + sol[getX(i, j)];
	    r3[j] += sol[getX(i, j)] * m_problem->getLoad(i, j);
	}

    /**Tenta melhorar a solução, pois nesse ponto temos uma solução viável, no entanto pode ser fraca. Para cada 
     * linha, identificaremos se podemos setar "1" sem que a[i][j] estoure a b[j] e que a restrição um 
     * continue sendo satisfeita. */
    for (int i = 0; i < m; i++) {
	int melhor = -1;
	if (r2[i] == 0)
	    for (int j = 0; j < n; j++)
		melhor = (m_problem->getGain(i, j) > m_problem->getGain(i, melhor)) && (r3[j] + m_problem->getLoad(i, j) < m_problem->getCapacity(j)) ? j : melhor;

	if (melhor != -1) {
	    sol[getX(i, melhor)] = 1;
	    r2[i] += 1;
	    r3[melhor] += m_problem->getLoad(i, melhor);
	}
    }

    return evaluate(sol);
}

double Solver::findLowerBound3(const vector<double> &lagSolution, vector<double> &sol) {

    sol.assign(getNVariables(), 0);
    for (int i = 0; i < m_problem->getNTasks(); i++)
	for (int j = 0; j < m_problem->getNAgents(); j++)
	    sol[getX(i, j)] = lagSolution[getX(i, j)];

    if (isFeasable(lagSolution)) {
	return evaluate(lagSolution);
    }


    vector<double> r2(m_problem->getNTasks());
    vector<double> r3(m_problem->getNAgents());

    r2.assign(m_problem->getNTasks(), 0);
    r3.assign(m_problem->getNAgents(), 0);

    int m = m_problem->getNTasks();
    int n = m_problem->getNAgents();

    //Calculando custos da restrição 1 e 2
    for (int i = 0; i < m; i++) //para cada tarefa i
	for (int j = 0; j < n; j++) { // para cada agente j
	    r2[i] = r2[i] + sol[getX(i, j)];
	    r3[j] += sol[getX(i, j)] * m_problem->getLoad(i, j);
	}

    for (int j = 0; j < r3.size(); j++) {
	if (m_problem->getCapacity(j) < r3[j])
	    while (m_problem->getCapacity(j) < r3[j]) {
		int menor = -1;
		for (int i = 0; i < m; i++)
		    if (sol[getX(i, j)] == 1)
			menor = menor == -1 ? i : m_problem->getGain(i, j) < m_problem->getGain(menor, j) ? i : menor;

		sol[getX(menor, j)] = 0;
		r2[menor]--;
		r3[j] -= m_problem->getLoad(menor, j);
	    }
    }

    for (int i = 0; i < m; i++) {
	int melhor = -1;
	if (r2[i] == 0) {
	    for (int j = 0; j < n; j++)
		melhor = (m_problem->getGain(i, j) > m_problem->getGain(i, melhor)) && (r3[j] + m_problem->getLoad(i, j) <= m_problem->getCapacity(j)) ? j : melhor;

	    if (melhor != -1) {
		sol[getX(i, melhor)] = 1;
		r2[i] += 1;
		r3[melhor] += m_problem->getLoad(i, melhor);
	    }
	}
    }


    return evaluate(sol);
}

bool Solver::isFeasable(const vector<double> &sol) {

    vector<double> r2;
    vector<double> r3;
    r2.assign(m_problem->getNTasks(), 0);
    r3.assign(m_problem->getNAgents(), 0);

    int m = m_problem->getNTasks();
    int n = m_problem->getNAgents();

    //Calculando custos da restrição 1 e 2
    for (int i = 0; i < m; i++) //para cada tarefa i
	for (int j = 0; j < n; j++) { // para cada agente j
	    r2[i] = r2[i] + sol[getX(i, j)];
	    r3[j] += sol[getX(i, j)] * m_problem->getLoad(i, j);
	    if (1 - r2[i] < 0 || m_problem->getCapacity(j) - r3[j] < 0)
		return false;
	}

    return true;
}

double Solver::findUpperBoundRelax3(vector<double> &lagSolution, vector<double> &u, vector<double> &v) {
    lagSolution.assign(getNVariables(), 0);

    for (int i = 0; i < m_problem->getNTasks(); i++) {
	int maior = 0;
	for (int j = 1; j < m_problem->getNAgents(); j++)
	    maior = (m_problem->getGain(i, j) - (m_problem->getLoad(i, j) * v[j]) > m_problem->getGain(i, maior) - (m_problem->getLoad(i, maior) * v[maior])) ? j : maior;
	lagSolution[getX(i, maior)] = 1;
    }

    return evaluateLagrangean(lagSolution, u, v);
}

double Solver::findUpperBoundRelax2(vector<double> &lagSolution, vector<double> &u, vector<double> &v) {
    lagSolution.assign(getNVariables(), 0);

    int m = m_problem->getNTasks();
    int n = m_problem->getNAgents();

    for (int j = 0; j < n; j++) {
	Knapsack ks((int) m_problem->getCapacity(j), m);
	for (int i = 0; i < m; i++)
	    ks.addItems(i, (int) m_problem->getGain(i, j) - u[i], (int) m_problem->getLoad(i, j));
	int solution = ks.solve();

	std::vector<item> resultItems;

	ks.getSelected(resultItems);
	for (vector<item>::iterator it = resultItems.begin(); it != resultItems.end(); it++)
	    lagSolution[getX(it->index, j)] = 1;
    }

    return this->evaluateLagrangean(lagSolution, u, v);
}

/* This heuristic just find out what variables would increase the objective function and if so, set 1 to then, 
 * or 0 otherwise*/
double Solver::findUpperBoundRelax1(vector<double> &lagSolution, vector<double> &u, vector<double> &v) {
    lagSolution.assign(getNVariables(), 0);

    for (int i = 0; i < m_problem->getNTasks(); i++) 
	for (int j = 0; j < m_problem->getNAgents(); j++)
	    lagSolution[getX(i, j)] = (m_problem->getGain(i, j) - (u[i]) - (m_problem->getLoad(i, j) * v[j])) > 0;

    return this->evaluateLagrangean(lagSolution, u, v);
}

void Solver::reset(vector<double> &u, vector<double> &v) {
    u.assign(u.size(), 0);
    v.assign(v.size(), 0);
}

void Solver::init(vector<double> solution, vector<double> &u, vector<double> &v) {
    solution.assign(getNVariables(), 0);
    u.assign(m_problem->getNTasks(), 0);
    v.assign(m_problem->getNAgents(), 0);
}

void Solver::print(const vector<double> &sol, const vector<double> &u, const vector<double> &v) {

    cout << "-----------------------------------------" << endl;
    cout << "X[] = " << endl;

    for (int i = 0; i < m_problem->getNTasks(); i++) {
	for (int j = 0; j < m_problem->getNAgents(); j++)
	    cout << " " << sol[getX(i, j)] << "(g:" << m_problem->getGain(i, j) << " l:" << m_problem->getLoad(i, j) << ") ";
	cout << endl;
    }

    cout << "U[] { " << u[0];
    for (int i = 1; i < u.size(); i++)
	cout << " , " << u[i];
    cout << " } " << endl << endl;

    cout << "V[] { " << v[0];
    for (int i = 1; i < v.size(); i++)
	cout << " , " << v[i];
    cout << " } " << endl << endl;

    cout << "IsFeasable: " << isFeasable(sol) << endl;

    vector<double> x;
}

void Solver::update(double &pi, const vector<double> &solution, vector<double> &u, vector<double> &v, double upperBound, double lowerBound) {
    vector<double> weight;

    if (m_type == 1 || m_type == 2) {
	double gradLenght = 0;
	weight.resize(m_problem->getNTasks());
	double rhs = 1;
	for (size_t i = 0; i < m_problem->getNTasks(); i++) {
	    for (size_t j = 0; j < m_problem->getNAgents(); j++) {
		weight[i] += solution[getX(i, j)];
	    }
	    weight[i] = rhs - weight[i];
	}

	for (size_t i = 0; i < weight.size(); i++)
	    gradLenght += weight[i] * weight[i];

	double t = pi * 0.8 * (upperBound - lowerBound) / gradLenght;

	for (size_t i = 0; i < u.size(); i++) {
	    u[i] = max(0.0, u[i] - (t * weight[i]));
	}
    }

    if (m_type == 1 || m_type == 3) {
	double gradLenght = 0;
	weight.resize(m_problem->getNAgents());
	weight.assign(weight.size(), 0);
	for (int i = 0; i < m_problem->getNAgents(); i++) {
	    double rhs = m_problem->getCapacity(i);
	    for (int j = 0; j < m_problem->getNTasks(); j++) {
		weight[i] += solution[getX(j, i)] * m_problem->getLoad(j, i);
	    }
	    weight[i] = rhs - weight[i];
	}

	for (size_t i = 0; i < weight.size(); i++)
	    gradLenght += weight[i] * weight[i];

	double t = pi * 0.8 * (upperBound - lowerBound) / gradLenght;

	for (size_t i = 0; i < v.size(); i++) {
	    v[i] = max(0.0, v[i] - (t * weight[i]));
	}
    }
}

bool Solver::isBetterDual(double value) {
    return value < m_dual_bound;
}

bool Solver::isBetterPrimal(double value) {
    return value > m_primal_bound;
}

double Solver::evaluate(const vector<double> &solution) {
    double total = 0.0;
    for (int i = 0; i < m_problem->getNTasks(); i++)
	for (int j = 0; j < m_problem->getNAgents(); j++)
	    total += (solution[getX(i, j)] * m_problem->getGain(i, j));

    return total;
}

double Solver::evaluateLagrangean(const vector<double> &solution, const vector<double> &u, const vector<double> &v) {
    double total = 0.0;
    total = evaluate(solution);
    if (m_type == 1 || m_type == 3)
	for (int j = 0; j < m_problem->getNAgents(); j++) {
	    total += v[j] * m_problem->getCapacity(j);
	    for (int i = 0; i < m_problem->getNTasks(); i++)
		total += v[j] * (-1 * solution[getX(i, j)]) * m_problem->getLoad(i, j);
	}

    if (m_type == 1 || m_type == 2)
	for (int i = 0; i < m_problem->getNTasks(); i++) {
	    total += u[i] * 1;
	    for (int j = 0; j < m_problem->getNAgents(); j++)
		total += u[i] * (-1 * solution[getX(i, j)]);
	}
    return total;
}

int Solver::getX(int i, int j) {
    return i * m_problem->getNAgents() + j;
}

int Solver::getLine(int index, int j) {
    return (index - j) / m_problem->getNAgents();
}

int Solver::getNVariables() {
    return m_problem->getNAgents() * m_problem->getNTasks();
}

int Solver::getNRelaxedConstraints() {
    if (m_type == 1) {
	return m_problem->getNAgents() + m_problem->getNAgents();
    }

    if (m_type == 2) {
	return m_problem->getNAgents();
    }

    if (m_type == 3) {
	return m_problem->getNTasks();
    }
}




