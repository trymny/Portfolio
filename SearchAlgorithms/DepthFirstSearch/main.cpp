#include <iostream>
#include <vector>

using namespace std;

class Puzzle
{
private:
   struct node
   {
        int id;
        //int board[3][3] =  {{4,1,0}, {6,3,2}, {7,5,8}};
        //int board[3][3] =  {{4,3,1}, {6,5,2}, {0,7,8}};
        //int board[3][3] =  {{3,1,0}, {4,5,2}, {6,7,8}};
        int board[3][3] =  {{0,1,2}, {3,4,5}, {6,7,8}};
        vector<node*> child;
        node* parent;

        node(int number)
        {
            child.push_back(NULL);
            child.push_back(NULL);
            child.push_back(NULL);
            child.push_back(NULL);
            id = number;
        }
   };
public:
vector <Puzzle*> myStack;
vector <node*> Stack;
vector <node*> Visited;
struct node *relation;

Puzzle(int n)
{
    relation = new node(n);
}

bool compare(int a[3][3],int b[3][3])
{
    for (int i=0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
        {
            if(a[i][j]!=b[i][j])
            return false;
        }
    }
    return true;
}

void copyBoard(int a[3][3],int b[3][3])
{
   for (int i=0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
        {
            a[i][j]=b[i][j];
        }
    }
}

bool check(struct node *myNode)
{
    bool equality = false;
    for(int g = 0; g < Visited.size(); g++)
    {
        if(compare(myNode->board,Visited[g]->board))
        {
            equality = true;
        }
    }
    return equality;
}

bool searchBoard(int goal[3][3])
{
    struct node *myNode = relation;
    Puzzle* addNode = new Puzzle(myNode->id);

    myStack.push_back(addNode);
    Stack.push_back(myNode);

    int visited = 0;
    int maxOpen = 0;

    struct
    {
        int dx;
        int dy;
    } dir[] = {{-1,0,},{0,-1},{0,1},{1,0}};

    //While initial matrix is not equal to the goal matrix
    while(!compare(myNode->board,goal))
    {
    int n = 0;

    if(Stack.empty())break;

    //pop the top of STACK
    do
    {
        myNode = Stack.back();
        Stack.pop_back();
    }
    while(check(myNode));

    if(maxOpen < Stack.size())
    {
        maxOpen = Stack.size();
    }

    visited++;
    //Insert v into VISTED
    Visited.push_back(myNode);

    //Go through each column
    for(int i = 0; i < 3; i++)
    {
         //Go through each row
        for(int j = 0; j < 3; j++)
        {
            if(myNode->board[i][j]==0)
            {
                //Find all neighbours
                for (int k = 0; k < 4; k++)
                {
                    //Limit the neighbour search space
                    if(i+dir[k].dx >= 0 && i+dir[k].dx < 3 && j+dir[k].dy >= 0 && j+dir[k].dy < 3 )
                    {
                        //create a new node
                        Puzzle* addNode = new Puzzle(myNode->board[i+dir[k].dx][j+dir[k].dy]);
                        //add the new node to a list
                        myStack.push_back(addNode);

                        //Make a link/relation between the old node and the new node.
                        myNode->child[n] = myStack.back()->relation;
                        //Make a link/relation between the child and the parent.
                        myNode->child[n]->parent=myNode;

                        //copy the excisting matrix over to the new child.
                        copyBoard(myNode->child[n]->board,myNode->board);

                        //move the empty space and update the position
                        swap(myNode->child[n]->board[i][j], myNode->child[n]->board[i+dir[k].dx][j+dir[k].dy]);

                        if(myNode->parent != NULL)
                        {
                            if(compare(myNode->parent->board,myNode->child[n]->board))
                            continue;
                        }

                        Stack.push_back(myNode->child[n]);
                        n++;
                    }
                }
            }
        }
    }

    };
    cout << "open:" << maxOpen << endl<<endl;
    cout << "visited:" << visited << endl<<endl;
    cout << "Found goal"<< endl;

    //Traverse the tree backwards to find the length of the solution
    vector<node*>goBack;
    int length = -1;
    while(myNode)
    {
        goBack.push_back(myNode);
        myNode = myNode->parent;
        length++;
    }
    cout << "Length:" << length << endl;
}
};
int main()
{
    Puzzle *node = new Puzzle(0);
    int goal[3][3] = {{1,2,3}, {4,5,6}, {7,8,0}} ;
    node->searchBoard(goal);
    return 0;
}
