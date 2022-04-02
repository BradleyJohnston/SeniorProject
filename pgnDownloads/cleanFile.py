# -*- coding: utf-8 -*-
"""
Created on Sun Mar 27 13:34:32 2022

@author: Bradley
"""

def clean_file(outFile, inFile):
    read = open(inFile, "r")
    write = open(outFile, "w")
    
    results = ""
    game_history = ""
    
    game_history_flag = False
    for line in read:
        if line[0:3] == "1. ":
            game_history_flag = True
        
        if line[0:7] == "[Result":
            results = line
        
        if game_history_flag == True:
            game_history += ' ' + line
        
        if line[0] == '\n' and game_history_flag == True:
            game_history_flag = False
            write.write(results + game_history)
            results = ""
            game_history = ""
    
    read.close()
    write.close()

def remove_time_signatures(outFile, inFile):
    read = open(inFile, "r")
    write = open(outFile, "w")
    
    results = ""
    time_flag = False
    newline_flag = False

    for line in read:

        if line[1] == '1':
            results = ""
            time_flag = False
            for letter in line:
                if letter == '{':
                    time_flag = True
                
                if time_flag == False:
                    results += letter
                
                if letter == '}':
                    time_flag = False
            write.write(results)
        else:
            write.write(line)
    
    read.close()
    write.close()
                


def main():
    remove_time_signatures("timelessFile.txt", "cleanFile.txt")
if __name__ == "__main__":
    main()
        