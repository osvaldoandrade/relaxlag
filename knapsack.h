#ifndef KNAPSACK_H
#define	KNAPSACK_H

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>

struct item {
    int index;
    int value;
    int capacity;
};

class Knapsack {
public:
    Knapsack(int capacity, int nObjects);
    ~Knapsack();
    void addItems(int index, int value, int capacity);
    int solve();
    void getSelected(std::vector<item>& result);
private:
    void init();
    std::vector<item> m_items;
    int m_capacity;
    int m_n_objects;
    std::vector<std::vector<int> > m_table;
    std::vector<std::vector<int> > m_selected;
protected:
};

#endif	/* SOLVER_H */
