# -*- coding: utf-8 -*-
"""
Created on Tue Jun 13 17:05:08 2023

@author: YeMengdie
"""

import pandas as pd
import math
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib import rc
import matplotlib.lines as mlines



def contains_only_nan(lst):
    return all(math.isnan(x) for x in lst)

def remove_nan_and_empty_elements(lst):
    #return [x for x in lst if not math.isnan(x)]
    return [x for x in lst if x != ' ' and not math.isnan(x)]


def read_and_clean_data(input_file):
    # Read the CSV file
    df = pd.read_csv(input_file, header=None)
    
    # Store data of each row (excluding the header) into a list
    data_list = []
    for _, row in df.iterrows():
        data_list.append(row.values[3:])
    
    # clean the data
    data_list_new = []
    for data in data_list:
        if not contains_only_nan(data):
            data = remove_nan_and_empty_elements(data)
            #print(data)   
            data_list_new.append(data)
            
    return data_list_new

def sort_tradeoff_with_objectives (rows, data_list, data_row_index):
    
    target_list = []
    cost_percent = []
    
    for i in range(rows):
        # Create a new row
        row = data_list[data_row_index]
        target_list.append(row)
        data_row_index += 1
    
    for data_ic in target_list[0]:
        data_ic = data_ic * 100
        cost_percent.append(data_ic)
    
    target_list[0] = cost_percent
    #print(target_list[0])
    return data_row_index, target_list


# =============================================================================
method1 = 'MDLS'
method2 = 'MBBM'
instance = 'n60w200.001'
distribution = 'discrete'
measure_IC = 'IC'
measure_vio = 'Vio'
#measure_vio2 = 'Wvio'

plot_label_IC = r''
plot_label_vio = r''
if measure_vio == 'Vio':
    plot_label_IC = r'$C_{avg}\,(\%)$'
    plot_label_vio = r'$L_{avg}$'
elif measure_vio == 'Nvio':
    plot_label_IC = r'$C_{avg}\,(\%)$'
    plot_label_vio = r'$N_{avg}$'
elif measure_vio == 'Wvio':
    plot_label_IC = r'$C^{L}_{wst}\,(\%)$'
    plot_label_vio = r'$L_{wst}$'
else:
    plot_label_IC = r'$C^{N}_{wst}\,(\%)$'
    plot_label_vio = r'$N_{wst}$'
    
plot_title = instance
if distribution == 'discrete':
    plot_title = plot_title + ' (two-point)'
else:
    plot_title = plot_title + ' (uniform)'

IC_gap = 0.5
Vio_gap = 1
if measure_vio == 'Nvio' or measure_vio == 'Wnvio':
    Vio_gap = 0.1
#Vio_gap2 = 0.1

extra_space=0 #uni2001 wvio =2 wnvio 0.2 nvio 0.1

# set the input and output path
file_path = '../visualization/2_solution_evaluation_tradeoff/'
input_file_path = file_path + 'measures/plots_info/'
output_file_path = file_path + 'plots/algo_selection/'

# read in the two files for same instance with two algorithms
filename1 = 'tradeoff_' + method1 + '_' + instance + '_' + distribution + '_' + measure_IC + '-' + measure_vio + '.csv'
inputfile1 = input_file_path + filename1

filename2 = 'tradeoff_' + method2 + '_' + instance + '_' + distribution + '_' + measure_IC + '-' + measure_vio + '.csv'
inputfile2 = input_file_path + filename2

#set the output info 
filename_out = instance + '_' + distribution + '_' + measure_IC + '-' + measure_vio
output_file = output_file_path + filename_out

#---------------------------------------------#
# read the data and preprocessing
data_list1 = read_and_clean_data(inputfile1)
# for data in data_list1:
#     print(data)

# restore the data into corresponding lists
curr_row_in_list = 0
# in-sample performance
curr_row_in_list, delta_tradeoff_1 = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_tradeoff_general_1 = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_tradeoff_1501_1 = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
    
#out-sample performance
curr_row_in_list, delta_tradeoff_out_1 = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_tradeoff_general_out_1 = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_tradeoff_1501_out_1 = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)

#---------------------------------------------#
# read another data if necessary
data_list2 = read_and_clean_data(inputfile2)

curr_row_in_list = 0
# in-sample performance
curr_row_in_list, delta_tradeoff_2 = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_tradeoff_general_2 = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_tradeoff_1501_2 = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
    
#out-sample performance
curr_row_in_list, delta_tradeoff_out_2 = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_tradeoff_general_out_2 = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_tradeoff_1501_out_2 = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)


#--------------------------------------------#
# Create a figure and subplots
#----------------------#
# setting of the plots
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
rc('font',**{'family':'serif','serif':['Times']})
rc('text', usetex=True)
sns.set_style("whitegrid")

