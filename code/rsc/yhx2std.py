import json

D = json.loads(open("database_yhx.json", "r").read())
for j in D:
    j.update(number=j.pop("train_name"))
    j["from"] = j.pop("ticket_from")  # 因为 from 是关键字
    j.update(to=j.pop("ticket_to"))
    j.update(start=j.pop("ticket_start_time"))
    j.update(end=j.pop("ticket_end_time"))
    j.update(duration=j.pop("ticket_length"))
    j.update(price=j.pop("price"))
    j["type"] = j["number"][0]
    j.pop('train_number')
    j.pop('train_start')
    j.pop('train_end')
    j.pop('ticket_date')
    j.pop('ticket_from_station_number')
    j.pop('ticket_to_station_number')
    j.pop('seat_types')

    j['price'] = int(float(j['price']))
    j['start'] = j['start'].split(":")
    j['start'] = [0, int(j['start'][0]), int(j['start'][1])]
    j['end'] = j['end'].split(":")
    j['end'] = [0, int(j['end'][0]), int(j['end'][1])]
    if j['end'][1] < j['start'][1] or (j['end'][1] == j['start'][1] and j['end'][2] < j['start'][2]):
        j['end'][0] = 1
    j['duration'] = j['duration'].split(":")
    j['duration'] = [int(int(j['duration'][0]) > 24), int(
        j['duration'][0]) % 24, int(j['duration'][1])]

with open("database_yhx.txt", "w") as f:
    for j in D:
        print(j, file=f)
