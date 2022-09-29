import csv
import pandas
# df = pandas.read_csv('drone_local_position_unformated.csv')
# print(df['pose'][0])
# print(df)


# Get .csv file and put it into x,y,z list


def printcsv():
    DELIMITER = ',""'
    # x = []
    # y = []
    # z = []
    csv_file = open('drone_local_position_unformated.csv', "rb")
    csv_reader = csv.reader((line.replace(',""', '|') for line in csv_file), delimiter='|')
    line_count = 0
    for row in csv_reader:
        if line_count == 0:
            print(f'Column names are {",".join(row)}')
            line_count += 1
        else:
            print(f'\t{row[0]}.')
            break
        
        # for row in csv_reader:
        #     x.append(row[0])
        #     y.append(row[1])
        #     z.append(row[2])
        # x = list(map(float, x))
        # y = list(map(float, y))
        # z = list(map(float, z))
        # return x, y, z
        
printcsv()


#%%
f = open("drone_local_position_unformated.csv", 'r')
# %%
