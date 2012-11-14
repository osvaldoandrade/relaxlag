#ifndef GAPPROBLEM_H
#define	GAPPROBLEM_H

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

class Problem  {
public:
    Problem();
    Problem(const Problem& orig);
    virtual ~Problem();

    void setup(const char* filename);

    int getNTasks();
    int getNAgents();

    vector< vector<double> > getGain();
    vector< vector<double> > getLoad();
    vector< double> getCapacity();

    double getGain(int i, int j);
    double getLoad(int i, int j);
    double getCapacity(int j);

private:

    int m_tasks;
    int m_agents;

    vector< vector<double> > gain;
    vector< vector<double> > load;
    vector<double> capacity;
};

#endif	/* GAPPROBLEM_H */

