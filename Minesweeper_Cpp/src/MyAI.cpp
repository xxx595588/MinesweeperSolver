// ======================================================================
// FILE:        MyAI.cpp
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

// - SAVED BOARD INT MEANING
// - (-1) MEANS COVERED
// - (-2) MEANS SAFE TO BE UNCOVERED
// - (n from {1,2,3,4,5,6,7,7,9}) MEANS N MINES IN VICINITY
// - (10) MEANS FLAGGED AS MINE


#include "MyAI.hpp"
#include <cmath>

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    rowDimension = _rowDimension;
    colDimension = _colDimension;
    totalMines = _totalMines;
    agentX = _agentX;
    agentY = _agentY;
    coverd_tile = rowDimension * colDimension;
    tiles_to_uncover = (rowDimension * colDimension) - _totalMines;

    //Make board
    saved_board = new int*[rowDimension];
    for(int i = 0; i < rowDimension; ++i){
        saved_board[i] = new int[colDimension];
        for(int j = 0; j < colDimension; ++j){
            saved_board[i][j] = -1;
        }
    }
};

int MyAI::convert_coor_to_int(const int& x, const int& y){return (x * colDimension) + y;}

int MyAI::convert_int_to_x(const int& value){return (value / colDimension);}

int MyAI::convert_int_to_y(const int& value){return (value % colDimension);}

void MyAI::print_queue(queue<coordinate> to_print){
    //Makes a copy of the provided queue and prints it out
    queue<coordinate> pq (to_print);
    while(!pq.empty()){
        cout << "[X: " << pq.front().x << ", Y: " << pq.front().y << "]";
        pq.pop();
    }
    cout << "" << endl;
}


