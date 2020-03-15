#include <fstream>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <vector>

/* Pour décompresser le fichier obtenu avec le code généré par la fonction shorten
 * on doit passer par les étapes suivantes:
 * 1/ Lire le fichier avec une fonction a load (ou juste modifier cette fonction 
 * 		pour prendre le fichier a lire comme parametre)
 * 2/ On inverse (la valeur devient la clé - pas de problème de conversion car
 * 		au départ les valeur sont uniques) deux maps de types codes et minimizedCodes 
 * 		utilisés pour compresser le fichier
 * 3/ Passer chaque mot codé par un nouvelle fonction similaire a minimizeSingleCode
 * 		mais 'inverse' -> pour chaque char dans le mot essayer de le remplacer par sa paire
 * 		correspondante dans le minimizedCodes inverse. Refaire jusqu'au pas de changements dans le string resultant
 * 4/ A cette étape nous avons chaque mots représenter sous 0 et 1, donc il suffit de remplacer 
 * chaque mot-code par la valeur dans le codes inverse ou la clé est ce mot-code
 * 5/ Le fichier est décompressé!
 */


std::vector<std::string> load()
{
	//Temporary variable to store current read line
    std::string line;
    std::ifstream file;
    //Result containing each line from text
    std::vector<std::string> words;

    file.open("./1984.txt");
    if(file.is_open())
    {
        while(std::getline(file,line))
        {
            words.push_back(line);
        }
        file.close();
    }
    return words;
}

//type defined to count occurences(int) for each word(string)
typedef std::map<std::string, int> occs;

//function count returns how many times specific strings occured in words as occs
occs count(std::vector<std::string> const &words)
{
    occs occurences;
    for(auto &word : words){
        if(occurences.find(word) == occurences.end())
        {
			//if word does not exist in occurences add it with occurence equal to 1
            occurences.insert({word,1});
        }
        else{
            occurences.at(word)++;
        }
    }
    return occurences;
}

typedef std::map<std::string, std::string> codes;

// function prefix returns passed root pair of strings
// with passed pref string appended to the begining of second string
std::pair<std::string,std::string>
prefix(std::pair<std::string,std::string> const& root, std::string const& pref)
{
    return std::make_pair(root.first, pref + root.second);
}

// function creates new codes with left (x) codes prefixed with 0
// and right (y) prefixed with 1
codes merge(codes const &x, codes const &y)
{
    codes result;
    //prefix_string: prefix to add to the next 'branch'
    auto prefix_string = "0";
    for(const auto& elem : x)
    {
        result.insert(prefix(elem, prefix_string));
    }
    //change prefix for right 'branch'
    prefix_string = "1";
    for(const auto& elem : y)
    {
        result.insert(prefix(elem, prefix_string));
    }
    return result;
}

// partial_codes represents one 'branch' or even a 'leaf' used to create
// complete codes
typedef std::multimap<int, codes> partial_codes;

//extracts a pair(occurence&codes) with lowest occurence count
auto extract(partial_codes& pc)
{
  auto result =  std::make_pair(pc.begin()->first,pc.begin()->second);
  pc.erase(pc.begin());
  return result;
}

void reduce(partial_codes &res)
{
    /* Useless because it's already checked in create function in while loop
    if(res.size() < 2){
        return;
    }
    */
    auto elem1 = extract(res);
    auto elem2 = extract(res);
    res.insert(
                {
                    elem1.first + elem2.first,
                    merge(elem1.second, elem2.second)
                }
              );
}

codes create(occs const &occ)
{
    partial_codes pCodes;
    for(const auto& elem : occ)
    {
        auto occurencesNumber = elem.second;
        //Had some problems with inserting to partial_codes and below is the
        //easiest method I found
        codes newCodes;
        newCodes.insert(std::make_pair(elem.first, ""));
        pCodes.insert(std::make_pair(occurencesNumber,newCodes));
    }
    while(pCodes.size() > 1)
    {
        reduce(pCodes);
    }
    return pCodes.begin()->second;
}

