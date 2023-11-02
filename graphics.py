import matplotlib.pyplot as plt
import sys
import os


if __name__ == '__main__':
    directory = f'demo/{sys.argv[1]}'
    for file in os.listdir(directory):
        if file.endswith('.txt'):
            if file in {'debug.txt', 'dna-stats.txt'}:
                continue
            ys = list(map(float, open(f'{directory}/{file}', 'r').read().strip().split('\n')))
            xs = [10 * i for i in range(len(ys))]

            plt.plot(xs, ys)
            plt.title(file[:-4])
            plt.xlabel('Time')
            plt.ylabel(f"{file[:-4]} average")
            plt.savefig(f'{directory}/{file[:-4]}.png')
            plt.show()
            plt.close()