Agent::Action MyAI::getAction( int number )
{
    //flag
    if(number == -1 && saved_board[agentY][agentX] == -1){
        saved_board[agentY][agentX] = 10;
        totalMines--;
        coverd_tile--;
    }

    //unflag
    else if(number == -1 && saved_board[agentY][agentX] == 10){
        saved_board[agentY][agentX] = -1;
        totalMines++;
        coverd_tile++;
    }

    //uncover
    else{
        saved_board[agentY][agentX] = number;
        coverd_tile--;
        tiles_to_uncover--;
    }

    if (coverd_tile == 0){
        return {LEAVE,-1,-1};
    }
    
    //If there are no more mines to find, discover every tile:
    if(totalMines == 0){
       for(int i = 0; i < rowDimension; ++i){
            for(int j = 0; j < colDimension; ++j){
                if(saved_board[i][j] == -1){
                    agentX = j;
                    agentY = i;
                    safe_queue.push(MyAI::coordinate(j, i));

                }
            }
        }
    }
        
    // check if (x,y) return 0 ,if so, uncover all neighbors
    unmark_all_neighbors();
    clean_safe_queue();
    if(!safe_queue.empty())
    {
        agentX = safe_queue.front().x;
        agentY = safe_queue.front().y;
        
        safe_queue.pop();
        //if (saved_board[agentY][agentX] >= 0){cout << "DUPLICATE LOCATED!!!! -1 " << convert_coor_to_int(agentX, agentY) << endl;}
        return {UNCOVER, agentX, agentY};
    }
    
    // check all unknown neighbors
    clean_safe_queue();
    check_unknown_neighbors();
    if(!safe_queue.empty())
    {
        agentX = safe_queue.front().x;
        agentY = safe_queue.front().y;
        
        safe_queue.pop();
        //if (saved_board[agentY][agentX] >= 0){cout << "DUPLICATE LOCATED!!!! -2" << endl;}

        return {UNCOVER, agentX, agentY};
    }

    if(mine_vector.size() > 0){
        agentX = mine_vector.back().x;
        agentY = mine_vector.back().y;

        mine_vector.pop_back();
        return {FLAG, agentX, agentY};
    }

    //Now we have gotten to the model checking
    //This will enumerate all possible frontiers and check which ones are valid, the uncover the tile that is least likely to have the mine

    //First, set all the tools we will use back to their default.
    valid_world_mine_map.clear();
    guess_map.clear();
    num_frontier = 0;
    valid_world = 0;
    
    int counter = 0;

    for(int i = 0; i < rowDimension; ++i){
        for(int j = 0; j < colDimension; ++j){
            if(saved_board[i][j] == -1 && contact_frontier(j , i)){
                guess_map.insert(pair<int, int>(counter, convert_coor_to_int(j, i)));
                counter++;
            
            //if(colDimension == 30 && rowDimension == 16 && counter == 19)
            if((colDimension == 30 && rowDimension == 16 && counter == 19) || (colDimension == 16 && rowDimension == 16 && counter == 17) || (colDimension == 8 && rowDimension == 8 && counter == 15))
                break;
            
            }
        }
        if((colDimension == 30 && rowDimension == 16 && counter == 19) || (colDimension == 16 && rowDimension == 16 && counter == 17) || (colDimension == 8 && rowDimension == 8 && counter == 17))
            break;
        
    }
    num_frontier = counter;
    
    //Now set the mine count for all frontier indexes to 0
    for (int i = 0; i < num_frontier; ++i){
        valid_world_mine_map.insert(pair<int, int>(i, 0));
    }

    string emptystring = "";
    recur_backtrack(guess_map.size(), emptystring);

    //Now add all the ones with zero to the safe queue
    if(valid_world > 0){
        for (int i = 0; i < num_frontier; ++i){
            if(valid_world_mine_map[i] == 0){
                //Add to safe queue
                int x = convert_int_to_x(guess_map[i]);
                int y = convert_int_to_y(guess_map[i]);

                safe_queue.push(MyAI::coordinate(x, y));
            }
            else if (valid_world_mine_map[i] == valid_world){
                //Add to mine vector
                int x = convert_int_to_x(guess_map[i]);
                int y = convert_int_to_y(guess_map[i]);

                add_to_mine_vector_no_dupe(MyAI::coordinate(x, y));
            }
        }
    }
    clean_safe_queue();
    if(!safe_queue.empty())
    {
        agentX = safe_queue.front().x;
        agentY = safe_queue.front().y;
        
        safe_queue.pop();

        if (saved_board[agentY][agentX] >= 0){cout << "DUPLICATE LOCATED!!!! -3" << endl;}

        return {UNCOVER, agentX, agentY};
    }

    if(mine_vector.size() > 0){
        agentX = mine_vector.back().x;
        agentY = mine_vector.back().y;

        mine_vector.pop_back();
        return {FLAG, agentX, agentY};
    }

    //Else, take the best guess from the models seen.
    int current_best_index = -1;
    float lowest_mine_prob = 1.1;

    if(valid_world > 0){
        
        for (int i = 0; i < num_frontier; ++i){
            float local_mine_prob = valid_world_mine_map[i] / ((float) valid_world);
            if (local_mine_prob < lowest_mine_prob){
                lowest_mine_prob = local_mine_prob;
                current_best_index = i;
            }
        }


    }

    //Now choose the best found guess
    if(current_best_index > -1){
        int x = convert_int_to_x(guess_map[current_best_index]);
        int y = convert_int_to_y(guess_map[current_best_index]);

        agentX = x;
        agentY = y;

        if (saved_board[agentY][agentX] >= 0){cout << "DUPLICATE LOCATED!!!! -4" << endl;}
        
        return {UNCOVER, agentX, agentY};

    }
    
    
    // ==============================DANGEROUS CODE==============================
    if(totalMines == 1 && number == 1){
        //Find somewhere to make guess
        //We know there is one more mine left and it is around agentX and agentY
        for(int i = 0; i < rowDimension; ++i){
            for(int j = 0; j < colDimension; ++j){
                if(saved_board[i][j] == -1 && abs(j - agentX) > 1 && abs(i - agentY) > 1){
                    agentX = j;
                    agentY = i;
                
                    return {UNCOVER, agentX, agentY};
                }

            }
        }
    }
    //REMOVE THE ABOVE CODE AFTER STAGE 1 SUBMISSION
    //it NEEDS to be replaced with a better guessing function

    return {LEAVE,-1,-1};
}

