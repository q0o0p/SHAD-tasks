#include<iostream>
#include<vector>
#include<algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::swap;

const int WHITE = 0;
const int GRAY = 1;
const int BLACK = 2;
const int INIT_COMPONENT_INDEX = -1;

typedef vector<vector<int> > vectorOfIntVectors;

enum GameResult
{
    FIRST_WIN = 1,
    SECOND_WIN = 2,
    DRAW = 3
};

struct Game
{
    GameResult gameResult;
    int winner;
    int looser;
};

class DoNothing
{
public:
    void operator()(int) const
    {
    }
};

class BuildingTimesOut
{
    vector<int> & timeOutOrdered;
public:
    explicit BuildingTimesOut(vector<int> & timeOutOrdered) :
      timeOutOrdered(timeOutOrdered)
      {
      }
      void operator()(int vertexIndex) const
      {
          timeOutOrdered.push_back(vertexIndex);
      }
};

class BuildingStronglyConnectedComponents
{
    vector<int> & componentIndex;
    int componentsCount;
public:
    explicit BuildingStronglyConnectedComponents(vector<int> & componentIndex):
    componentIndex(componentIndex), componentsCount(0)
    {
    }
    void operator()(int index) const
    {
        componentIndex[index] = componentsCount;
    }
    void IncrementComponentsCount()
    {
        ++componentsCount;
    }
};


template <class ActionBefore, class ActionAfter>
void DFSWithExtraActions(int vertexIndex, 
    vector<int> & color, 
    const vectorOfIntVectors & incidenceMatrix,
    ActionBefore actionBefore, 
    ActionAfter actionAfter)
{
    actionBefore(vertexIndex);
    color[vertexIndex] = GRAY;
    for (int i = 0; i < incidenceMatrix[vertexIndex].size(); ++i)
    {
        if (WHITE == color[incidenceMatrix[vertexIndex][i]])
        {
            DFSWithExtraActions(incidenceMatrix[vertexIndex][i], 
                                color, 
                                incidenceMatrix, 
                                actionBefore, 
                                actionAfter);
        }
    }
    color[vertexIndex] = BLACK;
    actionAfter(vertexIndex);
}

class StronglyConnectedComponentsInfo
{
private:
    vectorOfIntVectors stronglyConnectedComponents;
    vector<int> componentIndex;
public:
    explicit StronglyConnectedComponentsInfo(vector<int> componentIndex):
    componentIndex(componentIndex)
    {
    }
    const vectorOfIntVectors & GetStronglyConnectedComponents() const
    {
        return stronglyConnectedComponents;
    }
    void BuildStronglyConnectedComponents()
    {
        for (int componentIndexCount = 0; componentIndexCount < componentIndex.size(); 
            ++componentIndexCount)
        {
            if (stronglyConnectedComponents.size() <= componentIndex[componentIndexCount])
            {
                stronglyConnectedComponents.resize(componentIndex[componentIndexCount] + 1);
            }
            stronglyConnectedComponents[componentIndex[componentIndexCount]].
                push_back(componentIndexCount);
        }
    }
};

class StronglyConnectedComponentsReachabilityInfo
{
private:
    StronglyConnectedComponentsInfo stronglyConnectedComponentsInfo;
    vector<bool> reachable;
public:
    explicit StronglyConnectedComponentsReachabilityInfo(
        const StronglyConnectedComponentsInfo & stronglyConnectedComponentsInfo):
    stronglyConnectedComponentsInfo(stronglyConnectedComponentsInfo)
    {
    }
    void Init(int vertexCount)
    {
        reachable.resize(vertexCount, false);
    }
    void SetReachable(int index, bool value)
    {
        reachable[index] = value;
    }
    const StronglyConnectedComponentsInfo & GetStronglyConnectedComponentsInfo() const
    {
        return stronglyConnectedComponentsInfo;
    }
    const vector<bool> & GetReachable() const
    {
        return reachable;
    }
};

vectorOfIntVectors ReverseDirectedGraph(const vectorOfIntVectors & directedGraph)
{
    vectorOfIntVectors reversedDirectedGraph;
    vector<int> incidenceMatrixInitVector;
    reversedDirectedGraph.resize(directedGraph.size(), incidenceMatrixInitVector);
    for (int row = 0; row < directedGraph.size(); ++row)
    {
        for (int column = 0; column < directedGraph[row].size(); ++column)
        {
            reversedDirectedGraph[directedGraph[row][column]].push_back(row);
        }
    }
    return reversedDirectedGraph;
}

