# -*- coding: utf-8 -*-
"""
Created on Wed May 24 14:28:03 2023

@author: mengdie
"""

import os

def contains_letters(string, letters):
    for letter in letters:
        if letter in string:
            return True
    return False

def collect_info(input_directory):   
    # Define the directory containing the input files
    input_files = []
    
    for filename in os.listdir(input_directory):
        if filename.endswith('.txt'):   
            input_file_path = os.path.join(input_directory, filename)
            input_files.append(input_file_path)
    
    # transfer the contents to lists 
    table_contents = []
    
    for file_name in input_files:
        with open(file_name, 'r') as file:
            for line_number, line in enumerate(file, start=0):
                if line_number == 13:
                    # Split the line using the '&' delimiter
                    tokens = line.strip().split('&')
                
                    # Remove leading and trailing whitespace from each token
                    tokens = [token.strip() for token in tokens]
                
                    # Append the tokens to the data list
                    table_contents.append(tokens)
                    break  # Exit the loop after reading the desired line
    return table_contents

def get_the_head(inputfile):
    table_head = []
    
    with open(inputfile, 'r') as file:
        for line_number, line in enumerate(file, start=0):
            if line_number == 12:
                # Split the line using the '&' delimiter
                tokens = line.strip().split('&')
            
                # Remove leading and trailing whitespace from each token
                tokens = [token.strip() for token in tokens]
            
                # Append the tokens to the data list
                #table_head.append(tokens)
                table_head = tokens
                break  # Exit the loop after reading the desired line
                
    return table_head


obj_type = 'cost-delta' #remember to change here when change the problem, i.e., BRTSPTW(B) or BRTSPTW(S)
test_class = ''
ref_type = 'exact'
subinterval_type = ''
if obj_type == 'cost-slack':
    subinterval_type = '_single'

common_input = '../data_output/indicators/N60/size_80/'
input_directory = common_input + ref_type + '/' + obj_type + subinterval_type + '/'
input_for_header = input_directory + obj_type + '_MBBM_dis-n60w120.001.txt' # only used to get the indicator names for columns

output_directory = '../visualization/1_performance_indicators_MDLSMBBM/'
output_file = output_directory + obj_type + '_' + ref_type + '.csv'

header = get_the_head(input_for_header)
#print(header[0])

new_header = []
new_header.append(header[0])
new_header.append(header[5])
new_header.append(header[6])
for i in range(7, len(header)):
    new_header.append(header[i]) 
for i in range(7, len(header)):
    new_header.append(header[i]) 

# combine for each instance the two algorithms
contents = collect_info(input_directory)
#print(contents[0][1])

content_dis1201 = []
content_uni1201 = []
content_dis2001 = []
content_uni2001 = []

content_dis1201.append('dis-n60w120.001')
content_dis2001.append('dis-n60w200.001')
content_uni1201.append('uni-n60w120.001')
content_uni2001.append('uni-n60w200.001')

for content in contents:
    if content[0] == 'n60w120.001' and content[1] == 'discrete':     
        if content[3] == 'MDLS':    
            for i in range(5, len(content)):
                content_dis1201.append(content[i]) 
                
    elif content[0] == 'n60w200.001' and content[1] == 'discrete':
        if content[3] == 'MDLS':    
            for i in range(5, len(content)):
                content_dis2001.append(content[i]) 
                
    elif content[0] == 'n60w120.001' and content[1] == 'uniform':
        if content[3] == 'MDLS':    
            for i in range(5, len(content)):
                content_uni1201.append(content[i])
                
    else:
        if content[3] == 'MDLS':    
            for i in range(5, len(content)):
                content_uni2001.append(content[i]) 
                
for content in contents:
    if content[0] == 'n60w120.001' and content[1] == 'discrete':    
        if content[3] == 'MBBM':    
            for i in range(7, len(content)):
                content_dis1201.append(content[i]) 
                
    elif content[0] == 'n60w200.001' and content[1] == 'discrete':
        if content[3] == 'MBBM':    
            for i in range(7, len(content)):
                content_dis2001.append(content[i])  
                
    elif content[0] == 'n60w120.001' and content[1] == 'uniform':
        if content[3] == 'MBBM':    
            for i in range(7, len(content)):
                content_uni1201.append(content[i]) 
                
    else:
        if content[3] == 'MBBM':    
            for i in range(7, len(content)):
                content_uni2001.append(content[i]) 
                
# print(content_dis1201)
# print(content_dis2001)
# print(content_uni1201)
# print(content_uni2001)

# output the aggregated info to a file
with open(output_file, 'w') as file:
    file.write('reference' + ',' + 'MDLS' + ',' + 'MBBM' + '\n')
    for item in new_header:
        file.write(str(item) + ',')
    file.write('\n')
    for item in content_dis1201:
        file.write(str(item) + ',')
    file.write('\n')
    for item in content_dis2001:
        file.write(str(item) + ',')
    file.write('\n')
    for item in content_uni1201:
        file.write(str(item) + ',')
    file.write('\n')
    for item in content_uni2001:
        file.write(str(item) + ',')
