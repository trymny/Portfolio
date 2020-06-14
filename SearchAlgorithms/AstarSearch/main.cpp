#include <iostream>
#include <vector>
#include <queue>

using namespace std;

class Puzzle
{
private:
   struct node
   {
        int cost;
        // int board[3][3] =  {{4,1,0}, {6,3,2}, {7,5,8}};
        //int board[3][3] =  {{4,3,1}, {6,5,2}, {0,7,8}};
        //int board[3][3] =  {{3,1,0}, {4,5,2}, {6,7,8}};
        int board[3][3] =  {{0,1,2}, {3,4,5}, {6,7,8}};

        vector<node*> child;
        node* parent;
        node()
        {
            child.push_back(NULL);
            child.push_back(NULL);
            child.push_back(NULL);
            child.push_back(NULL);
            parent = NULL;
            cost = 0;
        }
   };
public:
class CompareCost
{
public:
    bool operator() (node* left, node* right)
    {
        return left->cost > right->cost;
    }
};

vector <Puzzle*> myStack;
vector <node*> Closed;
struct node *relation;

Puzzle()
{
    relation = new node();
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

bool check(struct node *myNode, int *index)
{
    bool equality = false;
    for(int g = 0; g < Closed.size(); g++)
    {
        if(compare(myNode->board,Closed[g]->board))
        {
            *index = g;
            equality = true;
        }
    }
    return equality;
}

bool check2(int a, int index)
{
    if(a < Closed[index]->cost)
    {
        return true;
    }
    return false;
}

int misplaced(struct node *myNode, int goal[3][3])
{
    int k = 0;
    for (int i=0; i < 3; i++)
    {
        for (int j=0; j < 3; j++)
        {
            if(myNode->board[i][j]!=goal[i][j])
            k++;
        }
    }
    return k;
}

int manhattan(struct node *myNode, int goal[3][3])
{
    int diffx, diffy, distance = 0;

	for(int row0= 0; row0 < 3; row0++)
	{
		for(int col0 = 0; col0 < 3; col0++)
		{
			for(int row1 = 0; row1 < 3; row1++)
			{
				for(int col1 = 0; col1 < 3; col1++)
				{
					if(myNode->board[row0][col0] == goal[row1][col1])
					{
						diffx = (row0 - row1 < 0) ? row1 - row0 : row0 - row1;
                        diffy = (col0 - col1 < 0) ? col1 - col0 : col0 - col1;
                        distance += diffx + diffy;
					}
				}
			}
		}
	}
	return distance;
}

void searchBoard(int goal[3][3])
{
    priority_queue<node*, vector<node*>, CompareCost> Open;
    struct node *myNode = relation;
    Puzzle* addNode = new Puzzle();

    myStack.push_back(addNode);
    Open.push(myNode);

    int index = 0;
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
        if(Open.empty())break;

        while(1)
        {
            myNode = Open.top();
            Open.pop();

            if(!check(myNode,&index))
            break;
            else if(check2(manhattan(myNode,goal),index))
            {
                visited--;
                Closed.erase(Closed.begin()+index);
                break;
            }
        };

        if(maxOpen < Open.size())
        {
            maxOpen = Open.size();
        }

        visited++;
        //Insert v into VISTED
        Closed.push_back(myNode);

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
                            Puzzle* addNode = new Puzzle();
                            //add the new node to a list
                            myStack.push_back(addNode);
                            //Make a link/relation between the parent and the new child.
                            myNode->child[n] = myStack.back()->relation;
                            //Make a link/relation between the child and the parent.
                            myNode->child[n]->parent=myNode;
                            //copy the excisting matrix over to the new child.
                            copyBoard(myNode->child[n]->board,myNode->board);
                            //move the empty space and update the position
                            swap(myNode->child[n]->board[i][j], myNode->child[n]->board[i+dir[k].dx][j+dir[k].dy]);
                            //compute and assign the new cost
                            myNode->child[n]->cost = myNode->cost+manhattan(myNode->child[n],goal);
                            //restricting the possibility of moving back and forth
                            if(myNode->parent != NULL)
                            {
                                if(compare(myNode->parent->board,myNode->child[n]->board))
                                continue;
                            }
                            //Add the new child to the open list.
                            Open.push(myNode->child[n]);
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
    Puzzle *node = new Puzzle();
    int goal[3][3] = {{1,2,3}, {4,5,6}, {7,8,0}} ;
    node->searchBoard(goal);
    return 0;
}
