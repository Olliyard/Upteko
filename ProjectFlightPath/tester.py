# import csv

# #Get .csv file and put it into x,y,z list
# def printcsv():
#     time = []
#     y = []
#     z = []
#     with open('drone_local_position_unformated.csv') as csv_file:
#         csv_reader = csv.reader((line.replace(',""', '%') for line in csv_file), delimiter='%', quotechar= '%', doublequote=False)
#         line_count = 0
#         for row in csv_reader:
#             if line_count == 0:
#                   print(f'Column names are {", ".join(row)}')
#                   line_count += 1
#             else:
#                   print(f'\t{row[0]} ----------------------- {row[1]} ----------------------- {row[2]}')
#                   time.append(row[0])
#                   line_count += 1
#             #y.append(row[1])
#             #z.append(row[2])
#         #x = list(map(float, x))
#         #y = list(map(float, y))
#         #z = list(map(float, z))
#         return time

# time = printcsv()
# print("Time: ", time[1], "\r\n")

# import csv
# fields = ["time", "header", "pose"]
# with open('drone_local_position_unformated.csv') as csvfile:
#     reader = csv.DictReader((line.replace(',"', '&') for line in csvfile), fieldnames=fields, delimiter='&', quotechar='&', doublequote=False)
#     line_count = 0
#     for row in reader:
#         if line_count == 0:
#             print(f'Column names are {", ".join(row)}')
#             line_count += 1
#         else:
#             print(row['pose'])
#             line_count += 1

# %%
import csv
import json
with open("drone_local_position_unformated.csv", 'r') as file:
    headers = ['time', 'header', 'pose']
    csvreader = csv.DictReader(file, delimiter=',',  fieldnames=headers)
    for i, row in enumerate(csvreader):
        if i == 0:
            continue 
        if i > 1:
            break 
        
        print(row)
        print(row['pose'])
        a = row['pose'].replace("'", "\"")
        convertedDict = json.loads(a)
        print(convertedDict)
        print(convertedDict['position'])