void MyAI::clean_safe_queue(){
    vector<int> cleanvec;
    while(!safe_queue.empty()){
        int tempx = safe_queue.front().x;
        int tempy = safe_queue.front().y;

        if (saved_board[tempy][tempx] < 0){
            cleanvec.push_back(convert_coor_to_int(tempx, tempy));
        }
        safe_queue.pop();
    }
    for(int i=0; i < cleanvec.size(); i++){
        safe_queue.push(MyAI::coordinate(convert_int_to_x(cleanvec.at(i)), convert_int_to_y(cleanvec.at(i))));    
    }
    

}

void MyAI::add_to_mine_vector_no_dupe(coordinate c){
    if (count(mine_vector.begin(), mine_vector.end(), c) == 0) {
        mine_vector.push_back(c);
    }
}

void MyAI::print_mine_vector(){
    for(int i=0; i < mine_vector.size(); i++){
        cout << "[X: " << mine_vector.at(i).x << ", Y: " << mine_vector.at(i).y << "] ";
    }
    cout << "" << endl;

}

void MyAI::PrintBoard(){
    for(int i = rowDimension - 1 ; i >= 0; --i){
        for(int j = 0; j < colDimension; ++j){
            if(saved_board[i][j] > -1 && saved_board[i][j] < 10){
                cout << " " << saved_board[i][j] << " ";
            }else{
                cout << " " << saved_board[i][j];
            }
        }
        cout << "" << endl;
    }
    cout << "\n\n" << endl;
}


void MyAI::unmark_all_neighbors()
{
    // check if (x,y) return 0
    if(saved_board[agentY][agentX] == 0)
    {
        // if true, then we can uncover all neighbors of (x,y)
        
        // right top
        if(agentX + 1 < colDimension && agentY + 1 < rowDimension)
        {
            if(saved_board[agentY + 1][agentX + 1] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX + 1, agentY + 1));
                saved_board[agentY + 1][agentX + 1] = -2;
            }
        }
        
        // top
        if(agentX < colDimension && agentY + 1 < rowDimension)
        {
            if(saved_board[agentY + 1][agentX] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX, agentY + 1));
                saved_board[agentY + 1][agentX] = -2;
            }
        }
        
        // left top
        if(agentX - 1 >= 0 && agentY + 1 < rowDimension)
        {
            if(saved_board[agentY + 1][agentX - 1] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX - 1, agentY + 1));
                saved_board[agentY + 1][agentX - 1] = -2;
            }

        }
        
        // right
        if(agentX + 1 < colDimension && agentY < rowDimension)
        {
            if(saved_board[agentY][agentX + 1] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX + 1, agentY));
                saved_board[agentY][agentX + 1] = -2;
            }

        }
        
        // left
        if(agentX - 1 >= 0 && agentY < rowDimension)
        {
            if(saved_board[agentY][agentX - 1] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX - 1, agentY));
                saved_board[agentY][agentX - 1] = -2;
            }
        }
        
        // bottom right
        if(agentX + 1 < colDimension && agentY - 1 >= 0)
        {
            if(saved_board[agentY - 1][agentX + 1] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX + 1, agentY - 1));
                saved_board[agentY - 1][agentX + 1] = -2;
            }
        }
        
        // bottom
        if(agentX < colDimension && agentY - 1 >= 0)
        {
            if(saved_board[agentY - 1][agentX] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX, agentY - 1));
                saved_board[agentY - 1][agentX] = -2;
            }

        }
        
        // bottom left
        if(agentX - 1 >= 0 && agentY - 1 >= 0)
        {
            if(saved_board[agentY - 1][agentX - 1] == -1)
            {
                safe_queue.push(MyAI::coordinate(agentX - 1, agentY - 1));
                saved_board[agentY - 1][agentX - 1] = -2;
            }
        }
    }
}


