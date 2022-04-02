# -*- coding: utf-8 -*-
"""
Created on Sun Mar 27 13:31:29 2022

@author: Bradley
"""

def print_file_line_by_line(fileName, numberOfLines):
    f = open(fileName, "r")
    
    for i in range(numberOfLines):
        text = f.readline()
        print(text)
    
    f.close()
    
def main():
    print_file_line_by_line("board_values.txt", 250)    

if __name__ == "__main__":
    main()
     