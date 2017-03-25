/*
 * HTS programming mission 1 solver
 * (c) Sebastien Gonzalve 2017
 *
 * This program takes the word list file as first argument and [1 .. n] scrambled words
 * ex:
 *     ./prog1 wordlist.txt $(cat list)
 *
 * compile on Linux with
 *    g++ -std=c++14 -o prog1 prog1.cpp
 *
 */
#include <algorithm>                                                                                
#include <iostream>                                                                                
#include <fstream>                                                                                  
#include <map>                                                                                      
#include <numeric>
#include <vector>                                                                                  
                                                                                                    
int main(int argc, char **argv)                                                                    
{                                                                                                  
    if (argc < 3) {                                                                                
        std::cerr << "Usage: " << argv[0] << " wordlist.txt scrambled_word1 [scrambled_word2 ...]\n"
                     " ex:\n"
                     " " << argv[0] << " wordlist.txt $(cat list)" << std::endl;                                                                    
        return 1;                                                                                  
    }                                                                                              
                                                                                                    
    std::string fileName(argv[1]);                                                                  
    std::ifstream wordFile(fileName);                                                              
    if (!wordFile) {                                                                                
        std::cerr << "Cannot open '" << fileName << "' for read." << std::endl;                    
        return 2;                                                                                  
    }                                                                                              
                                                                                                    
    std::vector<std::string> scrambledWords(argv + 2, argv + argc);                                
                                                                                                    
    /* input words are scrambled, thus the letters order is random, which means that letters may be
     * reordered without impact on the output. This is very useful because I can sort letters to use
     * them as keys of a map */                                                                    
    for (auto &sw : scrambledWords) {                                                              
        std::sort(sw.begin(), sw.end()); // sort all scramble words' letters                        
    }                                                                                              
                                                                                                    
    std::map<std::string, std::string> words;                                                      
                                                                                                    
    std::string word;                                                                              
    while (std::getline(wordFile, word)) {                                                          
        /* Remove '\r' */                                                                          
        word.erase(std::remove_if(word.begin(), word.end(), [](auto c) { return c == '\r'; }),      
                   word.end());                                                                    
                                                                                                    
        std::string orderedLetters(word);                                                          
        std::sort(orderedLetters.begin(), orderedLetters.end());                                    
                                                                                                    
        words[orderedLetters] = word;                                                              
    }                                                                                              
                                                                                                    
    /* used scrambled words as key of my map to find matching words */                              
    std::transform(scrambledWords.begin(), scrambledWords.end(), scrambledWords.begin(), [&words](auto sw) { return words.at(sw);});

    std::cout << accumulate(scrambledWords.begin() + 1, scrambledWords.end(), scrambledWords[0],
            [](auto a, auto b) {
            return a + "," + b;
            }) << std::endl;
                                                                                                    
    return 0;
}
