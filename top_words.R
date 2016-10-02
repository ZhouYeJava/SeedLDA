#!/usr/bin/Rscript

#################
####Top Words####
#################

####Zhou Ye####
####03/24/2015####

rm(list=ls())
setwd("/Users/yezhou/Documents/Xcode/SeedLDA/output/")
data <- read.csv("word_topic_prob.txt")
name <- "尺码"
d <- data[,c(1,which(names(data)==name))]
dd <- d[order(-d[,2]),]
dd[1:100,]