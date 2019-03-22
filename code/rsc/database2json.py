import json

def string2json(s):
    l = s.split()
    l[3] = l[3].split(":")
    l[4] = l[4].split(":")

    l[3] = [int(i) for i in l[3]]
    l[4] = [int(i) for i in l[4]]
    l[5] = int(l[5])

    if l[4] < l[3]:
        l[4].insert(0,1)
    else:
        l[4].insert(0,0)

    if l[6] == '0':
        l[6] = "N"
        # N stands for "normal"
    elif l[6] == '1':
        l[6] = "HSR"
        # HSR sands for "high-speed rail"
    elif l[6] == '2':
        l[6] = "AC"
        # AC stands for "aircraft"
    
    d = {
        'from':l[0],
        'to':l[1],
        'number':l[2],
        'start':l[3],
        'end':l[4],
        'price':l[5],
        'type':l[6]
        }
    j = json.dumps(d)
    return j

t = open("database.txt",'a')
with open("database_origin.txt") as f:
    for l in f:
        j = string2json(l)
        print(j,file=t)
    