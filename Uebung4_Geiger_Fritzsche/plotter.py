import numpy as np
import pylab as pl
import datetime

name = None
times = []
flops = []
sizes = []

data = open("./data/meta.txt","r")
i = 0;
for line in data:
    if i == 0:
        tmp = line.split()
        for str in tmp:
            sizes.append(int(str))
    i += 1

i = 0;
data = open("./data/dataSimple.txt","r")
for line in data:
    if i == 0:
        name = line
    elif i == 1:
        times = line.split()
    elif i == 2:
        flops = line.split()
    i += 1

pl.plot ( sizes,times,'.-',label='Times' )

i = 0;
data = open("./data/dataBlocked.txt","r")
for line in data:
    if i == 0:
        name = line
    elif i == 1:
        times = line.split()
    elif i == 2:
        flops = line.split()
    i += 1

pl.plot ( sizes,times,'.-',label='Times' )


i = 0;
data = open("./data/dataBlas.txt","r")
for line in data:
    if i == 0:
        name = line
    elif i == 1:
        times = line.split()
    elif i == 2:
        flops = line.split()
    i += 1

pl.plot ( sizes,times,'.-',label='Times' )

i = 0;
data = open("./data/dataBlockedSimd.txt","r")
for line in data:
    if i == 0:
        name = line
    elif i == 1:
        times = line.split()
    elif i == 2:
        flops = line.split()
    i += 1

pl.plot ( sizes,times,'.-',label='Times' )

i = 0;
data = open("./data/dataSimpleSimd.txt","r")
for line in data:
    if i == 0:
        name = line
    elif i == 1:
        times = line.split()
    elif i == 2:
        flops = line.split()
    i += 1

pl.plot ( sizes,times,'.-',label='Times' )


# pl.plot ( np.arange(0,10),[12,5,33,2,4,5,3,3,22,10],'o-',label='sample2' )

pl.xlabel('Matrix Size [N x N]')
pl.ylabel('Needed Time [s]')
ax = pl.subplot(111)
ax.set_xticks(sizes)

#pl.title('my sample graphs')
pl.legend(('Simple','Blocked','BLAS','BlockedSimd','SimpleSimd'))
#fig = pl.figure()
#fig.autofmt_xdate()
pl.show()
#savefig("sampleg.png",dpi=(640/8))

"""
index = np.arange(len(values))

ax = pl.subplot(111)
width=0.8
ax.bar(index, values, width=width)
ax.set_xticks(np.arange(len(dates)) + width/2)
ax.set_xticklabels(names, rotation=90)

pl.show()
"""


