# -*- coding: utf-8 -*-
"""
Created on Fri Mar 25 19:11:59 2022

@author: Bradley
"""
import requests

def get_list_of_GM(fileName):
    f = open(fileName, 'r')
    text = f.read()
    f.close()
    text = text.replace("\"","")
    text = text.split(",")
    return text
    
def get_pgn(player, month, year):
    url = "https://api.chess.com/pub/player/" + player + "/games/" + year + "/" + month + "/pgn"
    return requests.get(url).text

def write_pgn(text, player, month, year):
    f = open("pgn/" + player + "," + year + "," + month + ".txt", "w")
    f.write(text)
    f.close()
    
names = get_list_of_GM("GMs.txt")

print("Last name is {}".format(names[-1]))
count = 0

for name in names:
    print("Files written {}".format(count))
    print("Finding games from player {}".format(name))
    for month in range(1, 13):
        for year in range(2022, 2010, -1):
            try:
                text = get_pgn(str(name), str(month), str(year))
                if (len(text) > 20):
                    count += 1
                    write_pgn(text, str(name), str(month), str(year))
                
            except:
                continue