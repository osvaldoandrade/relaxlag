#include <cstdlib>

#include <string>
#include <iostream>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>

#include "problem.h"
#include "solver.h"
#include "result.h"

using namespace std;

int main(int argc, char** argv);

Problem* read_problem(const char* filename);
Result solve_relax1(const char* filename);
Result solve_relax2(const char* filename);
Result solve_relax3(const char* filename);
void print_usage();

int main(int argc, char** argv) {
    int debug = 0;
    int latex = 0;
    int type;
    char* in;
    char* out;
    Result result;

    if (argc < 4) {
	print_usage();
	return -1;
    }

    try {
	type = atoi(argv[1]);
	in = argv[2];
	out = argv[3];

	if (argc == 5)
	    debug = atoi(argv[4]);

	if (argc == 6)
	    latex == atoi(argv[5]);
    } catch (...) {
	print_usage();
	return -1;
    }

    typedef std::numeric_limits< double > dbl;

    std::numeric_limits<double>::digits10;
    ofstream fout(out);
    fout.precision(6);

    if (type == 1)
	result = solve_relax1(in);

    if (type == 2)
	result = solve_relax2(in);

    if (type == 3)
	result = solve_relax3(in);

    result.setDebug(debug == 1);

    if(debug)
	cout << result;
    
    fout << result;
    return 0;
}

Problem * read_problem(const char* filename) {
    Problem *p = new Problem();
    p->setup(filename);

    return p;
}

Result solve_relax1(const char* filename) {
    Problem *p = read_problem(filename);

    Solver solver;
    return solver.solve(1, p);
}

Result solve_relax2(const char* filename) {
    Problem *p = read_problem(filename);

    Solver solver;
    return solver.solve(2, p);
}

Result solve_relax3(const char* filename) {
    Problem *p = read_problem(filename);

    Solver solver;
    return solver.solve(3, p);
}

void print_usage() {
    cout << "Usage: relaxlag <relax_type> <input_file> <output_file> [debug] [latex]" << endl << endl;
    cout << "<relax_type>  :Required - Type of relaxation to use." << endl;
    cout << "\t1 - Solve the problem by the Relaxation 1 (Inspection)." << endl;
    cout << "\t2 - Solve the problem by the Relaxation 2 (Knapsack)." << endl;
    cout << "\t3 - Solve the problem by the Relaxation 3 (Inspection)." << endl << endl;
    cout << "<input_file>  :Required - Full path for the instance test of the Gap problem." << endl << endl;
    cout << "<output_file> :Required - Full path for output test with results." << endl << endl;
    cout << "[debug]       :Optional - 1 to enable Debug 0 (default) otherwise." << endl;
    cout << "[latex]       :Optional - 1 to generate LATeX table 0 (default) otherwise." << endl;
}