void MyAI::check_unknown_neighbors()
{
    for(int i = 0; i < rowDimension; ++i)
    {
        for(int j = 0; j < colDimension; ++j)
        {
            int num_marked_neig = 0;
            int mine = 0;
            
            if(saved_board[i][j] > 0 && saved_board[i][j] != 10)
            {
                int label = saved_board[i][j];               
                
                if(j + 1 < colDimension && i + 1 < rowDimension)
                {
                    if(saved_board[i + 1][j + 1] == -1)
                    {
                        num_marked_neig++;
                    }
                    else if(saved_board[i + 1][j + 1] == 10)
                    {
                        mine++;
                    }
                }
                
                if(j < colDimension && i + 1 < rowDimension)
                {
                    if(saved_board[i + 1][j] == -1)
                    {
                        num_marked_neig++;
                    }
                    else if(saved_board[i + 1][j] == 10)
                    {
                        mine++;
                    }
                }
                
                if(j - 1 >= 0 && i + 1 < rowDimension)
                {
                    if(saved_board[i + 1][j - 1] == -1)
                    {
                        num_marked_neig++;
                    }
                        
                    else if(saved_board[i + 1][j - 1] == 10)
                    {
                        mine++;
                    }
                }
                if(j + 1 < colDimension && i < rowDimension)
                {
                    if(saved_board[i][j + 1] == -1)
                    {
                        num_marked_neig++;
                    }
                        
                    else if(saved_board[i][j + 1] == 10)
                    {
                        mine++;
                    }
                }
                
                if(j - 1 >= 0 && i < rowDimension)
                {
                    if(saved_board[i][j - 1] == -1)
                    {
                        num_marked_neig++;
                    }
                        
                    else if(saved_board[i][j - 1] == 10)
                    {
                        mine++;
                    }
                }
                
                if(j + 1 < colDimension && i - 1 >= 0)
                {
                    if(saved_board[i - 1][j + 1] == -1)
                    {
                        num_marked_neig++;
                    }
                        
                    else if(saved_board[i - 1][j + 1] == 10)
                    {
                        mine++;
                    }
                }
                
                if(j < colDimension && i - 1 >= 0)
                {
                    if(saved_board[i - 1][j] == -1)
                    {
                        num_marked_neig++;
                    }
                        
                    else if(saved_board[i - 1][j] == 10)
                    {
                        mine++;
                    }
                }
                
                if(j - 1 >= 0 && i - 1 >= 0)
                {
                    if(saved_board[i - 1][j - 1] == -1)
                    {
                        num_marked_neig++;
                    }
                    else if(saved_board[i - 1][j - 1] == 10)
                    {
                        mine++;
                    }
                }

            if((saved_board[i][j]) == mine){
                found_all_mines(i, j, 1);
            }
            else if((saved_board[i][j] - mine) ==  num_marked_neig || ((saved_board[i][j]) == num_marked_neig) && mine == 0){
                found_all_mines(i, j, 0);
            }
            }
        }
        
    }
}


