#include <iostream>
#include <vector>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::sort;

class SoccerPlayer
{
    long int efficiency_;
    int id_; // Order number in input

public:
    void SetId(int id)
    {
        id_ = id;
    }

    int GetId() const
    {
        return id_;
    }

    void SetEfficiency(long int efficiency)
    {
        efficiency_ = efficiency;
    }

    long int GetEfficiency() const
    {
        return efficiency_;
    }
};

class CompareByEfficiency
{
public:
    bool operator()(const SoccerPlayer &firstArg, const SoccerPlayer &secondArg) const
    {
        return firstArg.GetEfficiency() < secondArg.GetEfficiency();
    }
};

class CompareById
{
public:
    bool operator()(const SoccerPlayer &firstArg, const SoccerPlayer &secondArg) const
    {
        return firstArg.GetId() < secondArg.GetId();
    }
};

template <typename It, class T, class Compare>
void Merge(It beginFirst, 
           It endFirst,
           It beginSecond,
           It endSecond,
           Compare compare) 
{
    std::vector<T> result; 
    It outputIt = beginFirst;
    while (beginFirst != endFirst && beginSecond != endSecond)
	{
        if (compare(*beginFirst, *beginSecond))
        {
            result.push_back(*beginFirst);
            ++beginFirst;
        }
        else
        {
            result.push_back(*beginSecond);
            ++beginSecond;
        }
    }
    while (beginFirst != endFirst)
    {
        result.push_back(*beginFirst);
        ++beginFirst;
    }
    while (beginSecond != endSecond)
    {
        result.push_back(*beginSecond);
        ++beginSecond;
    }
    std::copy(result.begin(), result.end(), outputIt);
}

template<typename It, class T, class Compare>
void MergeSort(It begin, 
               It end,
               Compare compare)
 {
    int length = std::distance(begin, end);
    if (length <= 1)
        return;
    It middle = begin + length / 2;
    MergeSort<It, T, Compare>(begin, middle, compare);
    MergeSort<It, T, Compare>(middle, end, compare);
    Merge<It, T, Compare>(begin, middle, middle, end, compare);
    return;
} 

std::vector<SoccerPlayer> ReadSoccerPlayers()
{
    std::vector<SoccerPlayer> players;
    int playersCount;
    cin >> playersCount;
    for (int cnt = 0; cnt < playersCount; ++cnt)
    {
		long efficiency;
        cin >> efficiency;
		SoccerPlayer soccerPlayer;
        soccerPlayer.SetEfficiency(efficiency);
        soccerPlayer.SetId(players.size());
        players.push_back(soccerPlayer);
    }
    return players;
}

bool OnePlayerEfficiencyLessOrEqualThenSumOthers(SoccerPlayer onePlayer,
                                                 SoccerPlayer firstAnotherPlayer,
                                                 SoccerPlayer secondAnotherPlayer)
{
    return onePlayer.GetEfficiency() <=
           firstAnotherPlayer.GetEfficiency() +
           secondAnotherPlayer.GetEfficiency();
}

std::vector<SoccerPlayer> BuildSoccerTeam(std::vector<SoccerPlayer> players)
{
    std::vector<SoccerPlayer> team;
    CompareByEfficiency compareByEfficiency;
    MergeSort<std::vector<SoccerPlayer>::iterator, SoccerPlayer, CompareByEfficiency>
        (players.begin(), 
         players.end(),
         compareByEfficiency);
    int currentTeamBegin = 0;
    int possiblePlayersRight = 0;
    int possiblePlayersLeft = 0;
    int currentTeamEnd = 0;
    long possiblePlayersSum = players[0].GetEfficiency();
    long maxSumEffisiency = players[0].GetEfficiency();
    int playersCount = players.size();
    while (playersCount - 1 != possiblePlayersRight)
    {
        possiblePlayersRight++;
        possiblePlayersSum += players[possiblePlayersRight].GetEfficiency();
        if (possiblePlayersRight - currentTeamBegin < 2)
        {
            maxSumEffisiency += players[possiblePlayersRight].GetEfficiency();
            currentTeamEnd = possiblePlayersRight;
            possiblePlayersLeft = currentTeamBegin;
            continue;
        }
        while (players[possiblePlayersLeft].GetEfficiency() +
               players[possiblePlayersLeft + 1].GetEfficiency() <
               players[possiblePlayersRight].GetEfficiency())
        {
            possiblePlayersSum -= players[possiblePlayersLeft].GetEfficiency();
            possiblePlayersLeft++;
        }
        while (currentTeamEnd < possiblePlayersRight &&
               OnePlayerEfficiencyLessOrEqualThenSumOthers(players[currentTeamEnd + 1],
                                                           players[currentTeamBegin],
                                                           players[currentTeamBegin + 1]))
        {
            currentTeamEnd++;
            maxSumEffisiency += players[currentTeamEnd].GetEfficiency();
        }
        if (possiblePlayersSum > maxSumEffisiency)
        {
            maxSumEffisiency = possiblePlayersSum;
            currentTeamBegin = possiblePlayersLeft;
            currentTeamEnd = possiblePlayersRight;
        }
    }
    for (int countTeam = currentTeamBegin; countTeam <= currentTeamEnd; ++countTeam)
    {
        team.push_back(players[countTeam]);
    }
    return team;
}

long ComputeTeamEfficiency(std::vector<SoccerPlayer> team)
{
    long sumEfficiency = 0;
    int teamSize = team.size();
    for (int countTeam = 0; 
         countTeam < teamSize; ++countTeam )
    {
        sumEfficiency += team[countTeam].GetEfficiency();
    }
    return sumEfficiency;
}

// Prints effective team
void PrintSoccerTeam(std::vector<SoccerPlayer> team)
{
    CompareById compareById;
    MergeSort<std::vector<SoccerPlayer>::iterator, SoccerPlayer, CompareById>
        (team.begin(), team.end(), compareById);
    int teamSize = team.size();
    cout << ComputeTeamEfficiency(team) << endl;
    for (int countTeam = 0 ; countTeam < teamSize; ++countTeam)
    {
        cout << team[countTeam].GetId() + 1 << " ";
    }
    cout << endl;
}

int main()
{
    std::vector<SoccerPlayer> players = ReadSoccerPlayers();
    std::vector<SoccerPlayer> team = BuildSoccerTeam(players);
    PrintSoccerTeam(team);
    return 0;
}
