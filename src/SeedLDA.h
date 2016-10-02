//
//  SeedLDA.h
//  SeedLDA
//
//  Created by Zhou Ye on 3/13/15.
//  Copyright (c) 2015 Zhou Ye. All rights reserved.
//

#include <string>
#include <vector>
#include <map>

using namespace std;

#ifndef DOCUMENT_H
#define DOCUMENT_H

class Document {
private:
    vector<string> words; //document words
    vector<int> b; //topic vector; 1 indicates the document contains this topic
    vector<int> topicCount; //counts of each topic for this document
    vector<int> topics; //topics for each word
    
public:
    Document(vector<string> words, vector<int> b);
    vector<string> getWords();
    void initializeTopicCount(int numOfTopics); //initialize topic count
    void initializeTopic(int numOfWords); //initialize topic for each word
    void increaseTopicCount(int topic); //add one on some topic
    void decreaseTopicCount(int topic); //reduce one on some topic
    int getTopicCount(int topic); //get some topic count
    void setTopic(int index, int topic); //set topic for some word
    int getTopic(int index); //get topic for some word
    void adjustTopicCount(int index, int oldTopic, int newTopic); //adjust topic count
    int getTopicIndicator(int topic); //get the topic indicator from seed words
};

#endif

#ifndef SeedLDA_H
#define SeedLDA_H

class SeedLDA {
private:
    vector<Document> documents; //documents
    vector<string> topicNames; //topic names
    map<string, vector<string>> seeds; //seed words
    map<string, int> wordIndicator; //word -> seed topic; it will not contain regular word
    map<string, vector<int>> wordCount; //count of words on each topic
    vector<int> topicCount; //counts of each topic
    map<string, vector<double>> wordDist; //word distribution
    int numOfTopics; //number of topics
    int numOfWords; //number of Words
    double beta; //prior of word
    double alpha; //prior of topic
    double pi; //prior of mixture
    
public:
    SeedLDA(double beta, double alpha, double pi);
    void loadSeeds(string path); //load seed data
    void loadDocuments(string path); //load all tokenized documents
    void printSeed(); //output seed data
    void printSummaryStatistics(); //output summary statistics
    void initialize(); //random initialization
    void checkCorrectness(); //check the correctness of word/topic count
    vector<int> obtainTopicIndicator(vector<string> words); //calculate topic vector for this document
    void calculateWordTopicDistribution(); //calculate the distribution of words and topics from counts
    void iterate(); //the iteration of collapsed gibbs sampling
    void adjustWordTopicCount(string word, int oldTopic, int newTopic); //adjust word topic count
    void collapsedGibbsSampling(int numOfInteration); //perform collapsed gibbs sampler
    void outputDistribution(string path); //output word topic distribution
};

#endif

#ifndef UTIL_H
#define UTIL_H

inline vector<string> split(string str, const string pattern = " ") {
    size_t pos;
    vector<string> result;
    str += pattern;
    int size = (int) str.size();
    for(int i=0; i<size; i++) {
        pos = str.find(pattern, i);
        if(pos<size) {
            string s=str.substr(i, pos-i);
            result.push_back(s);
            i = (int) (pos+pattern.size()-1); //change position
        }
    }
    return result;
};

inline void trim(string& str, const string& delimiters = " \f\n\r\t\v") {
    str.erase(str.find_last_not_of(delimiters)+1);
    str.erase(0, str.find_first_not_of(delimiters));
};

inline vector<double> randMultProb(int num) {
    vector<double> dist;
    double sum = 0;
    for (int i=0; i<num; i++) {
        double r = (double) rand();
        dist.push_back(r);
        sum += r;
    }
    for (int i=0; i<num; i++) {
        dist[i] = dist[i]/sum;
    }
    return dist;
}

#endif
