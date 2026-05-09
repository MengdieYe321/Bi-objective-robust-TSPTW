# -*- coding: utf-8 -*-
"""
Created on Wed May 24 14:28:03 2023

@author: mengdie
"""



import math
import csv

# check whether a string is contained in a filename
def contains_letters(string, letters):
    for letter in letters:
        if letter in string:
            return True
    return False

#check whether some strings are all contained in a filename
def check_filename (filename, names_to_check):
    return all(string in filename for string in names_to_check)

def contains_only_nan(lst):
    return all(math.isnan(x) for x in lst)

def remove_nan_and_empty_elements(lst):
    #return [x for x in lst if not math.isnan(x)]
    return [x for x in lst if x != ' ' and not math.isnan(x)]

def read_and_clean_data(input_file, obj, sample_type):
    
    vio = []
    nvio = []
    wvio = []
    wnvio = []
    
    #read the txt file
    with open(input_file, 'r') as file:
        #nvio
        for line_number, line in enumerate(file, start=0):
            if line_number in [2,3]:
                elements = line.split("&")
                elements = [element.strip() for element in elements]
                nvio.append(elements[2:])
        
        for data in nvio:
            data.pop(-1)
            data.insert(0, obj)
            data.insert(1, sample_type)
        
        nvio[0].insert(2, 'IC')
        nvio[1].insert(2, 'Nvio')
        
    with open(input_file, 'r') as file:
        #vio
        for line_number, line in enumerate(file, start=0):
            if line_number in [5,6]:
                elements = line.split("&")
                elements = [element.strip() for element in elements]
                vio.append(elements[2:])
        
        for data in vio:
            data.pop(-1)
            data.insert(0, obj)
            data.insert(1, sample_type)
        
        vio[0].insert(2, 'IC')
        vio[1].insert(2, 'Vio')
                        
    with open(input_file, 'r') as file:
        #wnvio
        for line_number, line in enumerate(file, start=0):
            if line_number in [8,9]:
                elements = line.split("&")
                elements = [element.strip() for element in elements]
                wnvio.append(elements[2:]) 
        
        for data in wnvio:
            data.pop(-1)
            data.insert(0, obj)
            data.insert(1, sample_type)
        
        wnvio[0].insert(2, 'IC')
        wnvio[1].insert(2, 'Wnvio')
                            
    with open(input_file, 'r') as file:
        #wvio
        for line_number, line in enumerate(file, start=0):
            if line_number in [11,12]:
                elements = line.split("&")
                elements = [element.strip() for element in elements]
                wvio.append(elements[2:]) 
        
        for data in wvio:
            data.pop(-1)
            data.insert(0, obj)
            data.insert(1, sample_type)
        
        wvio[0].insert(2, 'IC')
        wvio[1].insert(2, 'Wvio')
                         
    return vio, nvio, wvio, wnvio


# general setting # #one instance + method + distribution at a time #
inst_type = 'N60'
measure_type = 'curve'
method_type = 'MDLS'      ##
instance = 'n60w120.001'  ##
distribution = 'uniform' ##
dis_type = ''
if distribution == 'discrete':
    dis_type = 'dis-'
else:
    dis_type = 'uni-'

file_directory = '../visualization/2_solution_evaluation_tradeoff/measures/'

filename_output = 'tradeoff_' + method_type + '_' + instance + '_' + distribution  # lack of the tradeoff measures + .csv
filename_input = measure_type + '_' + method_type + '_' + dis_type + instance # lack sample type + .csv

#define the output path
output_dir = file_directory + 'plots_info/' + filename_output

#define the input directories
input_dir_delta = file_directory + 'aggregated/cost-delta/cost-delta_' + filename_input
input_dir_slack_s = file_directory + 'aggregated/cost-slack_single/cost-slack_' + filename_input
input_dir_slack_m = file_directory + 'aggregated/cost-slack_multi/cost-slack_' + filename_input 

#--------------------------------
#restor all the related info into corresponding lists, i.e., Vio, Nvio, Wvio, Wnvio
# in_sample
sample_type = 'in'
#---------------------
#delta
delta_vio, delta_nvio, delta_wvio, delta_wnvio = read_and_clean_data(input_dir_delta + '_' + sample_type + '.txt', 'delta', 'in')
# print (delta_nvio)
# print (delta_vio)
# print (delta_wnvio)
# print (delta_wvio)

#---------------------
#slack_single
slack_vio, slack_nvio, slack_wvio, slack_wnvio = read_and_clean_data(input_dir_slack_s + '_' + sample_type + '.txt', 'slack', 'in')

#---------------------
#slack_multi
slack1501_vio, slack1501_nvio, slack1501_wvio, slack1501_wnvio = read_and_clean_data(input_dir_slack_m + '_' + sample_type + '.txt', 'slack_m', 'in')


#-------------------------------
#out_sample
sample_type = 'out'
#---------------------
#delta
delta_vio_out, delta_nvio_out, delta_wvio_out, delta_wnvio_out = read_and_clean_data(input_dir_delta + '_' + sample_type + '.txt', 'delta', 'out')

#---------------------
#slack_single
slack_vio_out, slack_nvio_out, slack_wvio_out, slack_wnvio_out = read_and_clean_data(input_dir_slack_s + '_' + sample_type + '.txt', 'slack', 'out')

