import mpl_toolkits.mplot3d as a3
import matplotlib.pyplot as plt
import numpy as np
import sys
import os


if __name__ == '__main__':
    directory = f'demo/{sys.argv[1]}'
    for file in os.listdir(directory):
        if file.endswith('.txt'):
            if file in {'debug.txt', 'dna-stats.txt'}:
                continue
            if 'alleles' in file: # 3D histogram
                data = open(f"{directory}/{file}", 'r').read().strip().split('\n')
                # Data are in the form: "allele:value,allele:value,..." for each line
                data = [line.split(',') for line in data]
                # Remove the last element of each line, which is an empty string
                data = [line[:-1] for line in data]
                # Data are in the form: [["allele:value", "allele:value", ...], ...]
                labels: list[str] = []
                for i in range(len(data)): # iterate over each line
                    labels.append([pair.split(':')[0] for pair in data[i]])
                    data[i] = [int(pair.split(':')[1]) for pair in data[i]]
                # Add padding to the data so that it is a square matrix
                max_len_line = 0
                for index, line in enumerate(data):
                    if len(line) > len(data[max_len_line]):
                        max_len_line = index
                # Padding with zeros may be needed on the left or on the right
                for index, line in enumerate(data):
                    if len(line) < len(data[max_len_line]):
                        left_padding_needed = labels[max_len_line].index(labels[index][0])
                        right_padding_needed = len(data[max_len_line]) - len(data[index]) - left_padding_needed
                        data[index] = [0] * left_padding_needed + data[index] + [0] * right_padding_needed
                # Data are in the form: [["value", "value", ...], ...]
                fig = plt.figure()
                ax = fig.add_subplot(111, projection='3d')
                data_array = np.array(data)
                z_data = [10*i for i in range(len(data_array))]
                x_data, y_data = np.meshgrid(
                    np.arange(data_array.shape[1]),
                    np.arange(data_array.shape[0])
                )
                x_data = x_data.flatten()
                y_data = y_data.flatten()
                z_data = np.zeros(data_array.shape).flatten()
                ax.bar3d(x_data, y_data, z_data, 1, 1, data_array.flatten())
                ax.set_xticks(np.arange(data_array.shape[1]))
                ax.set_xticklabels(labels[max_len_line], fontsize=8)
                plt.title(file[:-4])
                plt.xlabel('Alleles')
                plt.ylabel('Time')
                # plt.set_zlabel('Frequency')
                plt.savefig(f'{directory}/{file[:-4]}.png')
                plt.show()
                continue
            # 2D graphic
            ys = list(map(float, open(f'{directory}/{file}', 'r').read().strip().split('\n')))
            xs = [10 * i for i in range(len(ys))]

            plt.plot(xs, ys)
            plt.title(file[:-4])
            plt.xlabel('Time')
            plt.ylabel(f"{file[:-4]} average")
            plt.savefig(f'{directory}/{file[:-4]}.png')
            plt.show()
            plt.close()