# -*- coding: utf-8 -*-
"""
Created on Sun Jun 18 15:10:06 2023

@author: mengdie
"""

import os

def contains_letters(string, letters):
    for letter in letters:
        if letter in string:
            return True
    return False

#check whether some strings are all contained in a filename
def check_filename (filename, names_to_check):
    return all(string in filename for string in names_to_check)

def merge_specific_rows(input_files, output_file, first_line):
    with open(output_file, 'w') as output:
        #output general info
        for letter in first_line:
            output.write(letter + ',')     
        output.write('\n')
        
        for file_name in input_files:
            with open(file_name, 'r') as file:
                for line_number, line in enumerate(file, start=0):
                    if line_number == 24: ### rbg
                    #if line_number == 14: ### gde
                        # print(line)
                        # Split the line using the '&' delimiter
                        tokens = line.strip().split('&')
                    
                        # Remove leading and trailing whitespace from each token
                        tokens = [token.strip() for token in tokens]
                    
                        for token in tokens:
                            output.write(token + ',')
            output.write('\n')

def collect_info(input_directory, output_directory, instance, obj_type, method_type, size_name, sample_type):
    
    #give the info of the related file names
    input_files = []
    name_to_check = [method_type, sample_type]
    #print(name_to_check)
    
    #open the related files and restore the related info to separate files
    for filename in os.listdir(input_directory):
        if filename.endswith('.txt'):   
            if check_filename (filename, name_to_check):#if contains_letters(filename, name_to_check):
                 input_file_path = os.path.join(input_directory, filename)
                 input_files.append(input_file_path)
                 #print(str(input_files) + '\n')
                 
    output_name = obj_type + '_' + method_type + '_' + instance + '_' + size_name + '_' + sample_type + '.csv'
    output_file = os.path.join(output_directory, output_name)
    
    first_line = ['name', 'size', 'sample_type', 'objtype', 'method', 'eva_rule', 'cost', 'delta', 'slack', 'Mean_TT', 'ERI', 'SRI', 'Sum_Exp', 'Max_Exp', 'LateProb', 'Sum_LateProb', 'Max_LateProb', 'Num_LateNode', 'Sum_ExpEarly', 'Max_ExpEarly', 'Lateness_avg', 'Lateness_wst', 'Latenode_avg', 'Latenode_wst', 'Time']

    merge_specific_rows(input_files, output_file, first_line)



# general setting
distribution = 'dis'
instance = 'rbg010asep'
obj_type = 'cost-slack'

inst_name = distribution + '-' + instance

subinterval = 'false'
subinterval_type = ''
if obj_type == 'cost-slack':
    if subinterval == 'false':
        subinterval_type = '_single'
    else:
        subinterval_type = '_multi'

common_input = '../data_output/eva_riskindex/N12/with_cost_limit/'
output_path = '../visualization/3_solution_evaluation_riskindex/aggregated_measures/with_cost_limit/' + inst_name + "/"


# #size 20#
size_name = 'size_20'
input_path = common_input + size_name + '/' + obj_type + subinterval_type + '/'# + tradeoff
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'out')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'out')

#size 50#
size_name = 'size_50'
input_path = common_input + size_name + '/' + obj_type + subinterval_type + '/'# + tradeoff
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'out')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'out')

#size 80#
size_name = 'size_80'
input_path = common_input + size_name + '/' + obj_type + subinterval_type + '/'# rbg
#input_path = common_input + size_name + '/' + obj_type + subinterval_type + '/' + inst_name + '/' #GDE
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'out')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'out')

# #size 100#
size_name = 'size_100'
input_path = common_input + size_name + '/' + obj_type + subinterval_type + '/'# + tradeoff
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'out')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'out')

#size 150#
size_name = 'size_150'
input_path = common_input + size_name + '/' + obj_type + subinterval_type + '/'# + tradeoff
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MDLS', size_name, 'out')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'in')
collect_info(input_path, output_path, inst_name, obj_type, 'MBBM', size_name, 'out')




















