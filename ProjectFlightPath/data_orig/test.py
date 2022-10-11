import csv
import json

fields = ["time", "header", "pose"]
with open('drone_local_position_unformated.csv') as csvfile:
    reader = csv.DictReader((line.replace(',"', '&') for line in csvfile), fieldnames=fields, delimiter='&', quotechar='&', doublequote=False)
    line_count = 0
    for i, row in enumerate(reader):
        if i == 0:
            continue
        if i > 1:
            break
        print(row['pose'])
        a = row['pose']
        convertedDict = json.loads(a)
        print(convertedDict)
        print(convertedDict['position'])
