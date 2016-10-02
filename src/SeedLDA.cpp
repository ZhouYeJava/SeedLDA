//
//  SeedLDA.cpp
//  SeedLDA
//
//  Created by Zhou Ye on 3/13/15.
//  Copyright (c) 2015 Zhou Ye. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "SeedLDA.h"

using namespace std;

Document::Document(vector<string> words, vector<int> b) {
    this->words = words;
    this->b = b;
}

vector<string> Document::getWords() {
    return words;
}

void Document::initializeTopic(int numOfWords) {
    for (int i=0; i<numOfWords; i++) {
        topics.push_back(0);
    }
}

void Document::initializeTopicCount(int numOfTopics) {
    for (int i=0; i<numOfTopics; i++) {
        topicCount.push_back(0);
    }
}

void Document::setTopic(int index, int topic) {
    topics[index] = topic;
}

int Document::getTopic(int index) {
    return topics[index];
}

void Document::increaseTopicCount(int topic) {
    topicCount[topic]++;
}

void Document::decreaseTopicCount(int topic) {
    topicCount[topic]--;
}

int Document::getTopicCount(int topic) {
    return topicCount[topic];
}

int Document::getTopicIndicator(int topic) {
    return b[topic]+1; //add one for computing probability
}

void Document::adjustTopicCount(int index, int oldTopic, int newTopic) {
    setTopic(index, newTopic); //change document topic
    decreaseTopicCount(oldTopic); //decrease count of old topic
    increaseTopicCount(newTopic); //increase count of new topic
}

SeedLDA::SeedLDA(double beta, double alpha, double pi) {
    this->beta = beta;
    this->alpha = alpha;
    this->pi = pi;
}

void SeedLDA::loadSeeds(string path) {
    ifstream reader(path);
    string line;
    int count = 0;
    while (!reader.eof()) {
        getline(reader, line);
        trim(line); 
        string pattern1 = ": ";
        string pattern2 = " ";
        line += pattern2; //easy for substring
        size_t start = line.find(pattern1);
        if (start!=string::npos) {
            string topic = line.substr(0, start); //topic predefined
            topicNames.push_back(topic);
            start += pattern1.size(); //jump to words
            size_t end = line.find(pattern2, start);
            vector<string> words;
            while (end<line.size()) {
                string word = line.substr(start, end-start);
                wordIndicator[word] = count; //index of topic
                words.push_back(word);
                start = end+pattern2.size();
                end = line.find(pattern2, start);
            }
            seeds[topic] = words;
        }
        else {
            break;
        }
        count++;
    }
    numOfTopics = count;
    reader.close();
}

void SeedLDA::printSeed() {
    string output = "Seed Words:\n";
    for (map<string, vector<string>>::iterator item=seeds.begin(); item!=seeds.end(); item++) {
        output += item->first+": ";
        vector<string> words = item->second;
        for (int i=0; i<words.size(); i++) {
            output += words[i]+" ";
        }
        output += "\n";
    }
    cout << output << endl;
}

void SeedLDA::loadDocuments(string path) {
    ifstream reader(path);
    string line;
    while (!reader.eof()) {
        getline(reader, line);
        trim(line);
        vector<string> words = split(line);
        Document document(words, obtainTopicIndicator(words));
        document.initializeTopic((int) words.size()); //initialize topic for each word
        document.initializeTopicCount(numOfTopics); //initialize topic count
        documents.push_back(document);
    }
}

vector<int> SeedLDA::obtainTopicIndicator(vector<string> words) {
    vector<int> b(numOfTopics);
    for (int i=0; i<numOfTopics; i++) {
        b[i] = 0;
    }
    for (int i=0; i<words.size(); i++) {
        if (wordIndicator[words[i]]>=0) {
            b[wordIndicator[words[i]]] = 1;
        }
    }
    return b;
}

void SeedLDA::printSummaryStatistics() {
    string output = "";
    output += "Number Of Documents = "+to_string(documents.size())+"\n";
    output += "Number Of Words = "+to_string(numOfWords)+"\n";
    output += "Number Of Topics = "+to_string(numOfTopics)+"\n";
    output += "Hyperparameter beta = "+to_string(beta)+"\n";
    output += "Hyperparameter alpha = "+to_string(alpha)+"\n";
    output += "Hyperparameter pi = "+to_string(pi)+"\n";
    cout << output << endl;
}

void SeedLDA::checkCorrectness() {
    vector<int> wordTotalCount(numOfTopics);
    for (int i=0; i<numOfTopics; i++) {
        wordTotalCount[i] = 0;
    }
    for (map<string, vector<int>>::iterator item=wordCount.begin(); item!=wordCount.end(); item++) {
        for (int i=0; i<item->second.size(); i++) {
            wordTotalCount[i] += item->second[i];
        }
    }
    bool flag = true;
    for (int i=0; i<numOfTopics; i++) {
        if (wordTotalCount[i]!=topicCount[i]) {
            flag = false;
            break;
        }
    }
    cout << "Correctness = "+to_string(flag) << endl;
}

void SeedLDA::initialize() {
    for (int i=0; i<numOfTopics; i++) {
        topicCount.push_back(0);
    }
    for (int i=0; i<documents.size(); i++) {
        vector<string> words = documents[i].getWords();
        for (int j=0; j<words.size(); j++) {
            if (wordCount.find(words[j])==wordCount.end()) {
                for (int k=0; k<numOfTopics; k++) {
                    wordCount[words[j]].push_back(0);
                }
            }
            int r = rand()%numOfTopics;
            wordCount[words[j]][r]++; //add one on word count
            topicCount[r]++; //add one on topic count
            documents[i].setTopic(j, r);
            documents[i].increaseTopicCount(r);
        }
    }
    numOfWords = (int) wordCount.size(); //assign number of words
}

