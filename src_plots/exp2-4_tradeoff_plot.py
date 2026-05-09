# -*- coding: utf-8 -*-
"""
Created on Fri May 26 16:39:07 2023

@author: mengdie
"""

import pandas as pd
import math
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib import rc
import matplotlib.lines as mlines


def scientific_to_float(value):
    try:
        return float(value)
    except (ValueError, TypeError):
        return value
    
def contains_only_nan(lst):
    return all(math.isnan(x) for x in lst)

def remove_nan_and_empty_elements(lst):
    #return [x for x in lst if not math.isnan(x)]
    return [x for x in lst if x != ' ' and not math.isnan(x)]


def read_and_clean_data(input_file):
    # Read the CSV file
    df = pd.read_csv(input_file, header=None)
    #converters = {col: scientific_to_float for col in df.columns[3:]}
    #df = pd.read_csv(input_file, header = None, converters=converters)
    #df = pd.read_csv(input_file, header = None, dtype={col: float for col in range(3, len(df.columns))})
    #print(df)
    
    # Store data of each row (excluding the header) into a list
    data_list = []
    for _, row in df.iterrows():
        data_list.append(row.values[3:])
    
    # clean the data
    data_list_new = []
    for data in data_list:
        #print(data)
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
#method1 = 'MDLS'
method = 'MBBM'
instance = 'n60w120.001'
distribution = 'discrete'
measure_IC = 'IC'
measures_vio = ['Vio', 'Nvio', 'Wvio', 'Wnvio']

# set the labels of the plots
plot_label_IC_avg = r'$C_{avg}\,(\%)$'
plot_label_IC_wstV = r'$C^{L}_{wst}\,(\%)$'
plot_lable_IC_wstN = r'$C^{N}_{wst}\,(\%)$'
plot_label_vio = r'$L_{avg}$'
plot_label_nvio = r'$N_{avg}$'
plot_label_wvio = r'$L_{wst}$'
plot_label_wnvio = r'$N_{wst}$'
    
plot_title = instance
if distribution == 'discrete':
    plot_title = plot_title + ' (two-point)'
else:
    plot_title = plot_title + ' (uniform)'

#set the gaps in the plots
IC_gap = 0.5
Vio_gap = 0.5
Nvio_gap = 0.05
space_for_legend = 2


# set the input and output path
file_path = '../visualization/2_solution_evaluation_tradeoff/'
input_file_path = file_path + 'measures/plots_info/'
output_file_path = file_path + 'plots/model_comparison/'

# read in the four files for same instance with MBBM
filename1 = 'tradeoff_' + method + '_' + instance + '_' + distribution + '_' + measure_IC + '-' + measures_vio[0] + '.csv' #file vio
inputfile1 = input_file_path + filename1

filename2 = 'tradeoff_' + method + '_' + instance + '_' + distribution + '_' + measure_IC + '-' + measures_vio[1] + '.csv' #file nvio
inputfile2 = input_file_path + filename2

filename3 = 'tradeoff_' + method + '_' + instance + '_' + distribution + '_' + measure_IC + '-' + measures_vio[2] + '.csv' #file wvio
inputfile3 = input_file_path + filename3

filename4 = 'tradeoff_' + method + '_' + instance + '_' + distribution + '_' + measure_IC + '-' + measures_vio[3] + '.csv' #file wnvio
inputfile4 = input_file_path + filename4

#set the output info 
filename_out = instance + '_' + distribution + '.pdf'
output_file = output_file_path + filename_out

#---------------------------------------------#
# data set 1 -- IC-Vio
data_list1 = read_and_clean_data(inputfile1)
# for data in data_list1:
#     print(data)

# restore the data into corresponding lists
curr_row_in_list = 0
# in-sample performance
curr_row_in_list, delta_vio = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_gen_vio = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_1501_vio = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list) 
    
#out-sample performance
curr_row_in_list, delta_vio_out = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_gen_vio_out = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)
curr_row_in_list, slack_1501_vio_out = sort_tradeoff_with_objectives(2, data_list1, curr_row_in_list)

# data set 2 -- IC-Nvio
data_list2 = read_and_clean_data(inputfile2)

curr_row_in_list = 0
curr_row_in_list, delta_nvio = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_gen_nvio = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_1501_nvio = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
    
curr_row_in_list, delta_nvio_out = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_gen_nvio_out = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)
curr_row_in_list, slack_1501_nvio_out = sort_tradeoff_with_objectives(2, data_list2, curr_row_in_list)

