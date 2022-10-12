// ======================================================================
// FILE:        MyAI.hpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <utility> // for pair
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <queue>
#include <string>

using namespace std;

class MyAI : public Agent
{
public:
    
    
    int times_in_ga = 0;
    int** saved_board = NULL;
    int coverd_tile;
    int tiles_to_uncover;
    //vector<int> uncovered_vec;

    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );

    int convert_coor_to_int(const int& x, const int& y);
    int convert_int_to_x(const int& value);
    int convert_int_to_y(const int& value);
    Action getAction ( int number ) override;
    void unmark_all_neighbors();
    void check_unknown_neighbors();
    void PrintBoard();
    void found_all_mines(int i, int j, bool uncover);
    void clean_safe_queue();
    
private:
    struct coordinate
    {
        int x, y;
        
        coordinate(int input_X, int input_Y)
        : x(input_X), y(input_Y)
        {}
        
        bool operator<(const coordinate& coord) const
        {
             if(x + y < coord.x + coord.y)
                return true;
             return false;
        }

        bool operator==(const coordinate& coord) const{
            return (x == coord.x && y == coord.y);

        }
    };
    
    void add_to_mine_vector_no_dupe(coordinate c);
    void print_queue(queue<coordinate> to_print);
    void print_mine_vector();
    void enumerate_all_combos();
    bool contact_frontier(const int& x, const int& y);
    bool valid_model(const string& bit_string);
    void print_guess_map();
    void print_valid_world_map();
    bool check_changed_board(const vector<int>& neighbor_check);
    bool check_all_neigh(const int& x, const int& y);
    void recur_backtrack(int bits_left, string curr_string);
    bool check_all_neigh_recur(const int& x, const int& y);

    // store the coordinate which is safe to be discovered
    queue<coordinate> safe_queue;
    
    // store the coordinate which is unknown
    //queue<coordinate> unkown_queue;
    
    // store the coordinate which is mine to be flagged
    queue<coordinate> mine_queue;
    
    //Alternative to mine_queue
    vector<coordinate> mine_vector;

    // All global variables for the model checking
    // index -> coor
    int num_frontier; // Stores the size of guess_enum_vector
    map<int, int> guess_map; //Maps coordinates of covered tiles on the frontier to index in the vector
    map<int, int> valid_world_mine_map; //Maps index of bitstring to number of times that there was a mine there
    int valid_world; //How many valid worlds were found
};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
