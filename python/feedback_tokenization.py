#!/usr/bin/python
# coding: utf-8

#########################
####Tokenize Feedback####
#########################

####Zhou Ye####
####03/11/2014####

import jieba.posseg as pseg
import codecs

def tokenize(path_in, path_out):
	reader = codecs.open(path_in, "r", "utf-8")
	writer = codecs.open(path_out, "w", "utf-8")
	text = reader.readlines()
	count = 0
	for line in text:
		count += 1
		if count==1:
			continue
		print count
		line = line.strip()
		line = line[1:(len(line)-1)]
		temp = line.split("\",\"")
		if len(temp)<7:
			continue
		words = pseg.cut(temp[5])
		output = ""
		for w in words:
			if w.flag.startswith("n"):
				output += w.word+" "
		if output=="":
			continue
		output += "\n"
		writer.write(output)
	reader.close()
	writer.close()

if __name__=="__main__":
	tokenize("/Users/yezhou/Documents/Xcode/SeedLDA/data/feedback_vocabulary.csv", "/Users/yezhou/Documents/Xcode/SeedLDA/data/tokenized_feedback_vocabulary.txt")