# data set 3 -- IC-Wvio
data_list3 = read_and_clean_data(inputfile3)

curr_row_in_list = 0
curr_row_in_list, delta_wvio = sort_tradeoff_with_objectives(2, data_list3, curr_row_in_list)
curr_row_in_list, slack_gen_wvio = sort_tradeoff_with_objectives(2, data_list3, curr_row_in_list)
curr_row_in_list, slack_1501_wvio = sort_tradeoff_with_objectives(2, data_list3, curr_row_in_list)
    
curr_row_in_list, delta_wvio_out = sort_tradeoff_with_objectives(2, data_list3, curr_row_in_list)
curr_row_in_list, slack_gen_wvio_out = sort_tradeoff_with_objectives(2, data_list3, curr_row_in_list)
curr_row_in_list, slack_1501_wvio_out = sort_tradeoff_with_objectives(2, data_list3, curr_row_in_list)

# data set 4 -- IC-Wnvio
data_list4 = read_and_clean_data(inputfile4)

curr_row_in_list = 0
curr_row_in_list, delta_wnvio = sort_tradeoff_with_objectives(2, data_list4, curr_row_in_list)
curr_row_in_list, slack_gen_wnvio = sort_tradeoff_with_objectives(2, data_list4, curr_row_in_list)
curr_row_in_list, slack_1501_wnvio = sort_tradeoff_with_objectives(2, data_list4, curr_row_in_list)
    
curr_row_in_list, delta_wnvio_out = sort_tradeoff_with_objectives(2, data_list4, curr_row_in_list)
curr_row_in_list, slack_gen_wnvio_out = sort_tradeoff_with_objectives(2, data_list4, curr_row_in_list)
curr_row_in_list, slack_1501_wnvio_out = sort_tradeoff_with_objectives(2, data_list4, curr_row_in_list)

#--------------------------------------------#
# Create a figure and subplots
#----------------------#
# setting of the plots
rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
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
plt.rc('legend', fontsize=6)    # legend fontsize
plt.rc('figure', titlesize=BIGGER_SIZE)  # fontsize of the figure title

#---------------------#
##calc maximum / minimum for scaling the axis: should be separated for each subplot
#IC-Vio
maxIC_vio = max(max(delta_vio[0]), max(slack_gen_vio[0]), max(slack_1501_vio[0]), max(delta_vio_out[0]), max(slack_gen_vio_out[0]), max(slack_1501_vio_out[0]))
minIC_vio = min(min(delta_vio[0]), min(slack_gen_vio[0]), min(slack_1501_vio[0]), min(delta_vio_out[0]), min(slack_gen_vio_out[0]), min(slack_1501_vio_out[0]))
maxVio = max(max(delta_vio[1]), max(slack_gen_vio[1]), max(slack_1501_vio[1]), max(delta_vio_out[1]), max(slack_gen_vio_out[1]), max(slack_1501_vio_out[1]))

#IC-Nvio
maxIC_nvio = max(max(delta_nvio[0]), max(slack_gen_nvio[0]), max(slack_1501_nvio[0]), max(delta_nvio_out[0]), max(slack_gen_nvio_out[0]), max(slack_1501_nvio_out[0]))
minIC_nvio = min(min(delta_nvio[0]), min(slack_gen_nvio[0]), min(slack_1501_nvio[0]), min(delta_nvio_out[0]), min(slack_gen_nvio_out[0]), min(slack_1501_nvio_out[0]))
maxNvio = max(max(delta_nvio[1]), max(slack_gen_nvio[1]), max(slack_1501_nvio[1]), max(delta_nvio_out[1]), max(slack_gen_nvio_out[1]), max(slack_1501_nvio_out[1]))

#IC-Wvio
maxIC_wvio = max(max(delta_wvio[0]), max(slack_gen_wvio[0]), max(slack_1501_wvio[0]), max(delta_wvio_out[0]), max(slack_gen_wvio_out[0]), max(slack_1501_wvio_out[0]))
minIC_wvio = min(min(delta_wvio[0]), min(slack_gen_wvio[0]), min(slack_1501_wvio[0]), min(delta_wvio_out[0]), min(slack_gen_wvio_out[0]), min(slack_1501_wvio_out[0]))
maxWvio = max(max(delta_wvio[1]), max(slack_gen_wvio[1]), max(slack_1501_wvio[1]), max(delta_wvio_out[1]), max(slack_gen_wvio_out[1]), max(slack_1501_wvio_out[1]))

