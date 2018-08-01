import numpy as np
import matplotlib.pyplot as plt
plt.rcParams['figure.figsize'] = (10,6)
plt.rcParams.update({'font.size': 16})
plt.style.use('seaborn-whitegrid')


x1 = []
y1 = []
with open('./out/wrap256.txt', 'r') as f:
    params = next(f).split()
    print(params)
    L = int(params[0])
    n_perc = float(params[1])
    for line in f:
        vals = line.split()
        x1.append( float(vals[0]) )
        y1.append( float(vals[1]) )

plt.plot(x1,y1)
plt.legend([f'L={L}'])
