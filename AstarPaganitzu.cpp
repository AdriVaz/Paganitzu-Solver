#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <chrono>

using namespace std;

struct point_t {
    int x, y;

    bool operator==(const point_t& point2) const
    {
        return x == point2.x && y == point2.y;
    }

    bool operator!=(const point_t& point2) const
    {
        return x != point2.x || y != point2.y;
    }

    const point_t nextPosition(int direction) const
    {
        point_t out;

        switch (direction)
        {
            case 0: //UP
                out.x = x - 1;
                out.y = y;
            break;
            case 1: //RIGHT
                out.x = x;
                out.y = y + 1;
            break;
            case 2: //DOWN
                out.x = x + 1;
                out.y = y;
            break;
            case 3: //LEFT
                out.x = x;
                out.y = y - 1;
        }

        return out;
    }

};

//For printing a point
ostream& operator<<(ostream& out, const point_t& point)
{
    out << "(" << point.x  << "," << point.y << ")";
    return out;
}

//Problem static information (map)
vector<vector<char>> map;
point_t door; //For easy and fast access

struct state_t {
    point_t player;
    vector<point_t> rocks;
    vector<point_t> keys;

    bool operator==(const state_t& state2) const
    {
        //Test players and number of keys
        bool sameState = player == state2.player && keys.size() == state2.keys.size();

        //Test if all keys are equal
        for(unsigned int i = 0; i < keys.size() && sameState; i++)
            if(keys[i] != state2.keys[i])
                sameState = false;

        //Test if all rocks are equal
        for(unsigned int i = 0; i < rocks.size() && sameState; i++)
            if(rocks[i] != state2.rocks[i])
                sameState = false;

        return sameState;
    }

    const bool isFinalState() const
    {
        return player == door && keys.size() == 0;
    }

    const vector<state_t> descendents() const
    {
        vector<state_t> out;

        //4 possible actions:
        //  0: move UP
        //  1: move RIGHT
        //  2: move DOWN
        //  3: move LEFT
        for(int i = 0; i < 4; i++)
        {
            bool willMove = true;
            point_t nextPoint = player.nextPosition(i);
            // cout << "Action: " << i << "\tnextPoint: " << nextPoint << "\t";

            unsigned int snakeCol = 0;
            //Snake would attack player
            // for(snakeCol = 0; snakeCol < map[nextPoint.x].size() && willMove; snakeCol++)
            //     if(map[nextPoint.x][snakeCol] == 'S')
            //         willMove = false;
            //
            //Adjust index incremented by 1
            // snakeCol--;

            //Snake would attack player
            while(snakeCol < map[nextPoint.x].size() && willMove)
            {    // bool operator<(const node_t& node2) const
    // {
    //     return f() < node2.f();
    // }
                snakeCol++;

                if(map[nextPoint.x][snakeCol] == 'S')
                    willMove = false;
            }

            //Wall would block attack
            for(int j = min(nextPoint.y, (int) snakeCol); j < max(nextPoint.y, (int) snakeCol) && !willMove; j++)
                if(map[nextPoint.x][j] == '%')
                    willMove = true;

            //Rock would block attack
            if(!willMove && find_if(rocks.begin(), rocks.end(), [&nextPoint, &snakeCol](const point_t& rock)
                {
                    // cout << "############### SNAKE VERIFICATION ###############" << endl;
                    // cout << "Rock: " << rock << endl;
                    // cout << "nextPoint: " << nextPoint << endl;
                    // cout << "SnakeCol: " << snakeCol << endl;

                    //Assuming there won't be more than one snake in a row, this condition is correct
                    return rock.x == nextPoint.x && (rock.y > min(nextPoint.y, (int) snakeCol) && rock.y < max(nextPoint.y, (int) snakeCol));
                }
            ) != rocks.end())
                willMove = true;

            //Key would block attack
            if(!willMove && find_if(keys.begin(), keys.end(), [&nextPoint, &snakeCol](const point_t& key)
                {
                    // cout << "############### SNAKE VERIFICATION ###############" << endl;
                    // cout << "Rock: " << rock << endl;
                    // cout << "nextPoint: " << nextPoint << endl;
                    // cout << "SnakeCol: " << snakeCol << endl;

                    //Assuming there won't be more than one snake in a row, this condition is correct
                    return key.x == nextPoint.x && (key.y > min(nextPoint.y, (int) snakeCol) && key.y < max(nextPoint.y, (int) snakeCol));
                }
            ) != keys.end())
                willMove = true;

            //Map square check
            if(willMove)
            {
                //There is a key to pick up
                if(find(keys.begin(), keys.end(), nextPoint) != keys.end()) //map[nextPoint.x][nextPoint.y] == 'K'
                {
                    // cout << "Move with cost 2 picking up a key" << endl;
                    state_t newState = *this; //Copy previous state
                    newState.player = nextPoint; //Move player
                    newState.keys.erase(find(newState.keys.begin(), newState.keys.end(), nextPoint)); //Pick up the key

                    out.push_back(newState); //Action is applied and produced state will be returned

                }
                //There is a rock to push
                else if(find(rocks.begin(), rocks.end(), nextPoint) != rocks.end()) //map[nextPoint.x][nextPoint.y] == 'O'
                {
                    //To check if rock can be moved, we look at rock's next square
                    point_t next2Point = nextPoint.nextPosition(i);

                    if(map[next2Point.x][next2Point.y] == ' ')
                    {
                        // cout << "Move with cost 4" << endl;
                        state_t newState = *this; //Copy previous state
                        newState.player = nextPoint; //Move player
                        *find(newState.rocks.begin(), newState.rocks.end(), nextPoint) = next2Point; //Mover rock

                        out.push_back(newState); //Action is applied and produced state will be returned
                    }
                    else
                    {
                        // cout << "Rock cannot be pushed" << endl;
                    }
                }
                //There is nothing in front (normal movement) OR there's the door and player has picked up all keys
                else if(map[nextPoint.x][nextPoint.y] == ' ' || (keys.size() == 0 && nextPoint == door))
                {
                    // cout << "Move with cost 2" << endl;
                    state_t newState = *this; //Copy previous state
                    newState.player = nextPoint; //Move player

                    out.push_back(newState); //Action is applied and produced state will be returned
                }
                else
                {
                    // cout << "Cannot move. Next square not transitable" << endl;
                }
            }
            else
            {
                // cout << "Cannot move. Snake would kill player" << endl;
            }

        }

        return out;
    }