#IC-Wnio
maxIC_wnvio = max(max(delta_wnvio[0]), max(slack_gen_wnvio[0]), max(slack_1501_wnvio[0]), max(delta_wnvio_out[0]), max(slack_gen_wnvio_out[0]), max(slack_1501_wnvio_out[0]))
minIC_wnvio = min(min(delta_wnvio[0]), min(slack_gen_wnvio[0]), min(slack_1501_wnvio[0]), min(delta_wnvio_out[0]), min(slack_gen_wnvio_out[0]), min(slack_1501_wnvio_out[0]))
maxWnvio = max(max(delta_wnvio[1]), max(slack_gen_wnvio[1]), max(slack_1501_wnvio[1]), max(delta_wnvio_out[1]), max(slack_gen_wnvio_out[1]), max(slack_1501_wnvio_out[1]))

#---------------------------------------------#
# create the plot for each instance
#----------------#
fig, axes = plt.subplots(nrows=1, ncols=4, sharey=False, figsize=(12, 4))
#fig.suptitle(plot_title, fontsize=MEDIUM_SIZE, y=0.01)

#legend
#separate
# handles1 = [
#     mlines.Line2D([], [], color='black', linewidth=1, linestyle='-', label = r'$\mathit{BRTSPTW(B)}$'),
#     mlines.Line2D([], [], color='blue', linewidth=1, linestyle='-', label=r'$\mathit{BRTSPTW(S)}$'), ##0069c0 #0045a5
#     mlines.Line2D([], [], color='red', linewidth=1, linestyle='-', label=r'$\mathit{BRTSPTW(B\colon \!S)}$'),
#     #mlines.Line2D([], [], color='black', linewidth=1, linestyle=':', label=r'$\Omega$, $\bar{\Omega}$'),
# ]
# labels1 = [r'$\mathit{BRTSPTW(B)}$', r'$\mathit{BRTSPTW(S)}$', r'$\mathit{BRTSPTW(B\colon \!S)}$']

# handles2 = [
#     mlines.Line2D([], [], color='black', linewidth=1, linestyle='-', label=r'$\Omega$'),
#     mlines.Line2D([], [], color='black', linewidth=1, linestyle=':', label=r'$\bar{\Omega}$'),
# ]
# labels2 = [r'$\Omega$', r'$\bar{\Omega}$']

# fig.legend(handles1, labels1, loc='upper right', bbox_to_anchor=(1.11, 0.975)) #1.11 for uni2001, dis1201, dis2001, 1 for uni1201
# for ax in axes.flat:
#     legend1 = ax.legend(handles1, labels1, loc='upper right')#, bbox_to_anchor=(1, 0.93))
#     legend2 = ax.legend(handles2, labels2, ncols = 2, loc='upper right')
#     #ax.add_artist(legend1)
#     #ax.add_artist(legend2)

#together
handles2 = [
    mlines.Line2D([], [], color='black', linewidth=1, linestyle='-', label = r'$\mathit{BRTSPTW(B)}$'),
    mlines.Line2D([], [], color='blue', linewidth=1, linestyle='-', label=r'$\mathit{BRTSPTW(S)}$'), ##0069c0 #0045a5
    mlines.Line2D([], [], color='red', linewidth=1, linestyle='-', label=r'$two$-$phase$ $approach$'),
    mlines.Line2D([], [], color='black', linewidth=1, linestyle='-', label=r'$\Omega$'), 
    #mlines.Line2D([], [], color='blue', linewidth=1, linestyle=':', label=r'$\bar{\Omega}\,\mathit{BRTSPTW(S)}$'), ##0069c0 #0045a5  
    mlines.Line2D([], [], color='black', linewidth=1, linestyle=':', label=r'$\bar{\Omega}$'),
    
]
labels2 = [r'$\mathit{BRTSPTW(B)}$', r'$\mathit{BRTSPTW(S)}$', r'$two$-$phase$ $approach$', r'$\Omega$', r'$\bar{\Omega}$']

#----------------#
# 1st: IC-Vio
axes[0].set_ylim([-Vio_gap+0.25, maxVio + Vio_gap]) #0.1, 0.2
axes[0].set_xlim([minIC_vio - IC_gap, maxIC_vio + IC_gap + space_for_legend])
axes[0].step(delta_vio[0], delta_vio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B)}$')
axes[0].step(slack_gen_vio[0], slack_gen_vio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='blue', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(S)}$')
axes[0].step(slack_1501_vio[0], slack_1501_vio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B:S)}$')