#---------------------
#slack_multi
slack1501_vio_out, slack1501_nvio_out, slack1501_wvio_out, slack1501_wnvio_out = read_and_clean_data(input_dir_slack_m + '_' + sample_type + '.txt', 'slack_m', 'out')


#-------------------------------
#output for each tradeoff_measures
#-----------------
#IC-Vio
# Find the maximum number of columns among all rows
max_columns_in = max(len(row) for row in delta_vio + slack_vio + slack1501_vio)
max_columns_out = max(len(row) for row in delta_vio_out + slack_vio_out + slack1501_vio_out)
max_columns = max(max_columns_in, max_columns_out)

# Pad shorter rows with placeholders (',') to match the maximum number of columns
delta_vio_padded = [row + [None] * (max_columns - len(row)) for row in delta_vio]
slack_vio_padded = [row + [None] * (max_columns - len(row)) for row in slack_vio]
slack1501_vio_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_vio]

delta_vio_out_padded = [row + [None] * (max_columns - len(row)) for row in delta_vio_out]
slack_vio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack_vio_out]
slack1501_vio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_vio_out]
  
with open(output_dir + '_IC-Vio.csv', "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerows(delta_vio_padded)
    writer.writerows(slack_vio_padded) 
    writer.writerows(slack1501_vio_padded)  
    
    writer.writerows(delta_vio_out_padded)
    writer.writerows(slack_vio_out_padded)
    writer.writerows(slack1501_vio_out_padded)
 
#-----------------
#IC-Nvio
max_columns_in = max(len(row) for row in delta_nvio + slack_nvio + slack1501_nvio)
max_columns_out = max(len(row) for row in delta_nvio_out + slack_nvio_out + slack1501_nvio_out)
max_columns = max(max_columns_in, max_columns_out)

# Pad shorter rows with placeholders (',') to match the maximum number of columns
delta_nvio_padded = [row + [None] * (max_columns - len(row)) for row in delta_nvio]
slack_nvio_padded = [row + [None] * (max_columns - len(row)) for row in slack_nvio]
slack1501_nvio_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_nvio]

delta_nvio_out_padded = [row + [None] * (max_columns - len(row)) for row in delta_nvio_out]
slack_nvio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack_nvio_out]
slack1501_nvio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_nvio_out]

with open(output_dir + '_IC-Nvio.csv', "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerows(delta_nvio_padded)
    writer.writerows(slack_nvio_padded) 
    writer.writerows(slack1501_nvio_padded)  
    
    writer.writerows(delta_nvio_out_padded)
    writer.writerows(slack_nvio_out_padded)
    writer.writerows(slack1501_nvio_out_padded)

#-----------------    
#IC-Wvio
max_columns_in = max(len(row) for row in delta_wvio + slack_wvio + slack1501_wvio)
max_columns_out = max(len(row) for row in delta_wvio_out + slack_wvio_out + slack1501_wvio_out)
max_columns = max(max_columns_in, max_columns_out)

# Pad shorter rows with placeholders (',') to match the maximum number of columns
delta_wvio_padded = [row + [None] * (max_columns - len(row)) for row in delta_wvio]
slack_wvio_padded = [row + [None] * (max_columns - len(row)) for row in slack_wvio]
slack1501_wvio_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_wvio]

delta_wvio_out_padded = [row + [None] * (max_columns - len(row)) for row in delta_wvio_out]
slack_wvio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack_wvio_out]
slack1501_wvio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_wvio_out]

with open(output_dir + '_IC-Wvio.csv', "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerows(delta_wvio_padded)
    writer.writerows(slack_wvio_padded) 
    writer.writerows(slack1501_wvio_padded)  
    
    writer.writerows(delta_wvio_out_padded)
    writer.writerows(slack_wvio_out_padded)
    writer.writerows(slack1501_wvio_out_padded)

#-----------------    
#IC-Wnvio
max_columns_in = max(len(row) for row in delta_wnvio + slack_wnvio + slack1501_wnvio)
max_columns_out = max(len(row) for row in delta_wnvio_out + slack_wnvio_out + slack1501_wnvio_out)
max_columns = max(max_columns_in, max_columns_out)

# Pad shorter rows with placeholders (',') to match the maximum number of columns
delta_wnvio_padded = [row + [None] * (max_columns - len(row)) for row in delta_wnvio]
slack_wnvio_padded = [row + [None] * (max_columns - len(row)) for row in slack_wnvio]
slack1501_wnvio_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_wnvio]

delta_wnvio_out_padded = [row + [None] * (max_columns - len(row)) for row in delta_wnvio_out]
slack_wnvio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack_wnvio_out]
slack1501_wnvio_out_padded = [row + [None] * (max_columns - len(row)) for row in slack1501_wnvio_out]

with open(output_dir + '_IC-Wnvio.csv', "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerows(delta_wnvio_padded)
    writer.writerows(slack_wnvio) 
    writer.writerows(slack1501_wnvio_padded)  
    
    writer.writerows(delta_wnvio_out_padded)
    writer.writerows(slack_wnvio_out_padded)
    writer.writerows(slack1501_wnvio_out_padded)



