    int heuristic(int heuristicID) const
    {
        if(heuristicID == 0)
            return abs(abs(door.x - player.x) + abs(door.y - player.y))*2; //Manhattan Distance * 2
        else
            return sqrt(pow(door.x - player.x, 2) + pow(door.y - player.y, 2))*2; //Euclidean Distance * 2
    }

};

//For printing a state
ostream& operator<<(ostream& out, const state_t& state)
{
    out << "Player: " << state.player << endl;

    out << "Rocks: ";
    for(unsigned int i = 0; i < state.rocks.size(); i++)
        out << state.rocks[i] << "  ";

    out << endl << "Keys: ";
    for(unsigned int i = 0; i < state.keys.size(); i++)
        out << state.keys[i] << "  ";

    return out;
}

struct node_t {
    int heuristic; //h(n)
    int totalCost; //g(n)

    node_t* parent; //For printing solution path

    state_t state;

    bool operator==(const node_t& node2) const
    {
        return heuristic == node2.heuristic && state == node2.state;
    }

    // bool operator<(const node_t& node2) const
    // {
    //     return f() < node2.f();
    // }

    //Default constructor
    node_t(){}

    node_t(const state_t& stateIn, int heuristicID)
    {
        state = stateIn;
        heuristic = state.heuristic(heuristicID);

        //Root node by default
        totalCost = 0;
        parent = NULL;
    }

    void setParent(node_t& parentIn)
    {
        //Now node is not root
        parent = &parentIn;

        //Add parent's cost and self-cost (2 or 4)
        bool cost4 = false;

        for(unsigned int i = 0; i < state.rocks.size() && !cost4; i++)
            if(state.rocks[i] != parent->state.rocks[i])
                cost4 = true;

        if(cost4)
            totalCost += parent->totalCost + 4;
        else
            totalCost += parent->totalCost + 2;
    }

    const int f() const
    {
        return heuristic + totalCost;
    }

};

// ostream& operator<<(ostream& out, const node_t& node)
// {
//     out << "Heuristic: " << node.heuristic << endl;
//     out << "Total cost: " << node.totalCost << endl;
//     out << "Parent: " << node.parent << endl;
//
//     return out;
// }

