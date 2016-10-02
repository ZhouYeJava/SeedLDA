#!/usr/bin/python
# coding: utf-8

###################
####Words Count####
###################

####Zhou Ye####
####03/12/2015####

import codecs

def words_count(path_in, path_out):
	vocabulary = {}
	reader = codecs.open(path_in, "r", "utf-8")
	text = reader.readlines()
	print len(text)
	for line in text:
		temp = line.strip().split(" ")
		for word in temp:
			if word not in vocabulary:
				vocabulary[word] = 1
			else:
				vocabulary[word] = vocabulary[word]+1
	reader.close()
	voc_list = []
	for word in vocabulary:
		voc_list.append((word, vocabulary[word]))
	voc_list = sorted(voc_list, key=lambda x : -x[1])
	writer = codecs.open(path_out, "w", "utf-8")
	for wc in voc_list:
		output = wc[0]+" "+str(wc[1])+"\n"
		writer.write(output)
	writer.close()

if __name__=="__main__":
	words_count("/Users/yezhou/Documents/Xcode/SeedLDA/data/tokenized_feedback_vocabulary.txt", "/Users/yezhou/Documents/Xcode/SeedLDA/data/words_count.txt")
