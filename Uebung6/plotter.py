import numpy as np
import matplotlib as ml
import matplotlib.pyplot as plt



for c in range(0,199):
    A = []
    N = []
    fi = "./" + str(c) + ".txt"
    print fi
    data = open(fi,"r")

    for line in data:
        A.append(line.split())

    for line in A:
        numbers = []
        for word in line:
            numbers.append(float(word))
        N.append(numbers)


    X = np.array(N)
    X = X.T
    
    counter = 0.0
    average = 0.0;
    for line in X:
        for word in line:
            average += float(word)
            counter += 1.0
    average = average / counter
    print average


    fig = plt.figure(figsize=(9, 7))

    ax = fig.add_subplot(111)
    ax.set_title('Bierfass')
    plt.imshow(X)
    ax.set_aspect('equal')

    ax.xaxis.set_ticks(range(0,len(A),1))
    ax.yaxis.set_ticks(range(0,len(A),1))
    ax.annotate('Average Temp: ' + str(average), xy=(2, 1), xytext=(0, 28.5),)

    plt.tick_params(axis='both', which='major', labelsize=8)
    ax.grid(b=True, which='major', color='g', linestyle='-')

    #cax = fig.add_axes([0.12, 0.1, 0.78, 0.8])
    #cax.get_xaxis().set_visible(False)
    #cax.get_yaxis().set_visible(False)
    #cax.patch.set_alpha(0)
    #cax.set_frame_on(False)
    plt.colorbar(orientation='vertical')
    #plt.show()
    name = "test"
    if (c < 10):
        name += "0"
    if (c < 100):
        name += "0"
    name += str(c)
    name += ".png"
    plt.savefig(name, dpi = 280, figsize=(34,24))
