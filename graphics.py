import matplotlib.pyplot as plt
import numpy as np
import pprint
import sys
import os


def allele_sort(allele: str) -> int:
    if allele.isnumeric():
        return int(allele)
    return {
        "slow": 1, "medium": 2, "fast": 3, "weak": 1, "moderate": 2, "strong": 3,
        "low": 1, "median": 3, "high": 5, "aggressive": 1, "passive": 2, "neutral": 3,
        "short": 1, "average": 2, "long": 3, "near": 1, "clear": 3, "far": 5,
        "photoautotroph": -2, "anaerobic": 0, "aerobic": 1
    }.get(allele, 0)


if __name__ == '__main__':
    directory = f'demo/{sys.argv[1]}'
    for file in os.listdir(directory):
        if file.endswith('.txt'):
            if file in {'debug.txt', 'dna-stats.txt'}:
                continue
            if file == 'atmosphere-stats.txt': # 2D graphic
                lines = open(f"{directory}/{file}", 'r').read().strip().split('\n')
                oxygen = [int(line.split(',')[0]) for line in lines]
                carbon = [int(line.split(',')[1]) for line in lines]
                xs = [10 * i for i in range(len(oxygen))]
                plt.plot(xs, oxygen, label='Oxygen')
                plt.plot(xs, carbon, label='Carbon')
                plt.title(file[:-4])
                plt.xlabel('Time')
                plt.ylabel('Atmosphere composition')
                plt.legend()
                plt.savefig(f'{directory}/{file[:-4]}.png')
                plt.show()
                continue
            if 'alleles' in file: # 3D histogram
                data = open(f"{directory}/{file}", 'r').read().strip().split('\n')
                # Data are in the form: "allele:value,allele:value,..." for each line
                data = [line.split(',') for line in data]
                # Remove the last element of each line, which is an empty string
                data = [line[:-1] for line in data]
                # Remove empty lines (may be present when "starklag --load" since v0.7)
                data = [data[i] for i in range(len(data)) if data[i] != []]
                # Data are in the form: [["allele:value", "allele:value", ...], ...]
                labels: list[str] = []
                for i in range(len(data)): # iterate over each line
                    labels.append([pair.split(':')[0] for pair in data[i]])
                    data[i] = [int(pair.split(':')[1]) for pair in data[i]]
                print(f"{labels=}")
                # Add padding to the data so that it is a square matrix
                all_possible_labels = []
                for line in labels:
                    all_possible_labels += line
                all_possible_labels = sorted(list(set(all_possible_labels)), key=allele_sort)
                print(f"{all_possible_labels=}")
                # Padding with zeros may be needed on the left or on the right
                for index, line in enumerate(data):
                    if len(line) < len(all_possible_labels):
                        print(f"Padding line {index} with {len(all_possible_labels) - len(data[index])} zeros, line is {data[index]} and its labels are {labels[index]}")
                        left_padding_needed = all_possible_labels.index(labels[index][0])
                        right_padding_needed = len(all_possible_labels) - len(data[index]) - left_padding_needed
                        data[index] = [0] * left_padding_needed + data[index] + [0] * right_padding_needed
                        print(f"Line {index} is now {data[index]}")
                # Data are in the form: [["value", "value", ...], ...]
                fig = plt.figure()
                ax = fig.add_subplot(111, projection='3d')
                pprint.pprint(data)
                data_array = np.array((*data,), dtype=np.int64)
                print(data_array)
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
                ax.set_xticklabels(all_possible_labels, fontsize=8)
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