void MyAI::found_all_mines(int i, int j, bool uncover)
//uncover = 1, add all -1 to safe queue
//uncover = 0, add all -1 to mines
//When this function is called, all mines around y, x have been found and all uncovered tiles are to be added to the the discovery queue
//Although it will work if there are no uncovered tiles, it is best for time to only call when there is an uncovered neighbor
{
    // if true, then we can uncover all neighbors of (x,y)
    //I is Y, J is X
    // right top
    if(j + 1 < colDimension && i + 1 < rowDimension)
    {
        if(saved_board[i + 1][j + 1] == -1)
        {
            if (uncover){
                safe_queue.push(MyAI::coordinate(j + 1, i + 1));
                saved_board[i + 1][j + 1] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j + 1, i + 1));
            }
        }
    }
    
    // top
    if(j < colDimension && i + 1 < rowDimension)
    {
        if(saved_board[i + 1][j] == -1)
        {
            if (uncover){
                safe_queue.push(MyAI::coordinate(j, i + 1));
                saved_board[i + 1][j] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j, i + 1));
            }
        }
    }
    
    // left top
    if(j - 1 >= 0 && i + 1 < rowDimension)
    {
        if(saved_board[i + 1][j - 1] == -1)
        {
            if(uncover){
                safe_queue.push(MyAI::coordinate(j - 1, i + 1));
                saved_board[i + 1][j - 1] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j - 1, i + 1));
            }
        }

    }
    
    // right
    if(j + 1 < colDimension && i < rowDimension)
    {
        if(saved_board[i][j + 1] == -1)
        {
            if (uncover){
                safe_queue.push(MyAI::coordinate(j + 1, i));
                saved_board[i][j + 1] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j + 1, i));
                
           }
        }

    }
    
    // left
    if(j - 1 >= 0 && i < rowDimension)
    {
        if(saved_board[i][j - 1] == -1)
        {
            if (uncover){
                safe_queue.push(MyAI::coordinate(j - 1, i));
                saved_board[i][j - 1] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j - 1, i));
            }
        }
    }
    
    // bottom right
    if(j + 1 < colDimension && i - 1 >= 0)
    {
        if(saved_board[i - 1][j + 1] == -1)
        {
            if (uncover){
                safe_queue.push(MyAI::coordinate(j + 1, i - 1));
                saved_board[i - 1][j + 1] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j + 1, i - 1));
            }
        }
    }
    
    // bottom
    if(j < colDimension && i - 1 >= 0)
    {
        if(saved_board[i - 1][j] == -1)
        {
            if(uncover){
                safe_queue.push(MyAI::coordinate(j, i - 1));
                saved_board[i - 1][j] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j, i - 1));
            }
        }

    }
    
    // bottom left
    if(j - 1 >= 0 && i - 1 >= 0)
    {
        if(saved_board[i - 1][j - 1] == -1)
        {
            if(uncover){
                safe_queue.push(MyAI::coordinate(j - 1, i - 1));
                saved_board[i - 1][j - 1] = -2;
            }else{
                add_to_mine_vector_no_dupe(MyAI::coordinate(j - 1, i - 1));
            }
        }
    }
}

bool MyAI::contact_frontier(const int& j, const int& i)
{
    // right top
    if(j + 1 < colDimension && i + 1 < rowDimension)
    {
        if(saved_board[i + 1][j + 1] >= 0 && saved_board[i + 1][j + 1] < 10)
            return true;
    }

    // top
    if(j < colDimension && i + 1 < rowDimension)
    {
        if(saved_board[i + 1][j] >= 0 && saved_board[i + 1][j] < 10)
            return true;
    }
    
    // left top
    if(j - 1 >= 0 && i + 1 < rowDimension)
    {
        if(saved_board[i + 1][j - 1] >= 0 && saved_board[i + 1][j - 1] < 10)
            return true;
    }
    
    // right
    if(j + 1 < colDimension && i < rowDimension)
    {
        if(saved_board[i][j + 1] >= 0 && saved_board[i][j + 1] < 10)
            return true;
    }
    
    // left
    if(j - 1 >= 0 && i < rowDimension)
    {
        if(saved_board[i][j - 1] >= 0 && saved_board[i][j - 1] < 10)
            return true;
    }
    
    // bottom right
    if(j + 1 < colDimension && i - 1 >= 0)
    {
        if(saved_board[i - 1][j + 1] >= 0 && saved_board[i - 1][j + 1] < 10)
            return true;
    }
    
    // bottom
    if(j < colDimension && i - 1 >= 0)
    {
        if(saved_board[i - 1][j] >= 0 && saved_board[i - 1][j] < 10)
            return true;
    }
    
    // bottom left
    if(j - 1 >= 0 && i - 1 >= 0)
    {
        if(saved_board[i - 1][j - 1] >= 0 && saved_board[i - 1][j - 1]  < 10)
            return true;
    }

    return false;
}

void MyAI::print_guess_map(){
    for(auto it = guess_map.cbegin(); it != guess_map.cend(); ++it)
    {
        std::cout << it->first << " " << it->second << " " << endl;;
    }

}

