#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <stdexcept>

using std::vector;
using std::set;
using std::map;
using std::string;
using std::queue;
using std::cin;
using std::cout;
using std::endl;
using std::invalid_argument;

const char WILDCARD = '?';
const char FIRST_LETTER = 'a';
const char LAST_LETTER = 'z';

struct Substring
{
  string patternPart;
  int positionInPattern;
};

template <class AuxiliaryData>
class TrieNode 
{
  set<int> labels;
  map<char, TrieNode<AuxiliaryData> > edges;
  AuxiliaryData auxiliaryData;

public:
  explicit TrieNode() {}

  const AuxiliaryData & GetAuxiliaryData() const
  {
    return this->auxiliaryData;
  }

  AuxiliaryData & GetAuxiliaryData()
  {
      return this->auxiliaryData;
  }

  TrieNode<AuxiliaryData> & GetNodeByLetter(char letter)
  {
    if (edges.end() != edges.find(letter))
    {
      return edges.find(letter)->second;
    }
    string errorMessage = "wrong letter ";
    errorMessage += letter;
    throw invalid_argument(errorMessage);
  }

  void SetNodeForLetter(char letter, const TrieNode<AuxiliaryData> & node)
  {
    edges[letter] = node;
  }

  const set<int>& GetLabels() const
  {
    return labels;
  }

  void AddLabel(int label) 
  {
    labels.insert(label);
  }

  void AddLabels(const set<int> & labels) 
  {
    this->labels.insert(labels.begin(), labels.end());
  }

  bool HasEdge(char letter) const
  {
    return edges.count(letter) > 0;
  }

  const TrieNode<AuxiliaryData>& GetChildNode(char letter) const
  {
    return edges.find(letter)->second;
  }

  TrieNode<AuxiliaryData>& GetChildNode(char letter)
  {
    return edges[letter];
  }
};


template <class AuxiliaryData>
class Trie 
{
private:
  TrieNode<AuxiliaryData>* root;
  vector<int> patternLengths;

  void AddSubstring(TrieNode<AuxiliaryData>& node,
    const string & string, 
    int startPosition, 
    int label) 
  {
    if (string.size() > startPosition) 
    {
      char letter = string[startPosition];
      AddSubstring(node.GetChildNode(letter), string, startPosition + 1, label);
    } 
    else 
    {
      node.AddLabel(label);
    }
  }
public:
  TrieNode<AuxiliaryData>* GetRoot()
  {
    return this->root;
  }

  const TrieNode<AuxiliaryData>* GetRoot() const 
  {
    return this->root;
  }

  TrieNode<AuxiliaryData>* GetNextNode(TrieNode<AuxiliaryData>* node, const char letter) const 
  {
    if (node->HasEdge(letter)) 
    {
      return &node->GetChildNode(letter);
    } 
    else
    { 
      return NULL;
    } 
  }

  void AddString(const string & str) 
  {
    int newLabel = patternLengths.size();
    AddSubstring(*root, str, 0, newLabel);
    patternLengths.push_back(str.length());
  }

  void Init(const vector<string> & strings) 
  {
    root = new TrieNode<AuxiliaryData>();    
    for (vector<string>::const_iterator it = strings.begin(); 
      it < strings.end(); ++it) 
    {
      AddString(*it);
    }
  }

  ~Trie() 
  {
    delete(root);
  }
};

class AhoCorasickNodesData {
private:
  TrieNode<AhoCorasickNodesData> * suffixLink;
  TrieNode<AhoCorasickNodesData> * terminalLink;
public:
  TrieNode<AhoCorasickNodesData> * GetSuffixLink()
  {
    return this->suffixLink;
  }
  AhoCorasickNodesData() {}
  AhoCorasickNodesData(TrieNode<AhoCorasickNodesData> * suffixLink,
      TrieNode<AhoCorasickNodesData> * terminalLink) :
  suffixLink(suffixLink), terminalLink(terminalLink)
  {
  }
};

typedef TrieNode<AhoCorasickNodesData> AutomatNode;

class Automat
{
private:
  Trie<AhoCorasickNodesData> * root;
  AutomatNode* initialState;
  AutomatNode * currentState;
  vector<int> patternLengths;

public:TrieNode<AhoCorasickNodesData>* 
           GetNextNode(TrieNode<AhoCorasickNodesData>* node, const char letter) const 
  {
    if (node->HasEdge(letter)) 
    {
      return &node->GetChildNode(letter);
    } 
    else if (node == this->root->GetRoot()) 
    { 
      return this->root->GetRoot();
    } 
    else 
    {
      return NULL;
    }
  }

  void AddSubstring(TrieNode<AhoCorasickNodesData>& node,
    const string & string, 
    int startPosition, 
    int label) 
  {
    if (string.size() > startPosition) 
    {
      char letter = string[startPosition];
      AddSubstring(node.GetChildNode(letter), string, startPosition + 1, label);
    } 
    else 
    {
      node.AddLabel(label);
    }
  }

  void AddString(const string & str) 
  {
    int newLabel = patternLengths.size();
    AddSubstring(*this->root->GetRoot(), str, 0, newLabel);
    patternLengths.push_back(str.length());
  }