axes[0].step(delta_vio_out[0], delta_vio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B)}$')
axes[0].step(slack_gen_vio_out[0], slack_gen_vio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='b', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(S)}$')
axes[0].step(slack_1501_vio_out[0], slack_1501_vio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B:S)}$')

axes[0].set(xlabel=plot_label_IC_avg, ylabel = plot_label_vio)
legend = axes[0].legend(handles2, labels2, loc='upper right')
legend.set_title(r'$\mathit{Average}$')
title = legend.get_title()
title.set_fontsize(6)
axes[0].tick_params(pad=-2)


#2nd: IC-Nvio
axes[1].set_ylim([-Nvio_gap+0.025, maxNvio + Nvio_gap]) #0.025, 0.05
axes[1].set_xlim([minIC_nvio - IC_gap, maxIC_nvio + IC_gap  + space_for_legend])
axes[1].step(delta_nvio[0], delta_nvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B)}$')
axes[1].step(slack_gen_nvio[0], slack_gen_nvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='b', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(S)}$')
axes[1].step(slack_1501_nvio[0], slack_1501_nvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B:S)}$')

axes[1].step(delta_nvio_out[0], delta_nvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B)}$')
axes[1].step(slack_gen_nvio_out[0], slack_gen_nvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='b', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(S)}$')
axes[1].step(slack_1501_nvio_out[0], slack_1501_nvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B:S)}$')


axes[1].set(xlabel = plot_label_IC_avg, ylabel = plot_label_nvio)
legend = axes[1].legend(handles2, labels2, loc='upper right')
legend.set_title(r'$\mathit{Average}$')
title = legend.get_title()
title.set_fontsize(6)
axes[1].tick_params(pad=-2)


#3rd: IC-Wvio
axes[2].set_ylim([-Vio_gap, maxWvio + Vio_gap])
axes[2].set_xlim([minIC_wvio - IC_gap, maxIC_wvio + IC_gap  + space_for_legend])
axes[2].step(delta_wvio[0], delta_wvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B)}$')
axes[2].step(slack_gen_wvio[0], slack_gen_wvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='b', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(S)}$')
axes[2].step(slack_1501_wvio[0], slack_1501_wvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B:S)}$')

axes[2].step(delta_wvio_out[0], delta_wvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B)}$')
axes[2].step(slack_gen_wvio_out[0], slack_gen_wvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='b', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(S)}$')
axes[2].step(slack_1501_wvio_out[0], slack_1501_wvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B:S)}$')

axes[2].set(xlabel = plot_label_IC_wstV, ylabel = plot_label_wvio)
legend = axes[2].legend(handles2, labels2, loc='lower left')
#legend = axes[2].legend(handles2, labels2, loc='upper right')
legend.set_title(r'$\mathit{Worst}\,5\%$')
title = legend.get_title()
title.set_fontsize(6)
axes[2].tick_params(pad=-2)


#4th: IC-Wnvio
axes[3].set_ylim([-Nvio_gap, maxWnvio + Nvio_gap])
axes[3].set_xlim([minIC_wnvio - IC_gap, maxIC_wnvio + IC_gap  + space_for_legend])
axes[3].step(delta_wnvio[0], delta_wnvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B)}$')
axes[3].step(slack_gen_wnvio[0], slack_gen_wnvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='b', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(S)}$')
axes[3].step(slack_1501_wnvio[0], slack_1501_wnvio[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1)#, label = r'$\Omega\,\mathit{BRTSPTW(B:S)}$')

axes[3].step(delta_wnvio_out[0], delta_wnvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='black', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B)}$')
axes[3].step(slack_gen_wnvio_out[0], slack_gen_wnvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='b', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(S)}$')
axes[3].step(slack_1501_wnvio_out[0], slack_1501_wnvio_out[1], marker='o', where='post', markerfacecolor='black', markersize=0, color='r', linewidth=1, linestyle = ':')#, label = r'$\bar{\Omega}\,\mathit{BRTSPTW(B:S)}$')

axes[3].set(xlabel = plot_lable_IC_wstN, ylabel = plot_label_wnvio)
legend = axes[3].legend(handles2, labels2, loc='lower left')
#legend = axes[3].legend(handles2, labels2, loc='upper right')
legend.set_title(r'$\mathit{Worst}\,5\%$')
title = legend.get_title()
title.set_fontsize(6)
axes[3].tick_params(pad=-2)


plt.tight_layout(pad=1.4, w_pad=1.5, h_pad=1.0)
plt.show();
fig.savefig(output_file, bbox_inches='tight')


