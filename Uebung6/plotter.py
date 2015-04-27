import numpy as np
import matplotlib as ml
import matplotlib.pyplot as plt

A = []
N = []
data = open("./bla.txt","r")

for line in data:
    A.append(line.split())

for line in A:
    numbers = []
    for word in line:
        numbers.append(float(word))
    N.append(numbers)

X = np.array(N)
X = X.T

fig = plt.figure(figsize=(6, 3.2))

ax = fig.add_subplot(111)
ax.set_title('Bierfass')
plt.imshow(X)
ax.set_aspect('equal')

ax.xaxis.set_ticks(range(0,len(A),1))
ax.yaxis.set_ticks(range(0,len(A),1))

plt.tick_params(axis='both', which='major', labelsize=8)
ax.grid(b=True, which='major', color='g', linestyle='-')

#cax = fig.add_axes([0.12, 0.1, 0.78, 0.8])
#cax.get_xaxis().set_visible(False)
#cax.get_yaxis().set_visible(False)
#cax.patch.set_alpha(0)
#cax.set_frame_on(False)
plt.colorbar(orientation='vertical')
plt.show()
