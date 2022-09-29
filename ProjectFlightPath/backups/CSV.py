import csv

#Get .csv file and put it into x,y,z list
def printcsv():
    x = []
    y = []
    z = []
    with open('drone_local_position.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        for row in csv_reader:
            x.append(row[0])
            y.append(row[1])
            z.append(row[2])
        x = list(map(float, x))
        y = list(map(float, y))
        z = list(map(float, z))
        return x, y, z