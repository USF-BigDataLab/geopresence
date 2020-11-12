#!../../py/env/bin/python

import pandas as pd
import glob
import matplotlib.pyplot as plt

cols = [x for x in range(10000, 100001, 10000)]

def create_frame(path):
    all_files = glob.glob(path + "/*.txt")
    li = []

    for filename in all_files:
        df = pd.read_csv(filename, header=None, names=cols)
        li.append(df)

    df = pd.concat(li, axis=0, ignore_index=True, sort=False).mean().to_frame()
    df.reset_index(inplace=True)
    df.columns = ['insertions','time']
    return df


rtree_frame = create_frame('c_rtree_out')
geode_frame_15 = create_frame('c_out_15bit')
geode_frame_20 = create_frame('c_out_20bit')
geode_frame_25 = create_frame('c_out_25bit')

ax = rtree_frame.plot(color='red', linestyle='-', marker='o', x='insertions',y='time', label='RTree')

ax = geode_frame_15.plot(color='blue', linestyle='-', marker='x', x='insertions', y='time', label='GEODE 15 bit', ax=ax)

ax = geode_frame_20.plot(color='green', linestyle='-', marker='+', x='insertions', y='time', label='GEODE 20 bit', ax=ax)


ax = geode_frame_25.plot(color='purple', linestyle='-', marker='1', x='insertions', y='time', label='GEODE 25 bit', ax=ax)

plt.legend(shadow=True, fancybox=True, loc='upper left')
plt.xlabel("# Data points in structure before query")
plt.ylabel("Mean query time in milliseconds")

plt.savefig('../../figures/query_scatter.pdf', bbox_inches='tight')