SMALL_SIZE = 8
MEDIUM_SIZE = 10
BIGGER_SIZE = 12
plt.rc('font', size=MEDIUM_SIZE)          # controls default text sizes
plt.rc('axes', titlesize=BIGGER_SIZE)    # fontsize of the axes title
plt.rc('axes', labelsize=MEDIUM_SIZE)     # fontsize of the x and y labels
plt.rc('xtick', labelsize=MEDIUM_SIZE)    # fontsize of the tick labels
plt.rc('ytick', labelsize=MEDIUM_SIZE)    # fontsize of the tick labels
plt.rc('legend', fontsize=8)    # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

#---------------------#
##calc maximum / minimum for scaling the axis
#in_sample
maxIC_slack_1501 = max(max(slack_tradeoff_1501_1[0]), max(slack_tradeoff_1501_2[0]))  #0
maxVio_slack_1501 = max(max(slack_tradeoff_1501_1[1]), max(slack_tradeoff_1501_2[1])) #0
minIC_slack_1501 = min(min(slack_tradeoff_1501_1[0]), min(slack_tradeoff_1501_2[0])) #1000


maxIC_delta = max(max(delta_tradeoff_1[0]), max(delta_tradeoff_2[0]))
maxIC_slack_general = max(max(slack_tradeoff_general_1[0]), max(slack_tradeoff_general_2[0]))
maxIC_slack_multiple = maxIC_slack_1501

maxVio_delta = max(max(delta_tradeoff_1[1]), max(delta_tradeoff_2[1]))
maxVio_slack_general = max(max(slack_tradeoff_general_1[1]), max(slack_tradeoff_general_2[1]))
maxVio_slack_multiple = maxVio_slack_1501

minIC_delta = min(min(delta_tradeoff_1[0]), min(delta_tradeoff_2[0]))
minIC_slack_general = min(min(slack_tradeoff_general_1[0]), min(slack_tradeoff_general_2[0]))
minIC_slack_multiple = minIC_slack_1501


maxIC_in = max(maxIC_delta, maxIC_slack_general, maxIC_slack_multiple)
maxVio_in = max(maxVio_delta, maxVio_slack_general, maxVio_slack_multiple)
minIC_in = min(minIC_delta, minIC_slack_general, minIC_slack_multiple)

#out_sample
maxIC_slack_1501 = max(max(slack_tradeoff_1501_out_1[0]), max(slack_tradeoff_1501_out_2[0]))  #0
maxVio_slack_1501 = max(max(slack_tradeoff_1501_out_1[1]), max(slack_tradeoff_1501_out_2[1])) #0

minIC_slack_1501 = min(min(slack_tradeoff_1501_out_1[0]), min(slack_tradeoff_1501_out_2[0])) #1000


maxIC_delta = max(max(delta_tradeoff_out_1[0]), max(delta_tradeoff_out_2[0]))
maxIC_slack_general = max(max(slack_tradeoff_general_out_1[0]), max(slack_tradeoff_general_out_2[0]))
maxIC_slack_multiple = maxIC_slack_1501

maxVio_delta = max(max(delta_tradeoff_out_1[1]), max(delta_tradeoff_out_2[1]))
maxVio_slack_general = max(max(slack_tradeoff_general_out_1[1]), max(slack_tradeoff_general_out_2[1]))
maxVio_slack_multiple = maxVio_slack_1501

minIC_delta = min(min(delta_tradeoff_out_1[0]), min(delta_tradeoff_out_2[0]))
minIC_slack_general = min(min(slack_tradeoff_general_out_1[0]), min(slack_tradeoff_general_out_2[0]))
minIC_slack_multiple = minIC_slack_1501


maxIC_out = max(maxIC_delta, maxIC_slack_general, maxIC_slack_multiple)
maxVio_out = max(maxVio_delta, maxVio_slack_general, maxVio_slack_multiple)
minIC_out = min(minIC_delta, minIC_slack_general, minIC_slack_multiple)

maxIC = max(maxIC_in, maxIC_out)
maxVio = max(maxVio_in, maxVio_out)
minIC = min(minIC_in, minIC_out)


#---------------------------------------------#
# create the plot creat plots for each test_type
#----------------#
fig, axes = plt.subplots(nrows=1, ncols=1, sharey=False, figsize=(4, 3))#two rows 6:8
# title_font = {'fontname': 'Times', 'fontsize': MEDIUM_SIZE}
# plt.suptitle(plot_title, **title_font, y=0.01)
fig.suptitle(plot_title, fontsize=BIGGER_SIZE, fontname = 'Times New Roman', y=0.01)

