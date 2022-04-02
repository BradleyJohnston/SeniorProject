# -*- coding: utf-8 -*-
"""
Created on Sat Apr  2 05:08:34 2022

@author: Bradley
"""

import os
import sys
import chess
import chess.engine
import numpy as np
import tensorflow as tf
from tensorflow.keras import datasets, layers, models

weights_file_location = "C:/Users/Bradley/Desktop/pgnDownloads/Network_Weights/chess_network.ckpt"
weights_dir = os.path.dirname(weights_file_location)

#engine = chess.engine.SimpleEngine.popen_uci("C:/Users/Bradley/Desktop/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe")

def testing_data():
    (train_images, train_labels), (test_images, test_labels) = datasets.cifar10.load_data()
    
    # Normalize pixel values to be between 0 and 1
    train_images, test_images = train_images / 255.0, test_images / 255.0
    
    print("Type of labels {}".format(type(train_labels)))
    print("Type of images {}".format(type(train_images)))
    print("Shape of images {}".format(train_images.shape))
    

def load_fen_to_bitboard(fen):
    bitboard = np.zeros(shape = (1, 12, 8, 8))
    
    row = 7
    col = 0
    
    for letter in fen:
        if letter == ' ':
            return bitboard
        if letter == '1' or letter == '2' or letter == '3' or letter == '4' or letter == '5' or letter == '6' or letter == '7' or letter == '8' or letter == '9':
            col += int(letter)
        else:
            if letter == 'P':
                bitboard[0, 0, row, col] = 1
            elif letter == 'N':
                bitboard[0, 1, row, col] = 1
            elif letter == 'B':
                bitboard[0, 2, row, col] = 1
            elif letter == 'R':
                bitboard[0, 3, row, col] = 1
            elif letter == 'Q':
                bitboard[0, 4, row, col] = 1
            elif letter == 'K':
                bitboard[0, 5, row, col] = 1
            elif letter == 'p':
                bitboard[0, 6, row, col] = 1
            elif letter == 'n':
                bitboard[0, 7, row, col] = 1
            elif letter == 'b':
                bitboard[0, 8, row, col] = 1
            elif letter == 'r':
                bitboard[0, 9, row, col] = 1
            elif letter == 'q':
                bitboard[0, 10, row, col] = 1
            elif letter == 'k':
                bitboard[0, 11, row, col] = 1
            elif letter == '/':
                row -= 1
                col = -1
            
            col += 1
    return bitboard

def create_model():
    model = models.Sequential()
    input_shape = (12, 8, 8)
    x = tf.random.normal(input_shape)
    model.add(layers.Conv2D(8, 1, activation='relu', 
                            input_shape=(input_shape)))
    model.add(layers.Flatten())
    model.add(layers.Dense(256))
    model.add(layers.Dense(128))
    model.add(layers.Dense(32))
    model.add(layers.Dense(1, activation='linear'))
    
    model.compile(optimizer='adam',
                  loss=tf.keras.losses.MeanSquaredError(),
                  metrics=['accuracy'])
    
    return model

def evaluate(fen):
    engine = chess.engine.SimpleEngine.popen_uci("C:/Users/Bradley/Desktop/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe")
    
    try:
        board = chess.Board(fen + " w - - 0 1")
        info = engine.analyse(board, chess.engine.Limit(depth=5))
    except ValueError:
        print("Improper fen code {}".format(fen + " w - - 0 1"))
        return 0
    except:     
        e = sys.exc_info()[0]
        print(e)
        return 0
    results = info["score"].white().score()
    engine.quit()
    return results

'''async def evaluate(fen) -> float:
    engine = await chess.engine.popen_uci("C:/Users/Bradley/Desktop/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2/stockfish_14.1_win_x64_avx2.exe")
    board = chess.Board(fen)
    info = await engine.analyse(board, chess.engine.Limit(time=.1))
    results = info["score"].white().score()
    await engine.quit()
    return results
'''
def parse_line(line):
    fen = str(line.split(' ')[0])
    result = np.ndarray((1))
    result[0] = evaluate(fen)
    
    return result, fen


def train_model(model, filename, size_of_train, size_of_validate):
    reader = open(filename, 'r')
    size_of_validate += size_of_train
    cp_callback = tf.keras.callbacks.ModelCheckpoint(filepath = weights_file_location,
                                                     save_weights_only = True,
                                                     verbose = 1)
    empty_array = np.empty(shape=(size_of_train, 12, 8, 8), dtype=float)
    small_empty_array = np.empty(shape=(size_of_train), dtype=float)
    x_train = empty_array
    y_train = small_empty_array
    x_validate = empty_array
    y_validate = small_empty_array
    axis_var = 0
    
    count = 0
    
    for line in reader:
        if line == "\n":
            continue
        
        result, fen = parse_line(line)
        
        
        count += 1
        if count % 10 == 0:
            print("Read {} boards".format(count))
            
        if count < size_of_train:        
            np.append(x_train, (load_fen_to_bitboard(fen)), axis = axis_var)
            np.append(y_train, np.array(result), axis = axis_var)
        elif count < size_of_validate and count > size_of_train:
            np.append(x_validate, (load_fen_to_bitboard(fen)), axis = axis_var)
            np.append(y_validate, np.array(result), axis = axis_var)
        elif count > size_of_validate:
            model.fit(x_train, np.array(y_train), 
                      epochs = 5,
                      validation_data = (x_validate, np.array(y_validate)),
                      callbacks = [cp_callback])
            
            model.evaluate(x_validate, y_validate)
            
            x_train = empty_array
            y_train = small_empty_array
            x_validate = empty_array
            y_validate = small_empty_array
            count = 0
    reader.close()

def get_board_value(fen):
    model = create_model()
    model.load_weights(weights_dir)
    results = model.predict(load_fen_to_bitboard(fen))
    return results

def main():
    model = create_model()
    #model.load_weights(weights_dir)
    train_model(model, "board_values.txt", 100, 100)

if __name__ == '__main__':
    main()