void MyAI::print_valid_world_map(){
    for(auto it = valid_world_mine_map.cbegin(); it != valid_world_mine_map.cend(); ++it)
    {
        std::cout << it->first << " " << it->second << " " << endl;;
    }

}


void MyAI::recur_backtrack(int bits_left, string curr_string){
    //Check for validity
    //Get result of curr_string + 0
    //Get return of curr_string + 1
    //print_guess_map();
    //cout << "Bits left: " << bits_left << endl;
    //cout << "with bitstr: " << curr_string << endl;
    bool res = valid_model(curr_string);
    //cout << "res = " << res << endl;
    if(bits_left == 0)
    {
        if (res){
        //The bitstring represents a valid world. Move it's configuration of mines into the map
        //cout << "valid world detected with following bitstring: " << bit_string << endl;
            for (int bit = 0; bit < curr_string.length(); ++bit){
                if (curr_string[bit] == '1'){
                    ++valid_world_mine_map[bit];
                }
            }
            valid_world++;
            return;
        }
    }
    if (!res)
        return;
    
    recur_backtrack(--bits_left, (curr_string + "0"));
    recur_backtrack(bits_left, (curr_string + "1"));

}


void MyAI::enumerate_all_combos(){
    //fills guess vector in with all possible combinations of mines
    //cout << "num mines: " << totalMines << endl;
    //cout << "front: " << num_frontier << endl;
    int mines_to_place = min(totalMines, num_frontier);
    
    for(int j = 1; j <= mines_to_place; ++j){

        string bit_string = "";
        int empty_squares = num_frontier - j;

        //cout << "in enum func:  going to enum with mines: " << mines_to_place << endl;
        for (int i = 0; i < empty_squares; ++i){
            bit_string.append("0");
        }
        
        for(int i = 0; i < j; ++i){
            bit_string.append("1");
        }


        //cout << "INIT BS: " << bit_string << endl;

        sort(bit_string.begin(), bit_string.end());
        do {
            //cout << bit_string << endl;;
            bool res = valid_model(bit_string);
            if (res){
                //The bitstring represents a valid world. Move it's configuration of mines into the map
                //cout << "valid world detected with following bitstring: " << bit_string << endl;
                ++valid_world;
                for (int bit = 0; bit < bit_string.length(); ++bit){
                    if (bit_string[bit] == '1'){
                        ++valid_world_mine_map[bit];
                    }
                }
                

            }

        } while(std::next_permutation(bit_string.begin(), bit_string.end()));

    }


}

bool MyAI::valid_model(const string& bit_string){

    //12 MEANS EXPLORATIONALLY NO MINE
    //Check the bitstring for validity

    //int** model_board = NULL; // Copy the board

    if (bit_string == "") {return true;}
    vector<int> neighbor_check;

    //Modify board to represent model:
    for(int bit = 0; bit < bit_string.length(); ++bit){
        int raw_coor = guess_map[bit];
        neighbor_check.push_back(raw_coor);
        if (bit_string[bit] == '1'){
            //Update board
            saved_board[convert_int_to_y(raw_coor)][convert_int_to_x(raw_coor)] = 10;
        }else{
            saved_board[convert_int_to_y(raw_coor)][convert_int_to_x(raw_coor)] = 12;
        }
    }

    //cout << "CHANGED BOARD: " << endl;
    //PrintBoard();

    //Check board for validity (for vector neighbor_check)
    bool return_val = check_changed_board(neighbor_check);

    //if(return_val)
        //cout << "FOUND POSSIBLE SOL\n";

    //Revert board
    for(int bit = 0; bit < bit_string.length(); ++bit){
        //if (bit_string[bit] == '1'){
            //Update board
            //cout << "HERE 1121" << endl;
        int raw_coor = guess_map[bit];
        saved_board[convert_int_to_y(raw_coor)][convert_int_to_x(raw_coor)] = -1;
        //}
    }
    //cout << "CHANGED BOARD BACK: " << endl;
    //PrintBoard();
    //Return value
    return return_val;


}


