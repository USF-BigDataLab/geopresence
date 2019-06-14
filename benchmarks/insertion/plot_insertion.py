#!../../py/env/bin/python

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

from matplotlib import rc
rc('font',**{'family':'sans-serif','sans-serif':['Arial']})

plt.ion()
plt.clf()

sns.set()
sns.set(font_scale=1.25)
sns.set_style("whitegrid")

mattplot = ["#b798d8", "#c3e7f4", "#95a5a6", "#e74c3c", "#34495e", "#2ecc71"]
sns.set_palette(mattplot)

cols = ['Average Insertion Throughput (Records/s)', 'Platform', 'Type']
geohashes = 262792

single_j = geohashes / pd.read_csv('java_out_20190610-1/wall_clock.txt',
        header=None, names=cols)
single_j['Platform'] = 'GeoGrid'
single_j['Type'] = 'NOAA One Pass'


single_c = geohashes / pd.read_csv('c_out_20190606-1/wall_clock.txt',
        header=None, names=cols)
single_c['Platform'] = 'GEODE'
single_c['Type'] = 'NOAA One Pass'

double_j = geohashes * 2 / pd.read_csv('java_2x_out_20190610-1/wall_clock.txt',
        header=None, names=cols)
double_j['Platform'] = 'GeoGrid'
double_j['Type'] = 'NOAA Two Pass'

double_c = geohashes * 2 / pd.read_csv('c_2x_out_20190607-1/wall_clock.txt',
        header=None, names=cols)
double_c['Platform'] = 'GEODE'
double_c['Type'] = 'NOAA Two Pass'

random_j = 1000000 / pd.read_csv('java_r_out_20190610-1/wall_clock.txt',
        header=None, names=cols)
random_j['Platform'] = 'GeoGrid'
random_j['Type'] = 'Random Location'


random_c = 100000 / pd.read_csv('c_r_out_20190608-1/wall_clock.txt',
        header=None, names=cols)
random_c['Platform'] = 'GEODE'
random_c['Type'] = 'Random Location'


data = pd.concat(
        (single_c, single_j, double_c, double_j, random_c, random_j)
    ).sort_values(['Platform', 'Type'], ascending=[False, True])

plt.suptitle('Insertion Throughput', fontsize=16)

ax = sns.barplot(x=cols[2], y=cols[0], data=data, hue=cols[1], ci=None)
ax.set_xlabel('')

for i, bar in enumerate(ax.patches):
    if i < len(ax.patches) / 2 :
        bar.set_hatch('/')
        bar.set_edgecolor('#652d90')
    else:
        bar.set_edgecolor('#26a9e0')

plt.legend(shadow=True, fancybox=True, loc='upper left')

plt.savefig('../../figures/insert.pdf', bbox_inches='tight')