// void printList(const vector<node_t>& vec)
// {
//     for(unsigned int i = 0; i < vec.size(); i++)
//     {
//         vector<vector<char>> map2 = map;
//
//         for(unsigned int j = 0; j < map2.size(); j++)
//         {
//             for(unsigned int k = 0; k < map2[j].size(); k++)
//             {
//                 for(unsigned int l = 0; l < vec[i].state.keys.size(); l++)
//                     if(vec[i].state.keys[l].x == (int) j && vec[i].state.keys[l].y == (int) k)
//                         map2[j][k] = 'K';
//
//                 for(unsigned int l = 0; l < vec[i].state.rocks.size(); l++)
//                     if(vec[i].state.rocks[l].x == (int) j && vec[i].state.rocks[l].y == (int) k)
//                         map2[j][k] = 'O';
//             }
//         }
//
//         map2[vec[i].state.player.x][vec[i].state.player.y] = 'A';
//
//         for(unsigned int j = 0; j < map.size(); j++)
//         {
//             for(unsigned int k = 0; k < map[j].size(); k++)
//             {
//                 cout << map2[j][k];
//             }
//             if(j == 0)
//                 cout << "    Heuristic: " << vec[i].heuristic;
//             else if(j == 1)
//                 cout << "    Total cost: " << vec[i].totalCost;
//             cout << endl;
//         }
//     }
// }

