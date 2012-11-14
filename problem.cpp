#include "problem.h"

Problem::Problem() {
}

Problem::Problem(const Problem& orig) {
    this->capacity = orig.capacity;
    this->gain = orig.gain;
    this->load = orig.load;
    this->m_agents = orig.m_agents;
    this->m_tasks = orig.m_tasks;
}

Problem::~Problem() {
}

int Problem::getNAgents() {
    return this->m_agents;
}

int Problem::getNTasks() {
    return this->m_tasks;
}

double Problem::getGain(int i, int j) {
    if (i < 0 || j < 0) return -1;
    return this->gain[i][j];
}

double Problem::getLoad(int i, int j) {
    if (i < 0 || j < 0) return -1;
    return this->load[i][j];
}

double Problem::getCapacity(int j) {
    if (j < 0) return -1;
    return this->capacity[j];
}

vector<vector<double> > Problem::getGain() {
    return this->gain;
}

vector<vector<double> > Problem::getLoad() {
    return this->load;
}

vector<double> Problem::getCapacity() {
    return this->capacity;
}

void Problem::setup(const char* filename) {

    ifstream fin(filename);

    fin >> m_agents >> m_tasks;

    //Initializing vectors gain, load and capacity
    gain.resize(m_tasks);
    load.resize(m_tasks);
    capacity.resize(m_agents);

    for (int i = 0; i < m_tasks; i++) {
	gain[i].resize(m_agents);
	load[i].resize(m_agents);
    }

    //Reading gain vector from input file
    for (int j = 0; j < m_agents; j++) {
	for (int i = 0; i < m_tasks; i++) {
	    fin >> gain[i][j];
	}
    }

    //Reading load vector from input file
    for (int j = 0; j < m_agents; j++) {
	for (int i = 0; i < m_tasks; i++) {
	    fin >> load[i][j];
	}
    }

    //Reading capacity vector from input file
    for (int j = 0; j < m_agents; j++)
	fin >> capacity[j];

}