#if common value range
#2d
# for row in axes:
#     for ax in row:
#         ax.set_ylim([-Vio_gap, maxVio + Vio_gap])
#         ax.set_xlim([minIC - IC_gap, maxIC + IC_gap])
#1d      
#for ax in axes:
axes.set_ylim([-Vio_gap, maxVio + Vio_gap + extra_space])
axes.set_xlim([minIC - IC_gap, maxIC + IC_gap+0.5])
        
#legend
handles1 = [
    mlines.Line2D([], [], color='black', linewidth=1, linestyle='-', label = r'$\Omega\;\mathit{MDLS}$'),
    #mlines.Line2D([], [], color='blue', linewidth=1, linestyle='-', label=r'$\mathit{BRTSPTW(S)}$'), ##0069c0 #0045a5
    mlines.Line2D([], [], color='black', linewidth=1, linestyle=':', label=r'$\bar{\Omega}\;\mathit{MDLS}$'),
    mlines.Line2D([], [], color='red', linewidth=1, linestyle='-', label=r'$\Omega\;\mathit{MBBM}$'),
    mlines.Line2D([], [], color='red', linewidth=1, linestyle=':', label=r'$\bar{\Omega}\;\mathit{MBBM}$'),
]
labels1 = [r'$\Omega\;\mathit{MDLS}$', r'$\bar{\Omega}\;\mathit{MDLS}$', r'$\Omega\;\mathit{MBBM}$', r'$\bar{\Omega}\;\mathit{MBBM}$']

# handles2 = [
#     mlines.Line2D([], [], color='black', linewidth=1, linestyle='-', label=r'$\Omega$'),
#     mlines.Line2D([], [], color='black', linewidth=1, linestyle=':', label=r'$\bar{\Omega}$'),
# ]
# labels2 = [r'$\Omega$', r'$\bar{\Omega}$']

# fig.legend(handles1, labels1, loc='upper right', bbox_to_anchor=(1.11, 0.9))

#---------------------
# 1st: delta
# axes.step(delta_tradeoff_1[0], delta_tradeoff_1[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1)#, label = r'$\Omega\,\mathit{MDLS}$')
# axes.step(delta_tradeoff_2[0], delta_tradeoff_2[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='red', linewidth=1)#, label = r'$\Omega\,\mathit{MBBM}$')
# axes.step(delta_tradeoff_out_1[0], delta_tradeoff_out_1[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{MDLS}$')
# axes.step(delta_tradeoff_out_2[0], delta_tradeoff_out_2[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='red', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{MBBM}$')

# axes.set(xlabel=plot_label_IC, ylabel = plot_label_vio)
# legend = axes.legend(handles1, labels1, loc="upper right")
# legend.set_title(r'$\mathit{BRTSPTW(B)}$')
# title = legend.get_title()
# title.set_fontsize(8)
# axes.tick_params(pad=-3)

#---------------------
#2nd: slack_single
# axes.step(slack_tradeoff_general_1[0], slack_tradeoff_general_1[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1)#, label = r'$\Omega\,\mathit{MDLS}$')
# axes.step(slack_tradeoff_general_2[0], slack_tradeoff_general_2[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='red', linewidth=1)#, label = r'$\Omega\,\mathit{MBBM}$')
# axes.step(slack_tradeoff_general_out_1[0], slack_tradeoff_general_out_1[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{MDLS}$')
# axes.step(slack_tradeoff_general_out_2[0], slack_tradeoff_general_out_2[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='red', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{MBBM}$')

# axes.set(xlabel=plot_label_IC, ylabel = plot_label_vio)
# legend = axes.legend(handles1, labels1, loc="upper right")
# legend.set_title(r'$\mathit{BRTSPTW(S)}$')
# title = legend.get_title()
# title.set_fontsize(8)
# axes.tick_params(pad=-3)

#---------------------
#3rd: slack_multi
axes.step(slack_tradeoff_1501_1[0], slack_tradeoff_1501_1[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1)#, label = r'$\Omega\,\mathit{MDLS}$')
axes.step(slack_tradeoff_1501_2[0], slack_tradeoff_1501_2[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1)#, label = r'$\Omega\,\mathit{MBBM}$')
axes.step(slack_tradeoff_1501_out_1[0], slack_tradeoff_1501_out_1[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{MDLS}$')
axes.step(slack_tradeoff_1501_out_2[0], slack_tradeoff_1501_out_2[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{MBBM}$')

axes.set(xlabel=plot_label_IC, ylabel = plot_label_vio)
legend = axes.legend(handles1, labels1, loc="upper right")
legend.set_title(r'$two$-$phase$ $approach$')
title = legend.get_title()
title.set_fontsize(8)
axes.tick_params(pad=-3)


plt.tight_layout(pad=1.4, w_pad=1.5, h_pad=1.0)
plt.show();
fig.savefig(output_file + '.pdf', bbox_inches='tight')