//Signature modifiée pour permettre de passer l'addresse de fichier comme paramètre
//Taille après la compression triviale: 60,7kB
// Le fichier est plus large que celui de début(34,4kB) car on ne sauvegarde pas
// les données sous forme binaire
std::string compress(std::vector<std::string> const &words, codes &c, std::string filepath)
{
    std::ofstream myfile(filepath);
    if (myfile.is_open())
    {
        for(auto &word : words)
        {
            auto coded = c.find(word);
            if(coded != c.end())
            {
                myfile << coded->second << '\n';
            }
        }
        myfile.close();
        return "Writing data to " + filepath + " done!";
    }
    return "Failed to write data to " + filepath;
}

typedef std::map<std::string, std::string> minimizedCodes;

//recurent function minimizing given string using passed dictionary
std::string minimizeSingleCode(const std::string &coded, const minimizedCodes &dictionary)
{
    bool changed = false;
    std::string result;
    for(int i = 0; i < coded.length() - coded.length()%2; i +=2)
    {
		//Take two chars and try to replace them 
		//with a new char from dictionary
        auto newMergedString = std::string(1,coded[i]) + coded[i+1];
        auto iteratorFound = dictionary.find(newMergedString);
        if(iteratorFound != dictionary.end())
        {
            newMergedString = iteratorFound->second;
            changed = true;
        }
        result += newMergedString;
    }
    //If the length of coded string is not pair we add the missing last
    //character(which was not treated in while loop beacus of '- coded.length()%2') to the result string
    if(coded.length()%2 == 1)
    {
        result += coded[coded.length()-1];
    }
    //Recurent call till nothing changes
    return changed ? minimizeSingleCode(result, dictionary) : result;
    //Example result: 00101111010 -> acddb0 -> fDr
}

//Returns minimise code dictionary
minimizedCodes getMinimizedDict(){
    minimizedCodes ToShortenDict;
      ToShortenDict["00"]="a";
      ToShortenDict["01"]="b";
      ToShortenDict["10"]="c";
      ToShortenDict["11"]="d";
      ToShortenDict["aa"]="A";
      ToShortenDict["bb"]="B";
      ToShortenDict["cc"]="C";
      ToShortenDict["dd"]="D";
      ToShortenDict["ab"]="e";
      ToShortenDict["ac"]="f";
      ToShortenDict["ad"]="g";
      ToShortenDict["ba"]="h";
      ToShortenDict["bc"]="i";
      ToShortenDict["bd"]="j";
      ToShortenDict["ca"]="k";
      ToShortenDict["cb"]="l";
      ToShortenDict["cd"]="m";
      ToShortenDict["da"]="n";
      ToShortenDict["db"]="o";
      ToShortenDict["dc"]="p";
      ToShortenDict["a0"]="q";
      ToShortenDict["b0"]="r";
      ToShortenDict["c0"]="s";
      ToShortenDict["d0"]="t";
      ToShortenDict["a1"]="u";
      ToShortenDict["b1"]="v";
      ToShortenDict["c1"]="w";
      ToShortenDict["d1"]="x";
    return ToShortenDict;
}

//Taille apres la compression plus short: 22,2kB
codes shorten(codes const &c)
{
    auto minimizeDict = getMinimizedDict();
    codes shortenCodes;
    for(auto &code : c)
    {
        shortenCodes[code.first]=minimizeSingleCode(code.second,minimizeDict);
    }
    return shortenCodes;
}

// A NE PAS DEPLACER
#include "check_1984.hpp"

int main()
{
  // FAITES VOS TESTS PERSOS ICI
    //create tests
    auto wordsToCompress = load();
    auto o = count(wordsToCompress);
    
    auto calculatedCodes = create(o);
    std::cout << compress(wordsToCompress, calculatedCodes, "compressedOutput.txt") << std::endl;
    //Calculate shortest code length for create function
		int shortestN = 100;
		for(const auto& elem : calculatedCodes) 
		{
			shortestN = elem.second.length() < shortestN ? elem.second.length() : shortestN;
		}
		
    auto shorterCodes = shorten(calculatedCodes);
    std::cout << compress(wordsToCompress,shorterCodes,"minimizedOutput.txt") << std::endl;
    //Calculate shortest code length for shorten function
		int shortestS = 100;
		for(const auto& elem : shorterCodes) 
		{
			shortestS = elem.second.length() < shortestS ? elem.second.length() : shortestS;
		}
    //Show results
    std::cout << "Shortest code for normal create: " << shortestN << std::endl;
    std::cout << "Shortest code for shorter version: " << shortestS << std::endl;

  // A NE PAS EFFACER
    run_tests();

  return 0;
}
