# -*- coding: utf-8 -*-
"""
Created on Wed May 24 14:28:03 2023

@author: mengdie
"""

import os

# check whether a string is contained in a filename
def contains_letters(string, letters):
    for letter in letters:
        if letter in string:
            return True
    return False

#check whether some strings are all contained in a filename
def check_filename (filename, names_to_check):
    return all(string in filename for string in names_to_check)

def merge_specific_rows(input_files, output_file, first_line, row_numbers):
    with open(output_file, 'w') as output:
        #output general info
        for letter in first_line:
            output.write(letter + ',')     
        output.write('\n')
        
        for file_name in input_files:
            with open(file_name, 'r') as file:
                #find out the measure
                row = file.readline().strip() 
                tokens = [' ', ',']
                words = []
                for token in tokens:
                    words.extend(row.split(token))           
                words = [word for word in words if word]
                output.write(words[8] + '&')
                output.write('\n')
                
                for line_number, line in enumerate(file, start=0):
                    if line_number in row_numbers:
                        #elements = line.split("&")
                        #elements = [element.strip() for element in elements]
                        #elements.pop(0)
                        #new_elements = elements[2:]
                                    
                        #for ele in new_elements:
                        #    output.write(ele + ',')
                        output.write(line)
            output.write('\n')

# for each given instance, e.g., dis-n60w120.001
# for each given objtype, e.g., cost-delta
# for each given method, e.g., MDLS
# for each given sample type, e.g., in_sample
# collect the info for four types of tradeoff measures, i.e., IC-Vio, IC-Nvio, IC-Wvio, IC-Wnvio
def collect_info(input_directory, output_directory, instance, obj_type, method_type, sample_type):
    
    #give the info of the related file names
    input_files = []
    name_to_check = [instance, method_type, sample_type]
    #print(name_to_check)
    
    #open the related files and restore the related info to separate files
    for filename in os.listdir(input_directory):
        if filename.endswith('.txt'):   
            if check_filename (filename, name_to_check):#if contains_letters(filename, name_to_check):
                 input_file_path = os.path.join(input_directory, filename)
                 input_files.append(input_file_path)
                 #print(str(input_files) + '\n')
                 
    output_measure_name = obj_type + '_measures_' + method_type + '_' + instance + '_' + sample_type + '.txt'
    output_file_measure = os.path.join(output_directory, output_measure_name)

    output_curve_name = obj_type + '_curve_' + method_type + '_' + instance + '_' + sample_type + '.txt'
    output_file_curve = os.path.join(output_directory, output_curve_name)
    
    first_line = [instance, obj_type, method_type, sample_type]

    # Define the rows to be read from each input file
    rows_measure = [63, 64, 65, 66, 67] #rows of measures
    rows_curve = [68, 69] #rows of tradeoff

    merge_specific_rows(input_files, output_file_measure, first_line, rows_measure)
    merge_specific_rows(input_files, output_file_curve, first_line, rows_curve)



# general setting #
inst_type = 'N60'
obj_type = 'cost-delta'
subinterval = 'false'

subinterval_type = ''
if obj_type == 'cost-slack':
    if subinterval == 'false':
        subinterval_type = '_single'
    else:
        subinterval_type = '_multi'


# aggregate info for each objective type
input_directory = '../data_output/eva_tradeoff/' + inst_type + '/size_80/' + obj_type + subinterval_type + '/'
output_directory = '../visualization/2_solution_evaluation_tradeoff/measures/aggregated/' + obj_type + subinterval_type + '/'

# MDLS -in#
collect_info(input_directory, output_directory, 'dis-n60w120.001', obj_type, 'MDLS', 'in')
collect_info(input_directory, output_directory, 'dis-n60w200.001', obj_type, 'MDLS', 'in')
collect_info(input_directory, output_directory, 'uni-n60w120.001', obj_type, 'MDLS', 'in')
collect_info(input_directory, output_directory, 'uni-n60w200.001', obj_type, 'MDLS', 'in')

# MDLS -out#
collect_info(input_directory, output_directory, 'dis-n60w120.001', obj_type, 'MDLS', 'out')
collect_info(input_directory, output_directory, 'dis-n60w200.001', obj_type, 'MDLS', 'out')
collect_info(input_directory, output_directory, 'uni-n60w120.001', obj_type, 'MDLS', 'out')
collect_info(input_directory, output_directory, 'uni-n60w200.001', obj_type, 'MDLS', 'out')

# MBBM -in#
collect_info(input_directory, output_directory, 'dis-n60w120.001', obj_type, 'MBBM', 'in')
collect_info(input_directory, output_directory, 'dis-n60w200.001', obj_type, 'MBBM', 'in')
collect_info(input_directory, output_directory, 'uni-n60w120.001', obj_type, 'MBBM', 'in')
collect_info(input_directory, output_directory, 'uni-n60w200.001', obj_type, 'MBBM', 'in')

# MBBM -out#
collect_info(input_directory, output_directory, 'dis-n60w120.001', obj_type, 'MBBM', 'out')
collect_info(input_directory, output_directory, 'dis-n60w200.001', obj_type, 'MBBM', 'out')
collect_info(input_directory, output_directory, 'uni-n60w120.001', obj_type, 'MBBM', 'out')
collect_info(input_directory, output_directory, 'uni-n60w200.001', obj_type, 'MBBM', 'out')





