bool MyAI::check_changed_board(const vector<int>& neighbor_check)
{
    //cout << "length of neig.check" << neighbor_check.size() << endl;
    bool return_val = true;
    for(int i = 0 ; i < neighbor_check.size() ; ++i)
    {
        int x = convert_int_to_x(neighbor_check.at(i));
        int y = convert_int_to_y(neighbor_check.at(i));

        //cout << "checking changed board...\n";
        //cout << "value: " << neighbor_check.at(i) << endl;
        //cout << "x: " << x << endl << "y: " << y << endl;


        // right top
        if(x + 1 < colDimension && y + 1 < rowDimension)
        {
            if(saved_board[y + 1][x + 1] != 10 && saved_board[y + 1][x + 1] != -1)
            {
                if(!check_all_neigh_recur(x + 1, y + 1))
                    return false;
            }
        }

        // top
        if(x < colDimension && y + 1 < rowDimension)
        {
            if(saved_board[y + 1][x] != 10 && saved_board[y + 1][x] != -1)
            {
                if(!check_all_neigh_recur(x, y + 1))
                    return false;
            }
        }
        
        // left top
        if(x - 1 >= 0 && y + 1 < rowDimension)
        {
            if(saved_board[y + 1][x - 1] != 10 && saved_board[y + 1][x - 1] != -1)
            {
                if(!check_all_neigh_recur(x - 1, y + 1))
                    return false;
            }
        }
        
        // right
        if(x + 1 < colDimension && y < rowDimension)
        {
            if(saved_board[y][x + 1] != 10 && saved_board[y][x + 1] != -1)
            {
                if(!check_all_neigh_recur(x + 1, y))
                    return false;
            }
        }
        
        // left
        if(x - 1 >= 0 && y < rowDimension)
        {
            if(saved_board[y][x - 1] != 10 && saved_board[y][x - 1] != -1)
            {
                if(!check_all_neigh_recur(x - 1, y))
                    return false;
            }
        }
        
        // bottom right
        if(x + 1 < colDimension && y - 1 >= 0)
        {
            if(saved_board[y - 1][x + 1] != 10 && saved_board[y - 1][x + 1] != -1)
            {
                if(!check_all_neigh_recur(x + 1, y - 1))
                    return false;
            }
        }
        
        // bottom
        if(x < colDimension && y - 1 >= 0)
        {
            if(saved_board[y - 1][x] != 10 && saved_board[y - 1][x] != -1)
            {
                if(!check_all_neigh_recur(x, y - 1))
                    return false;
            }
        }
        
        // bottom left
        if(x - 1 >= 0 && y - 1 >= 0)
        {
            if(saved_board[y - 1][x - 1] != 10 && saved_board[y - 1][x - 1] != -1)
            {
                if(!check_all_neigh_recur(x - 1, y - 1))
                    return false;
            }
        }
    

        //cout << "RETURN TRUE FROM CHECK CHANGED BOARD\n";

        //return true;
        
    }

    return true;
}


