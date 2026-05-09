# -*- coding: utf-8 -*-
"""
Created on Fri Jun 16 17:53:28 2023

@author: mengdie
"""

import os
import pandas as pd
import csv


# general setting
distribution = 'dis'
instance = 'rbg010asep'
obj_type = 'cost-slack'

inst_name = distribution + '-' + instance

subinterval = 'true'

subinterval_type = ''
if obj_type == 'cost-slack':
    if subinterval == 'false':
        subinterval_type = '_single'
    else:
        subinterval_type = '_multi'

common_path = '../visualization/3_solution_evaluation_riskindex/'
input_path = common_path + 'aggregated_measures/with_cost_limit/' + inst_name + '/'
output_path =  common_path + 'tables/with_cost_limit/'

header = ['size', 'Measure', 'Mean_TT', 'ERI', 'SRI', 'Sum_Exp', 'Max_Exp', 'LateProb', 'Sum_LateProb', 'Max_LateProb', 'Num_LateNode', 'Lateness_avg', 'Lateness_wst', 'Latenode_avg', 'Latenode_wst', 'size', 'Measure', 'Mean_TT', 'ERI', 'SRI', 'Sum_Exp', 'Max_Exp', 'LateProb', 'Sum_LateProb', 'Max_LateProb', 'Num_LateNode', 'Lateness_avg', 'Lateness_wst', 'Latenode_avg', 'Latenode_wst', 'Time']

### read the csv files
file_list = os.listdir(input_path)  # Get the list of files in the folder

dataframes = []  # List to store the data from each file

# Iterate over the files in the folder
for file_name in file_list:
    if file_name.endswith('.csv'):  # Check if the file is a CSV file
        file_path = os.path.join(input_path, file_name)  # Get the full path to the file
        df = pd.read_csv(file_path)  # Read the CSV file into a DataFrame
        dataframes.append(df)  # Append the DataFrame to the list

# Concatenate the DataFrames into a single DataFrame
combined_df = pd.concat(dataframes, ignore_index=True)

#MDLS#
# #size 20
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_20') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_20_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_20 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_20') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_20_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MDLS_20 = [x + y + z for x, y, z in zip(MDLS_20_in, MDLS_20_out, time_20)]

# #size 50
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_50') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_50_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_50 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_50') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_50_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MDLS_50 = [x + y + z for x, y, z in zip(MDLS_50_in, MDLS_50_out, time_50)]

#size 80
selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_80') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MDLS')]
MDLS_80_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
time_80 = selected_rows.iloc[:, [24]].values.tolist()

selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_80') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MDLS')]
MDLS_80_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

MDLS_80 = [x + y + z for x, y, z in zip(MDLS_80_in, MDLS_80_out, time_80)]

# #size 100
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_100') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_100_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_100 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_100') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_100_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MDLS_100 = [x + y + z for x, y, z in zip(MDLS_100_in, MDLS_100_out, time_100)]

# #size 150
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_150') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_150_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_150 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_150') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MDLS')]
# MDLS_150_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MDLS_150 = [x + y + z for x, y, z in zip(MDLS_150_in, MDLS_150_out, time_150)]

#output both in-/out-sample to one csv file
output_file = output_path + obj_type + subinterval_type + '_MDLS_' + inst_name + '.csv'#tradeoff + '.csv'
with open(output_file, "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(header)
    #writer.writerows(MDLS_20)
    #writer.writerows(MDLS_50)
    writer.writerows(MDLS_80)
    #writer.writerows(MDLS_100)
    #writer.writerows(MDLS_150)


#MBBM#
# #size 20
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_20') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_20_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_20 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_20') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_20_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MBBM_20 = [x + y + z for x, y, z in zip(MBBM_20_in, MBBM_20_out, time_20)]

# #size 50
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_50') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_50_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_50 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_50') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_50_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MBBM_50 = [x + y + z for x, y, z in zip(MBBM_50_in, MBBM_50_out, time_50)]

#size 80
selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_80') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MBBM')]
MBBM_80_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
time_80 = selected_rows.iloc[:, [24]].values.tolist()

selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_80') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MBBM')]
MBBM_80_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

MBBM_80 = [x + y + z for x, y, z in zip(MBBM_80_in, MBBM_80_out, time_80)]

# #size 100
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_100') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_100_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_100 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_100') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_100_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MBBM_100 = [x + y + z for x, y, z in zip(MBBM_100_in, MBBM_100_out, time_100)]

# #size 150
# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_150') & (combined_df.iloc[:, 2] == 'in') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_150_in = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()
# time_150 = selected_rows.iloc[:, [24]].values.tolist()

# selected_rows = combined_df[(combined_df.iloc[:, 1] == 'size_150') & (combined_df.iloc[:, 2] == 'out') & (combined_df.iloc[:, 4] == 'MBBM')]
# MBBM_150_out = selected_rows.iloc[:, [1,5,9,10,11,12,13,14,15,16,17,20,21,22,23]].values.tolist()

# MBBM_150 = [x + y + z for x, y, z in zip(MBBM_150_in, MBBM_150_out, time_150)]

#output both in-/out-sample to one csv file
output_file = output_path + obj_type + subinterval_type + '_MBBM_' + inst_name + '.csv'#tradeoff + '.csv'
with open(output_file, "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerow(header)
    #writer.writerows(MBBM_20)
    #writer.writerows(MBBM_50)
    writer.writerows(MBBM_80)
    #writer.writerows(MBBM_100)
    #writer.writerows(MBBM_150)










