import csv

#Get .csv file and put it into x,y,z list
def printcsv():
    time = []
    y = []
    z = []
    with open('drone_local_position_unformated.csv') as csv_file:
        csv_reader = csv.reader((line.replace(',""', '%') for line in csv_file), delimiter='%', quotechar= '%', doublequote=False)
        line_count = 0
        for row in csv_reader:
            if line_count == 0:
                  print(f'Column names are {", ".join(row)}')
                  line_count += 1
            else:
                  print(f'\t{row[0]} ----------------------- {row[1]} ----------------------- {row[2]}')
                  time.append(row[0])
                  line_count += 1
            #y.append(row[1])
            #z.append(row[2])
        #x = list(map(float, x))
        #y = list(map(float, y))
        #z = list(map(float, z))
        return time

time = printcsv()
print("Time: ", time[1], "\r\n")