bool MyAI::check_all_neigh(const int& x, const int& y)
{
    int mine_counter = 0;


    //cout << "NOW IN check_all_neigh\n";
    //cout << "X: " << x << endl << "Y: " << y << endl;
    //cout << "VALUE is " << saved_board[y][x] << endl;


    // right top
    if(x + 1 < colDimension && y + 1 < rowDimension)
    {
        if(saved_board[y + 1][x + 1] == 10)
            mine_counter++;
    }

    // top
    if(x < colDimension && y + 1 < rowDimension)
    {
        if(saved_board[y + 1][x] == 10)
            mine_counter++;
    }
        
    // left top
    if(x - 1 >= 0 && y + 1 < rowDimension)
    {
        if(saved_board[y + 1][x - 1] == 10)
            mine_counter++;
    }
        
    // right
    if(x + 1 < colDimension && y < rowDimension)
    {
        if(saved_board[y][x + 1] == 10)
            mine_counter++;
    }
        
    // left
    if(x - 1 >= 0 && y < rowDimension)
    {
        if(saved_board[y][x - 1] == 10)
            mine_counter++;
    }
        
    // bottom right
    if(x + 1 < colDimension && y - 1 >= 0)
    {
        if(saved_board[y - 1][x + 1] == 10)
            mine_counter++;
    }
        
    // bottom
    if(x < colDimension && y - 1 >= 0)
    {
        if(saved_board[y - 1][x] == 10)
            mine_counter++;
    }
        
    // bottom left
    if(x - 1 >= 0 && y - 1 >= 0)
    {
        if(saved_board[y - 1][x - 1] == 10)
            mine_counter++;
    }

    //cout << "finished checking neighbors... mine_counter = " << mine_counter << " and val = " << saved_board[y][x] << endl;

    if(mine_counter == saved_board[y][x]){
        //cout << " RETURN TRUE FROM CHECK NEIGHBORS " << endl;
        return true;
    }

    return false;
}

bool MyAI::check_all_neigh_recur(const int& x, const int& y)
{
    int mine_counter = 0;
    int covered_counter = 0;


    //cout << "NOW IN check_all_neigh\n";
    //cout << "X: " << x << endl << "Y: " << y << endl;
    //cout << "VALUE is " << saved_board[y][x] << endl;


    // right top
    if(x + 1 < colDimension && y + 1 < rowDimension)
    {
        if(saved_board[y + 1][x + 1] == 10)
            mine_counter++;
        else if(saved_board[y + 1][x + 1] == -1)
            covered_counter++;
    }

    // top
    if(x < colDimension && y + 1 < rowDimension)
    {
        if(saved_board[y + 1][x] == 10)
            mine_counter++;
        else if(saved_board[y + 1][x] == -1)
            covered_counter++;
    }
        
    // left top
    if(x - 1 >= 0 && y + 1 < rowDimension)
    {
        if(saved_board[y + 1][x - 1] == 10)
            mine_counter++;
        else if(saved_board[y + 1][x - 1] == -1)
            covered_counter++;
    }
        
    // right
    if(x + 1 < colDimension && y < rowDimension)
    {
        if(saved_board[y][x + 1] == 10)
            mine_counter++;
        else if(saved_board[y][x + 1] == -1)
            covered_counter++;
    }
        
    // left
    if(x - 1 >= 0 && y < rowDimension)
    {
        if(saved_board[y][x - 1] == 10)
            mine_counter++;
        else if(saved_board[y][x - 1] == -1)
            covered_counter++;
    }
        
    // bottom right
    if(x + 1 < colDimension && y - 1 >= 0)
    {
        if(saved_board[y - 1][x + 1] == 10)
            mine_counter++;
        else if(saved_board[y - 1][x + 1] == -1)
            covered_counter++;
    }
        
    // bottom
    if(x < colDimension && y - 1 >= 0)
    {
        if(saved_board[y - 1][x] == 10)
            mine_counter++;
        else if(saved_board[y - 1][x] == -1)
            covered_counter++;
    }
        
    // bottom left
    if(x - 1 >= 0 && y - 1 >= 0)
    {
        if(saved_board[y - 1][x - 1] == 10)
            mine_counter++;
        else if(saved_board[y - 1][x - 1] == -1)
            covered_counter++;
    }

    //cout << "finished checking neighbors... mine_counter = " << mine_counter << " and val = " << saved_board[y][x] << endl;
    //cout << "this is check all neight" << endl;
    //cout << "[x, y] = [" << x << ", " << y << "] - covcount: " << covered_counter << "minecnt: " << mine_counter << endl;
    if (covered_counter == 0){
            if(mine_counter == saved_board[y][x] || saved_board[y][x] == 12)
                return true;
            
            //cout << "fail1 at " << x << ", " << y << endl;
            return false;
        }
    else{
            if(mine_counter <= saved_board[y][x])
                return true;
            
            //cout << "fail2\n";
            return false;
        }

    return false;
    
}
