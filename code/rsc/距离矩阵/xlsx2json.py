import pandas as pd
from pandas import Series, DataFrame
import json

filename = '中国各省省会经纬度.csv'
df = pd.read_csv(filename)
d = df.set_index('地区').T.to_dict()
j = json.dumps(d, ensure_ascii=False)

with open("LatitudeAndLongitude.json", "w") as f:
    print(j, file=f)

filename = '距离矩阵.csv'
df = pd.read_csv(filename)
d = df.set_index('地区').T.to_dict()
j = json.dumps(d, ensure_ascii=False)
with open("DistanceMatrix.json", "w") as f:
    print(j, file=f)

D1 = json.loads(open("DistanceMatrix.json", "r").read())
D2 = json.loads(open("LatitudeAndLongitude.json", "r").read())
D = {}
for k, v in D1.items():
    D[k] = v
for k, v in D2.items():
    D[k]['经度'] = v['经度']
    D[k]['纬度'] = v['纬度']
j = json.dumps(D, ensure_ascii=False)
with open("DistanceInformation.json", "w") as f:
    print(j, file=f)
