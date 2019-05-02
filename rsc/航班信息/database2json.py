import json


def string2json(s):
    l = s.split()
    l[3] = l[3].split(":")
    l[4] = l[4].split(":")
    l[3] = [0, int(l[3][0]), int(l[3][1])]
    l[4] = [0, int(l[4][0]), int(l[4][1])]
    l[5] = int(l[5])

    if l[4] < l[3]:
        l[4][0] = 1
    else:
        l[4][0] = 0

    if l[6] == '0':
        l[6] = "N"
        # N stands for "normal"
    elif l[6] == '1':
        l[6] = "HSR"
        # HSR sands for "high-speed rail"
    elif l[6] == '2':
        l[6] = "AC"
        # AC stands for "aircraft"

    # 计算持续时间
    duration = [0, 0, 0]
    duration[0] = l[4][0]
    duration[1] = l[4][1] - l[3][1]
    duration[2] = l[4][2] - l[3][2]
    if(duration[2] < 0):
        duration[2] += 60
        duration[1] -= 1
    if(duration[1] < 0):
        duration[1] += 24
        duration[0] -= 1

    d = {
        'from': l[0],
        'to': l[1],
        'number': l[2],
        'start': l[3],
        'end': l[4],
        'duration': duration,
        'price': l[5],
        'type': l[6]
    }
    j = json.dumps(d)
    j = json.loads(j)
    return j


jl = []
with open("database_origin.txt") as f:
    t = open("database.txt", 'w')
    for l in f:
        j = string2json(l)
        jl.append(j)
        print(j, file=t)
t = open("database_json.txt", 'w')
print(jl, file=t)