  void BuildSuffixForest() 
  {
    this->currentState = new AutomatNode();
    this->currentState = this->root->GetRoot();
    this->root->GetRoot()->GetAuxiliaryData() = *(new AhoCorasickNodesData(this->root->GetRoot(), 
      this->root->GetRoot()));
    queue< TrieNode<AhoCorasickNodesData>* > nodes;

    for (char letter = FIRST_LETTER; letter <= LAST_LETTER; ++letter)
    {
      if (this->root->GetRoot()->HasEdge(letter))
      {
          this->root->GetRoot()->GetNodeByLetter(letter).GetAuxiliaryData() = 
          (*(new AhoCorasickNodesData(this->root->GetRoot(), 
          this->root->GetRoot())));
        nodes.push(&(this->root->GetRoot()->GetNodeByLetter(letter)));
      }
    }

    while (!nodes.empty()) 
    {
      TrieNode<AhoCorasickNodesData>* frontNode = nodes.front();
      nodes.pop();
      for (char letter = FIRST_LETTER; letter < LAST_LETTER; ++letter)
      {
        if (true == frontNode->HasEdge(letter))
        {
          TrieNode<AhoCorasickNodesData>* childNode = &(frontNode->GetNodeByLetter(letter));
          TrieNode<AhoCorasickNodesData>* transitionNode = 
            frontNode->GetAuxiliaryData().GetSuffixLink();
          nodes.push(childNode);
          while (GetNextNode(transitionNode, letter) == NULL) 
          {
            transitionNode = transitionNode->GetAuxiliaryData().GetSuffixLink();
          }
          childNode->GetAuxiliaryData() = (*(new AhoCorasickNodesData(GetNextNode(transitionNode, 
            letter), 
            this->root->GetRoot())));
          childNode->AddLabels(childNode->GetAuxiliaryData().GetSuffixLink()->GetLabels());
        }
      }
    }
  }

public:
  void Init(const vector<string> & strings)
  {
    this->initialState = new AutomatNode;
    this->root = new Trie<AhoCorasickNodesData>();  
    this->root->Init(strings);
    this->initialState = this->root->GetRoot();
    for (vector<string>::const_iterator it = strings.begin(); 
      it != strings.end(); ++it) 
    {
      AddString(*it);
    }

    BuildSuffixForest();
  }

  void MakeTransitionAuto(char letter)
  {
    while (GetNextNode(this->currentState, letter) == NULL) 
    {
      this->currentState = 
        this->currentState->GetAuxiliaryData().GetSuffixLink();
    }
    this->currentState = GetNextNode(this->currentState, letter);
  }

  vector< vector<int> > FindSubstringsPositionsAuto(const string & text)
  {
    vector< vector<int> > patternPartsPositions;
    for (int counter = 0; counter < patternLengths.size(); ++counter)
    {
      patternPartsPositions.push_back(vector<int>());
    }
    for (int letterPosition = 0; letterPosition < text.size(); ++letterPosition)
    {
      char letter = text[letterPosition];
      MakeTransitionAuto(letter);
      for (set<int>::const_iterator it = currentState->GetLabels().begin();
        it != currentState->GetLabels().end(); ++it) 
      {
        patternPartsPositions[*it].push_back(letterPosition - patternLengths[*it] + 1);
      }
    }
    return patternPartsPositions;
  }
};

void Split(const std::string& string, vector<Substring> & substrings, char delimiter)
{
  string::size_type previousPosition = string.find_first_not_of(delimiter, 0);
  string::size_type nextPosition = string.find_first_of(delimiter, previousPosition);
  while (string::npos != nextPosition || string::npos != previousPosition) 
  {
    Substring substring;
    substring.patternPart = string.substr(previousPosition, nextPosition - previousPosition);
    substring.positionInPattern = previousPosition;
    substrings.push_back(substring);
    previousPosition = string.find_first_not_of(delimiter, nextPosition);
    nextPosition = string.find_first_of(delimiter, previousPosition);
  }
}

vector<int> FindOccurences(const string& text, const string& pattern) 
{
  vector<Substring> substrings;
  Split(pattern, substrings, WILDCARD);
  vector<string> patternParts;
  for (vector<Substring>::iterator substringIter = substrings.begin();
    substringIter != substrings.end(); ++substringIter)
  {
    patternParts.push_back(substringIter->patternPart);
  }
  Automat automat = Automat();
  automat.Init(patternParts);
  vector<int> positionsTableForAllPatterns(text.length());
  vector< vector<int> > patternPartsPositions = automat.FindSubstringsPositionsAuto(text);
  for (int patternPart = 0; patternPart < patternPartsPositions.size(); ++patternPart) 
  {
    int delta = substrings[patternPart].positionInPattern;
    const vector<int>& patternPositions = patternPartsPositions[patternPart];
    for (vector<int>::const_iterator letterPosition = patternPositions.begin();
      letterPosition < patternPositions.end(); ++letterPosition) 
    {
      if (*letterPosition - delta < 0) 
      {
        continue;
      }
      positionsTableForAllPatterns[*letterPosition - delta]++;
    }
  }
  vector<int> positions;
  for (int i = 0; i < positionsTableForAllPatterns.size(); ++i) 
  {
    if (positionsTableForAllPatterns[i] == substrings.size() && 
      i + pattern.size() <= text.size()) 
    {
      positions.push_back(i);
    }
  }
  return positions;
}

template<typename T>
void PrintVector(vector<T> vectorToPrint)
{
  cout << vectorToPrint.size() << endl;
  for (int counter = 0; counter < vectorToPrint.size(); ++counter)
  {
    cout << vectorToPrint[counter] << " ";
  }
  cout << endl;
}

int main() 
{
  string pattern;
  cin >> pattern;
  string text;
  cin >> text;
  vector<int> occurrences = FindOccurences(text, pattern);
  PrintVector(occurrences);
  return 0;
}
