#include "knapsack.h"

Knapsack::Knapsack(int knapsack_size, int item_size) :
m_capacity(knapsack_size),
m_n_objects(item_size),
m_table(item_size + 1, std::vector<int>(knapsack_size + 1)),
m_selected(item_size + 1, std::vector<int>(knapsack_size + 1)) {
    m_items.reserve(m_n_objects);
}

void Knapsack::addItems(int index, int value, int capacity) {
    item t;
    t.index = index;
    t.value = value;
    t.capacity = capacity;
    m_items.push_back(t);
}

int Knapsack::solve() {
    for (int i = 0; i < m_capacity + 1; i++) {
	m_table [0][i] = 0;
	m_selected[0][i] = 0;
    }
    int row = 1;
    for (std::vector<item>::iterator itemIterator = m_items.begin();
	    itemIterator != m_items.end();
	    ++itemIterator) {
	item currentItem = *itemIterator;
	int col = 0; // col is capacity available.
	while (col < m_capacity + 1) {
	    if (currentItem.capacity > col) {
		m_table[row][col] = m_table[row - 1][col];
		m_selected[row][col] = 0;
	    } else {
		int capacity_remaining = col - currentItem.capacity;
		int new_value = currentItem.value + m_table[row - 1][capacity_remaining];
		int prev_value = m_table[row - 1][col];
		if (prev_value >= new_value) {
		    m_table[row][col] = m_table[row - 1][col];
		    m_selected[row][col] = 0;
		} else {
		    m_table[row][col] = new_value;
		    m_selected[row][col] = 1;
		}
	    }
	    col++;
	}
	row++;
    }
    return m_table[m_n_objects][m_capacity];
}

void Knapsack::getSelected(std::vector<item>& resultItems) {
    int row = m_n_objects;
    int col = m_capacity;
    int cap = m_capacity;
    while (cap > 0 && row >= 0) {
	if (m_selected[row][col] == 1) {
	    resultItems.push_back(m_items[row - 1]);
	    cap = cap - m_items[row - 1].capacity;
	    col = cap;
	}
	row = row - 1;
    }
}

Knapsack::~Knapsack() {
}