void SeedLDA::iterate() {
    for (int i=0; i<documents.size(); i++) {
        vector<string> words = documents[i].getWords();
        for (int j=0; j<words.size(); j++) {
            if (wordIndicator[words[j]]>=0) { //sample from seed and regular words
                double r = rand()/(RAND_MAX+1.0);
                if (r<pi) {
                    int oldTopic = documents[i].getTopic(j);
                    int newTopic = wordIndicator[words[j]]; //seed topic
                    documents[i].adjustTopicCount(j, oldTopic, newTopic); //adjust document
                    adjustWordTopicCount(words[j], oldTopic, newTopic); //adjust global model
                }
                else {
                    double totalProb = 0;
                    vector<double> prob(numOfTopics);
                    int oldTopic = documents[i].getTopic(j);
                    for (int k=0; k<numOfTopics; k++) {
                        int extract = k==oldTopic ? 1 : 0; //current instance which needs to be extracted
                        prob[k] = (double) (wordCount[words[j]][k]-extract+beta)/(double) (topicCount[k]-extract+numOfWords*beta)*(double) (documents[i].getTopicCount(k)-extract+alpha*documents[i].getTopicIndicator(k));
                        totalProb += prob[k];
                    }
                    double rr = totalProb*rand()/(RAND_MAX+1.0);
                    double currProb = prob[0];
                    int newTopic = 0;
                    while (rr>currProb) {
                        newTopic++;
                        currProb += prob[newTopic];
                    }
                    documents[i].adjustTopicCount(j, oldTopic, newTopic); //adjust document
                    adjustWordTopicCount(words[j], oldTopic, newTopic); //adjust global model
                }
            }
            else { //only sample from regular words
                double totalProb = 0;
                vector<double> prob(numOfTopics);
                int oldTopic = documents[i].getTopic(j);
                for (int k=0; k<numOfTopics; k++) {
                    int extract = k==oldTopic ? 1 : 0; //current instance which needs to be extracted
                    prob[k] = (double) (wordCount[words[j]][k]-extract+beta)/(double) (topicCount[k]-extract+numOfWords*beta)*(double) (documents[i].getTopicCount(k)-extract+alpha*documents[i].getTopicIndicator(k));
                    totalProb += prob[k];
                }
                double rr = totalProb*rand()/(RAND_MAX+1.0);
                double currProb = prob[0];
                int newTopic = 0;
                while (rr>currProb) {
                    newTopic++;
                    currProb += prob[newTopic];
                }
                documents[i].adjustTopicCount(j, oldTopic, newTopic); //adjust document
                adjustWordTopicCount(words[j], oldTopic, newTopic); //adjust global model
            }
        }
    }
}

void SeedLDA::adjustWordTopicCount(string word, int oldTopic, int newTopic) {
    /*change word topic count*/
    vector<int> count = wordCount[word];
    count[oldTopic]--;
    count[newTopic]++;
    wordCount[word] = count;
    /*change total topic count*/
    topicCount[oldTopic]--;
    topicCount[newTopic]++;
}

void SeedLDA::collapsedGibbsSampling(int numOfIteration) {
    for (int i=0; i<numOfIteration; i++) {
        cout << "Iteration "+to_string(i+1) << endl;
        iterate();
        //checkCorrectness();
    }
    cout << "Collect Samples" << endl;
    iterate();
}

void SeedLDA::calculateWordTopicDistribution() {
    for (map<string, vector<int>>::iterator item=wordCount.begin(); item!=wordCount.end(); item++) {
        string word = item->first;
        vector<int> topics = item->second;
        vector<double> prob(topics.size());
        for (int i=0; i<topics.size(); i++) {
            prob[i] = (double) (topics[i]+beta)/(double) (topicCount[i]+numOfWords*beta);
        }
        wordDist[word] = prob;
    }
}

void SeedLDA::outputDistribution(string path) {
    ofstream writer(path);
    string title = "词语";
    for (int i=0; i<topicNames.size(); i++) {
        title += ","+topicNames[i];
    }
    title += "\n";
    writer << title;
    for (map<string, vector<double>>::iterator item=wordDist.begin(); item!=wordDist.end(); item++) {
        string word = item->first;
        vector<double> prob = item->second;
        string output = word;
        for (int i=0; i<prob.size(); i++) {
            output += ","+to_string(prob[i]);
        }
        output += "\n";
        writer << output;
    }
    writer.close();
}

int main() {
    SeedLDA slda(10, 0.1, 0.5);
    slda.loadSeeds("/Users/yezhou/Documents/Xcode/SeedLDA/data/seed_words.txt");
    slda.printSeed();
    slda.loadDocuments("/Users/yezhou/Documents/Xcode/SeedLDA/data/tokenized_feedback_vocabulary.txt");
    slda.initialize();
    slda.printSummaryStatistics();
    slda.checkCorrectness();
    slda.collapsedGibbsSampling(1000);
    slda.calculateWordTopicDistribution();
    slda.outputDistribution("/Users/yezhou/Documents/Xcode/SeedLDA/output/word_topic_prob.txt");
}