int main(int argc, char const *argv[])
{
    // *************** PARAM CONTROL *************** //
    if(argc != 3 || (atoi(argv[2]) != 0 && atoi(argv[2]) != 1))
    {
        cerr << "Error: Wrong arguments" << endl;
        cerr << "Usage: AstarPaganitzu <map file> <heuristic index>" << endl;
        cerr << "\t<map file>  : file with an encoded labyrinth" << endl;
        cerr << "\t<heuristic id> : values: 0 o 1." << endl;
        cerr << "\t\theuristic 0: Manhattan Distance * 2" << endl;
        cerr << "\t\theuristica 1: Euclidean Distance * 2" << endl;
        return -1;
    }

    // *************** READ FILE, GENERATE MAP AND INITIAL STATE *************** //
    state_t initial_state;

    ifstream mapFile(argv[1]);
    char buff;
    int row = 0;
    int col = 0;

    //Create map's firt row
    map.resize(map.size() + 1);

    //Fill map and initial state
    do {
        buff = mapFile.get();

        point_t aux;
        aux.x = row;
        aux.y = col;

        switch (buff) {
            case 'A': //Add player to initial state
                initial_state.player = aux;
                map[row].push_back(' ');
            break;
            case 'O': //Add rock to initial state
                initial_state.rocks.push_back(aux);
                map[row].push_back(' ');
            break;
            case 'K': //Add key to initial state
                initial_state.keys.push_back(aux);
                map[row].push_back(' ');
            break;
            case 10: //End Of Line
                col = -1; // -1 because of the increment after this switch
                row++;
                //Add another row to map
                map.resize(map.size() + 1);
            break;
            case 'E': //Stores door point separately from map
                door = aux;
            //break; MISSING BUT NOT AN ERROR!!
            default: //Wall, Snake, Exit or blank
                if(buff != -1 && buff != 10)
                    map[row].push_back(buff);
        }

        col++;

    } while(buff > -1);

    mapFile.close();

    //Delete an extra row created in the last iteration
    map.pop_back();

    //*************** CONTROL INFORMATION *************** //
    cout << "############### MAP ###############" << endl << endl;

    for(unsigned int i = 0; i < map.size(); i++)
    {
        for(unsigned int j = 0; j < map[i].size(); j++)
        {
            cout << map[i][j];
        }
        cout << endl;
    }

    cout << endl << "Exit: " << door << endl;
    // cout << "RowNum: " << map.size() << endl;
    // cout << "ColNum: " << (map.size() == 0 ? 0 : map[0].size()) << endl;

    cout << endl << "############### INITIAL STATE ###############" << endl;
    cout << initial_state << endl;

    // *************** A* *************** //
    cout << endl << "############### SOLUTION ###############" << endl;

    //Create initial node from initial state
    node_t initial_node(initial_state, atoi(argv[2]));

    //Create open and closed lists. Add initial state to open list
    list<node_t> open{initial_node};
    list<node_t> closed;

    //Buffer for storing a node in an iteration
    node_t currentNode;

    bool success = false;
    int expandedNodes = 0;

    // cout << "ANTES DE NADA:    address: " << &currentNode << endl << endl;


    // state_t s1;
    // state_t s2;
    // state_t s3;
    //
    // node_t n1(s1, 1);
    //
    // node_t n2(s2, 1);
    // n2.setParent(n1);
    //
    // node_t n3(s3, 1);
    // n3.setParent(n2);
    //
    // cout << "N1:   address: " << &n1 << "   parent:   " << n1.parent << endl;
    // cout << "N2:   address: " << &n2 << "   parent:   " << n2.parent << endl;
    // cout << "N3:   address: " << &n3 << "   parent:   " << n3.parent << endl;

    //For accurate time measurement
    using clk = chrono::high_resolution_clock;

    auto initialTime = clk :: now();

    //A* iterations
    while(!success && !open.empty())
    {
        // panic++;
        // printList(open);
        // cout << endl;

        expandedNodes++;

        //Pick up first node
        currentNode = open.front();
        // open.pop_front();
        open.erase(open.begin());

        //Expand node
        if(currentNode.state.isFinalState())
            success = true;
        else
        {
            //Expando node. Generate sucessors
            vector<state_t> sucessors = currentNode.state.descendents();
            closed.push_back(currentNode);
            // closed.insert(find_if(closed.begin(), closed.end(), [&currentNode](const node_t& listNode){return listNode.f() >= currentNode.f();}), currentNode);

            //Process each sucessor
            for(unsigned int i = 0; i < sucessors.size(); i++)
            {
                node_t sucessorNode(sucessors[i], atoi(argv[2]));
                // cout << "closed.back().address: " << &closed.back() << endl;
                //Sets parent and also calculates heuristic and accumulated cost
                sucessorNode.setParent(closed.back());

                // cout << "SucessorNode.address: " << &sucessorNode << endl;
                // cout << "SucessorNode.parent: " << sucessorNode.parent << endl;

                if(find(closed.begin(), closed.end(), sucessorNode) == closed.end()) //Node is not in closed list
                // if(binary_search(closed.begin(), closed.end(), sucessorNode)) //Node is not in closed
                {
                    auto openNode = find(open.begin(), open.end(), sucessorNode);
                    if(openNode != open.end()) //Node is in open list
                    {
                        if(openNode->f() > sucessorNode.f())
                        {
                            //2. Node is in open and new node's f() is better than old node's f()
                            //Delete old node from open list and insert new node in order
                            open.erase(openNode);
                            open.insert(find_if(open.begin(), open.end(), [&sucessorNode](const node_t& listNode){return listNode.f() >= sucessorNode.f();}), sucessorNode);
                        }

                    }
                    else
                    {
                        //1. Insert ordered in open list
                        open.insert(find_if(open.begin(), open.end(), [&sucessorNode](const node_t& listNode){return listNode.f() >= sucessorNode.f();}), sucessorNode);
                    }
                }
                //3. If node is in closed list, do nothing
            }
        }

        // cout << "OPEN LIST LENGTH: " << open.size() << endl;
        // for(unsigned int list = 0; list < open.size(); list++)
        // {
        //     cout << open[list] << endl;
        // }

        // if(false)
        // {
        //    cout << "Iteracion: " << panic << " IsFinal: " << currentNode.state.isFinalState() << " Position: " << currentNode.state.player << " Coste: " << currentNode.totalCost << endl;;
        //
        //
        //     vector<vector<char>> map2 = map;
        //     vector<vector<char>> map3 = map;
        //
        //     for(unsigned int j = 0; j < map2.size(); j++)
        //     {
        //         for(unsigned int k = 0; k < map2[j].size(); k++)
        //         {
        //             for(unsigned int l = 0; l < currentNode.state.keys.size(); l++)
        //                 if(currentNode.state.keys[l].x == (int) j && currentNode.state.keys[l].y == (int) k)
        //                     map2[j][k] = 'K';
        //
        //             for(unsigned int l = 0; l < currentNode.state.rocks.size(); l++)
        //                 if(currentNode.state.rocks[l].x == (int) j && currentNode.state.rocks[l].y == (int) k)
        //                     map2[j][k] = 'O';
        //         }
        //     }
        //
        //     map2[currentNode.state.player.x][currentNode.state.player.y] = 'A';
        //
        //     if(currentNode.parent != NULL)
        //     {
        //         for(unsigned int j = 0; j < map3.size(); j++)
        //         {
        //             for(unsigned int k = 0; k < map3[j].size(); k++)
        //             {
        //                 for(unsigned int l = 0; l < currentNode.parent->state.keys.size() ; l++)
        //                     if(currentNode.parent->state.keys[l].x == (int) j && currentNode.parent->state.keys[l].y == (int) k)
        //                         map3[j][k] = 'K';
        //
        //                 for(unsigned int l = 0; l < currentNode.parent->state.rocks.size(); l++)
        //                 endl    if(currentNode.parent->state.rocks[l].x == (int) j && currentNode.parent->state.rocks[l].y == (int) k)
        //                         map3[j][k] = 'O';
        //             }
        //         }
        //
        //         map3[currentNode.parent->state.player.x][currentNode.parent->state.player.y] = 'A';
        //
        //     }
        //
        //
        //     // cout << "currentState: " << currentNode.state << endl;
        //     // if(currentNode.parent != NULL) cout << "currentState.parent: " << currentNode.parent->state << endl;
        //
        //     // cout << "currentState.parent: \t" << currentNode.parent << endl;
        //     // cout << "currentState: \t\t" << &currentNode << endl;
        //
        //
        //     for(unsigned int j = 0; j < map.size(); j++)
        //     {
        //         for(unsigned int k = 0; k < map[j].size(); k++)
        //         {
        //             cout << map3[j][k];
        //         }
        //         cout << "\t";
        //         for(unsigned int k = 0; k < map[j].size(); k++)
        //         {
        //             cout << map2[j][k];
        //         }
        //         cout << endl;
        //     }
        // }

    }

    auto finalTime = clk :: now();

    //Time consumed by A* execution
	auto time  = chrono::duration_cast<chrono::microseconds>(finalTime - initialTime);

    if(success)
    {
        cout << "Solution Found!" << endl << endl;

        vector<point_t> path;

        //Traverse nodes to read solution
        node_t* aux = &currentNode;

        while(aux != NULL)
        {
            path.push_back(aux->state.player);
            // cout << aux->state.player << ", ";
            aux = aux->parent;
        }

        //Generate out files
        string fileName(argv[1]);
		ofstream solutionFile(fileName + ".output");
		ofstream statsFile(fileName + ".statistics");

        //Fill the map with objects stored in initial state (keys, rocks and character)
        for(unsigned int i = 0; i < map.size(); i++)
        {
            for(unsigned int j = 0; j < map[i].size(); j++)
            {
                for(unsigned int l = 0; l < initial_state.keys.size(); l++)
                    if(initial_state.keys[l].x == (int) i && initial_state.keys[l].y == (int) j)
                        map[i][j] = 'K';

                for(unsigned int l = 0; l < initial_state.rocks.size(); l++)
                    if(initial_state.rocks[l].x == (int) i && initial_state.rocks[l].y == (int) j)
                        map[i][j] = 'O';
            }
        }

        map[initial_state.player.x][initial_state.player.y] = 'A';

        //Print the map to file
        for(unsigned int i = 0; i < map.size(); i++)
        {
            for(unsigned int j = 0; j < map[i].size(); j++)
            {
                solutionFile << map[i][j];
            }
            solutionFile << endl;
        }

        //*************** PRINT STATISTICS AND RESULTS ***************//

        //Time
        cout << "Time used: " << time.count() << " microseconds (" <<  time.count()/10e5 << ") seconds" << endl;
        statsFile << "Total time: " << time.count() << " microseconds (" << time.count()/10e5 << ") seconds" << endl;

        //Solution cost
        cout << "Total cost of solution: " << currentNode.totalCost << endl;
        statsFile << "Total cost of solution: " << currentNode.totalCost << endl;

        //Solution length
        cout << "Solution length: " << path.size() << endl;
        statsFile << "Solution length: " << path.size() << endl;

        //Expanded nodes
        cout << "Expanded nodes: " << expandedNodes << endl;
        statsFile << "Expanded nodes: " << expandedNodes << endl;

        //Path
        cout << endl << "Path: ";

        for(unsigned int i = path.size() - 1; i > 0; i--)
        {
            cout << path[i] << " → ";
            solutionFile << path[i] << " → ";
        }

        cout << path[0] << endl;
        solutionFile << path[0] << endl;


        statsFile.close();
        solutionFile.close();

    }
    else
    {
        cout << "Problem has no solution" << endl;
    }

    return 0;
}
