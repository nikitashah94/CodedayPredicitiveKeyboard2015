//
//  main.cpp - A Command-Line based demo using ncurses / curses
//  OpenScribe
//
//  Created by Prateek R Patil on 5/26/15.
//  Copyright (c) 2015 Prateek R Patil. All rights reserved.
//

#include <iostream>
#include <deque>
#include <iomanip>
#include <string>
#include <fstream>
#include <regex>
#include <sstream>
#include <memory>
#include <bitset>
#include <thread>
#include <ncurses.h>
#include <map>

using namespace std;

class CharacterNode {
protected:
    shared_ptr<map<char, shared_ptr<CharacterNode>>> nextCharacters = nullptr;
    CharacterNode* parent = nullptr;
    unsigned int count = 0;
    char currentPath = 0;
public:
    CharacterNode(){
        
    }
    
    CharacterNode(int count){
        this->count = count;
    }
    
    int getCount(){
        return this->count;
    }
    
    void setCount(int count){
        this->count = count;
    }
    
    map<char, shared_ptr<CharacterNode>>* getNextCharacters(){
        return this->nextCharacters.get();
    }
    
    CharacterNode* readTo(string path){
        CharacterNode* position = this;
        for (unsigned int i = 0; i < path.length(); i++) {
            if (position->getNextCharacters() == nullptr) {
                position->nextCharacters = shared_ptr<map<char, shared_ptr<CharacterNode>>>(new map<char, shared_ptr<CharacterNode>>);
            }
            if (position->getNextCharacters()->count(path[i]) == 0) {
                position->getNextCharacters()->insert(make_pair(path[i], nullptr));
            }
            if (position->getNextCharacters()->at(path[i]).get() == nullptr) {
                position->getNextCharacters()->at(path[i]) = shared_ptr<CharacterNode>(new CharacterNode);
            }
            position->getNextCharacters()->at(path[i])->parent = position;
            position = position->getNextCharacters()->at(path[i]).get();
            position->currentPath = path[i];
        }
        return position;
    }
    
    void readInWord(string word){
        readTo(word)->count++;
    }
    
    deque<CharacterNode*> getSearchResults(){
        deque<CharacterNode*> nodes;
        if(this->nextCharacters.get() != nullptr){
            for (auto& cn : *this->nextCharacters) {
                if (cn.second != nullptr) {
                    if (cn.second->count > 0) {
                        nodes.push_back(cn.second.get());
                    }
                    if (cn.second->nextCharacters.get() != nullptr) {
                        deque<CharacterNode*> childNodes = cn.second->getSearchResults();
                        while (!childNodes.empty()) {
                            nodes.push_back(childNodes.front());
                            childNodes.pop_front();
                        }
                    }
                }
            }
        }
        return nodes;
    }
    
    deque<CharacterNode*> getSearchResultsBF(){
        deque<CharacterNode*> nodes;
        if(this->nextCharacters.get() != nullptr){
            deque<CharacterNode*> toDive;
            for (auto& cn : *this->nextCharacters) {
                if (cn.second != nullptr) {
                    if (cn.second->count > 0) {
                        nodes.push_back(cn.second.get());
                    }
                    if (cn.second->nextCharacters.get() != nullptr) {
                        toDive.push_back(cn.second.get());
                    }
                }
            }
            for (auto cn : toDive) {
                if (cn->nextCharacters.get() != nullptr) {
                    if (cn->nextCharacters.get() != nullptr) {
                        deque<CharacterNode*> childNodes = cn->getSearchResultsBF();
                        while (!childNodes.empty()) {
                            nodes.push_back(childNodes.front());
                            childNodes.pop_front();
                        }
                    }
                }
            }
        }
        return nodes;
    }
    
    deque<CharacterNode*> getSearchResultsFrom(string rootPath){
        return this->readTo(rootPath)->getSearchResults();
    }
    
    deque<CharacterNode*> getSearchResultsFromBF(string rootPath){
        return this->readTo(rootPath)->getSearchResultsBF();
    }
    
    string print(){
        string r = &this->currentPath;
        if (this->parent != nullptr) {
            r = this->parent->print() + r;
        }
        return r;
    }
    
    friend ostream& operator << (ostream& o, CharacterNode& cn){
        
        o << cn.print() << " : " << cn.count;
        return o;
    }
    
    static CharacterNode MakeNodeFromFile(string filepath){
        CharacterNode newNode;
        ifstream file;
        file.open(filepath);
        string line;
        while (getline(file, line)) {
            newNode.readInWord(line);
        }
        file.close();
        return newNode;
    }
    
    static void SortByFrequency(deque<CharacterNode*>& results){
        bool isSorted = false;
        while (!isSorted) {
            int sCount = 0;
            for (int i = 1; i < results.size(); i++) {
                if (results.at(i - 1)->count < results.at(i)->count) {
                    sCount++;
                    CharacterNode* temp = results.at(i);
                    results.at(i) = results.at(i - 1);
                    results.at(i - 1) = temp;
                }
            }
            if (sCount == 0) {
                isSorted = true;
            }
        }
    }
    
    static void SaveToFile(CharacterNode* node, string filePath){
        ofstream outputFile;
        outputFile.open(filePath);
        deque<CharacterNode*> results = node->getSearchResultsBF();
        while (!results.empty()) {
            int tempCount = results.front()->count;
            while (tempCount > 0) {
                outputFile << results.front()->print() << endl;
                tempCount--;
            }
            results.pop_front();
        }
        outputFile.close();
    }
};

class DictionaryManager {
protected:
    string folderPath;
    CharacterNode defaultDictionary;
    CharacterNode userDictionary;
public:
    DictionaryManager(string folderPath){
        this->folderPath = folderPath;
    }
};

int main() {
    CharacterNode main = CharacterNode::MakeNodeFromFile("/Users/PRP/Documents/Work/OpenScribe/superdictionary.txt");
    initscr();
    raw();
    refresh();
    string response = "";
    string actualText = "";
    deque<CharacterNode*> results;
    while (response != "exit") {
        
        printw((actualText + "|\n").c_str());
        if (response.length() > 1) {
            //printw("Searching...\n");
            results = main.getSearchResultsFromBF(response);
            //printw("done.\nSorting...\n");
            CharacterNode::SortByFrequency(results);
            //printw("done.\n");
        } else {
            results.clear();
        }
        if (!results.empty()) {
            for (int i = 0; i < results.size() && i < 8; i++) {
                printw((results.at(i)->print() + "\n").c_str());
            }
        }
        char tempChar = getch();
        if (tempChar != '\t') {
            if (tempChar != 127 && tempChar != 8) {
                actualText += tempChar;
                if (isalnum(tempChar)) {
                    response += tempChar;
                } else {
                    main.readInWord(response);
                    response.clear();
                }
            } else {
                if (!actualText.empty()) {
                    actualText.pop_back();
                }
                if (!response.empty()) {
                    response.pop_back();
                }
            }
        } else {
            if (!results.empty()) {
                string wordToFill = results.front()->print();
                while (isalnum(actualText.back())) {
                    actualText.pop_back();
                }
                actualText += wordToFill;
                main.readInWord(wordToFill);
            }
        }
        clear();
    }
    endwin();
    exit(EXIT_SUCCESS);
    return 0;
}