StronglyConnectedComponentsReachabilityInfo 
    ComputeStronglyConnectedComponentsReachability(const vectorOfIntVectors & incidenceMatrix)
{
    vectorOfIntVectors matrixTransponed = ReverseDirectedGraph(incidenceMatrix);
    vector<int> timeOutOrdered;
    vector<int> color(matrixTransponed.size(), 0); 
    DoNothing doNothing;
    for (int peopleCounter = 0; peopleCounter < matrixTransponed.size(); ++peopleCounter)
    {
        if (WHITE == color[peopleCounter])
        {
            BuildingTimesOut buildingTimesOut(timeOutOrdered);
            DFSWithExtraActions(peopleCounter, 
                color,
                incidenceMatrix,
                doNothing,
                buildingTimesOut);
        }
    }
    color.clear();
    color.resize(matrixTransponed.size(), 0);
    vector<int> componentIndex;
    componentIndex.resize(matrixTransponed.size(), INIT_COMPONENT_INDEX);
    BuildingStronglyConnectedComponents buildingStronglyConnectedComponents(
                componentIndex);
    for (int peopleCounter = matrixTransponed.size() - 1; peopleCounter >= 0; --peopleCounter)
    {
        if (WHITE == color[timeOutOrdered[peopleCounter]])
        {
            DoNothing doNothing;
            DFSWithExtraActions(timeOutOrdered[peopleCounter], 
                color,
                matrixTransponed,
                buildingStronglyConnectedComponents,
                doNothing);
            buildingStronglyConnectedComponents.IncrementComponentsCount();
        }
    }
    StronglyConnectedComponentsInfo stronglyConnectedComponentsInfo(componentIndex);
    stronglyConnectedComponentsInfo.BuildStronglyConnectedComponents();
    StronglyConnectedComponentsReachabilityInfo 
        stronglyConnectedComponentsReachabilityInfo(stronglyConnectedComponentsInfo);
    stronglyConnectedComponentsReachabilityInfo.Init(matrixTransponed.size());
    for (int peopleCounter = 0; peopleCounter < matrixTransponed.size(); ++peopleCounter)
    {
        for (int edgeCounter = 0; 
            edgeCounter < matrixTransponed[peopleCounter].size(); 
            ++edgeCounter)
        {
            if (componentIndex[peopleCounter] != 
                componentIndex[matrixTransponed[peopleCounter][edgeCounter]])
            {
                stronglyConnectedComponentsReachabilityInfo.
                    SetReachable(componentIndex[peopleCounter], true);
            }
        }
    }
    return stronglyConnectedComponentsReachabilityInfo;
}

vectorOfIntVectors BuildIncidenceMatrix(const vector<Game> & games, int peopleNumber)
{
    vectorOfIntVectors incidenceMatrix;
    vector<int> matrixInitVector;
    incidenceMatrix.resize(peopleNumber, matrixInitVector);
    for (int gameCounter = 0; gameCounter < games.size(); ++gameCounter)
    {
        if (DRAW != games[gameCounter].gameResult)
        {
            incidenceMatrix[games[gameCounter].winner].push_back(games[gameCounter].looser);
        }
    }
    return incidenceMatrix;
}

int GetTeamSize(const vector<Game> & games, int peopleNumber)
{
    vectorOfIntVectors stupidMatrix = BuildIncidenceMatrix(games, peopleNumber);
    StronglyConnectedComponentsReachabilityInfo stronglyConnectedComponentsReachabilityInfo = 
        ComputeStronglyConnectedComponentsReachability(stupidMatrix);
    int minComponentWithoutEntry = peopleNumber;
    for (int componentCounter = 0; 
        componentCounter < stronglyConnectedComponentsReachabilityInfo.
        GetStronglyConnectedComponentsInfo().
        GetStronglyConnectedComponents().size(); 
        ++componentCounter)
    {
        if ( (minComponentWithoutEntry > (stronglyConnectedComponentsReachabilityInfo.
            GetStronglyConnectedComponentsInfo().
            GetStronglyConnectedComponents()[componentCounter]).size()) && 
            (false == stronglyConnectedComponentsReachabilityInfo.
            GetReachable()[componentCounter]))
        {
            minComponentWithoutEntry = (stronglyConnectedComponentsReachabilityInfo.
                                       GetStronglyConnectedComponentsInfo().
                                       GetStronglyConnectedComponents()[componentCounter]).
                                       size();
        }
    }
    return peopleNumber - minComponentWithoutEntry + 1;
}

vector<Game> ReadGames(int gamesNumber)
{
    vector<Game> games;
    for (int gameCounter = 0; gameCounter < gamesNumber; ++gameCounter)
    {
        Game game;
        int winner, looser, gameResult;
        cin >> winner >> looser >> gameResult;
        game.gameResult = (GameResult)gameResult;
        if (SECOND_WIN == gameResult)
        {
            swap(winner, looser);
            game.gameResult = FIRST_WIN;
        }
        game.winner = winner - 1;
        game.looser = looser - 1;
        games.push_back(game);
    }
    return games;
}

int main()
{
    int peopleNumber, gamesNumber;
    cin >> peopleNumber >> gamesNumber;
    vector<Game> games = ReadGames(gamesNumber);
    cout << GetTeamSize(games, peopleNumber) << endl;
    return 0;
}
