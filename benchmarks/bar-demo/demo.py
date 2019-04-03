import numpy as np
import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
from matplotlib.ticker import FuncFormatter

from matplotlib import rc
rc('font',**{'family':'sans-serif','sans-serif':['Arial']})
from matplotlib import rcParams
rcParams['figure.figsize'] = [16.0, 8.0]

plt.clf()

fig = plt.figure(1)
def thousands(x, pos):
    return '%dk' % (x*1e-3)

ax3 = plt.gca()
formatter = FuncFormatter(thousands)

ax3.set_title('(c) Aggregate Query Throughput', fontsize=16)
ax3.set_xlabel('Query Scope', fontsize=14)
ax3.set_ylabel('Queries/s', fontsize=14)
ax3.yaxis.set_major_formatter(formatter)

c2 = '#f7d7d4'
ec2 = '#da2b17'

c1 = '#9bceef'
ec1 = '#1f6098'

shift = 0.02

ax3.bar(1.25, 184775.28, width=.5, color=c1, edgecolor=ec1, lw=1.25, ecolor=ec1,
        label='Fog')
ax3.bar(1.75 + shift, 121031.03, width=.5, color=c2, edgecolor=ec2, lw=1.25,
        ecolor=ec2, hatch='/', label='Cloud', alpha=.8)

ax3.bar(2.5, 70329.67, width=.5, color=c1, edgecolor=ec1, lw=1.25, ecolor=ec1)
ax3.bar(3 + shift, 90649.76, width=.5, color=c2, edgecolor=ec2, lw=1.25,
        ecolor=ec2, hatch='/', alpha=.8)

ax3.bar(3.75, 46733.86, width=.5, color=c1, edgecolor=ec1, lw=1.25, ecolor=ec1)
ax3.bar(4.25 + shift, 71343.85, width=.5, color=c2, edgecolor=ec2, lw=1.25,
        ecolor=ec2, hatch='/', alpha=.8)

ax3.set_xticks([1.75, 3.0, 4.25])
ax3.set_xticklabels(['1 - 12 hours', '12 - 24 hours', '24 - 36 hours'])

ax3.legend(loc='upper right', fancybox=True, shadow=True, fontsize=14, numpoints=1)

plt.savefig('evaluation.pdf', bbox_inches='tight')
