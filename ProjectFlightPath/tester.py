<<<<<<< HEAD
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

import csv
fields = ["time", "header", "pose"]
with open('drone_local_position_unformated.csv') as csvfile:
    reader = csv.DictReader((line.replace(',"', '&') for line in csvfile), fieldnames=fields, delimiter='&', quotechar='&', doublequote=False)
    line_count = 0
    for row in reader:
        if line_count == 0:
            print(f'Column names are {", ".join(row)}')
            line_count += 1
        else:
            print(row['pose'])
            line_count += 1
=======
f = open('drone_local_position_unformated.csv', 'r')
data = f.read()

line_count = 0
for row in data:
    if line_count == 0:
        print(f'Column names are {",".join(row)}')
        line_count += 1
    else:
        print(f'\t{row[0]}.')
        break

for line in data:
    print(data[29:40])

# drone_local_position_unformated.csv
>>>>>>> 938b7a3 (Co-authored-by: Oliver Malling <revilo624@gmail.com>)
