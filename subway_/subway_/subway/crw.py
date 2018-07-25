import requests
import re
from bs4 import BeautifulSoup


class station:
    def __init__(self):
        station_id = ""
        station_name = ""
        station_distance = 0.0
        transnum = 0
        trans = ""
        
        

station_info = []

####################################
## 사용 전 아래 변수를 필히 변경한다.

## 이 아래 주소에 각 호선 역 목록 사이트 주소를 넣는다.

res = requests.get('https://namu.wiki/w/%EC%9D%B8%EC%B2%9C%20%EB%8F%84%EC%8B%9C%EC%B2%A0%EB%8F%84%202%ED%98%B8%EC%84%A0/%EC%97%AD%20%EB%AA%A9%EB%A1%9D')

## 저장할 파일 이름을 적는다.

f = open("incheon2_line.txt", "w")


## 그 사이트에 들어가서, 테이블을 보고 각 정보가 저장되어있는 열을 작성한다.

id_table = 1
name_table = 3
distance_table = 2
transfer_table = 7

##################################




soup = BeautifulSoup(res.content, 'html.parser')


cont = soup.find("table", {"class" : "wiki-table"})

cont_p = cont.find_all("tr")
 

dot = ""

station_number = 0
ignore_num = 0
for content in cont_p:      # each table row
    
    each_td = content.find_all("td")
    station_info.append(station())      # add station info

    if ignore_num < 2:
        ignore_num += 1
        continue

    
    num = 0
    flag = 0
    for i in each_td:
        num += 1
        foundcont = i.find("p")
        k = foundcont.find("del")
            
        if num == id_table and k == None:
            flag = 1
            station_info[station_number].station_id = foundcont.get_text()
                
        if num == distance_table and flag == 1 and k == None:
            station_info[station_number].station_distance = float(foundcont.get_text())

        if num == name_table and flag == 1 and k == None:
            station_info[station_number].station_name = foundcont.get_text()


        if num == transfer_table and flag == 1:
            station_info[station_number].trans = foundcont.get_text()

            trnsnum = 0
            for anum in foundcont.find_all("a"):
                trnsnum += 1
            station_info[station_number].transnum = trnsnum
            

    if flag == 1:
        station_number += 1
   

pattern = re.compile(r'\s+')


for i in range(station_number):
    station_info[i].station_name = re.sub(pattern, '', station_info[i].station_name)




for i in range(station_number):
    tmp_transnum = 2
    if i == 0 or i == station_number - 1:
        tmp_transnum = 1

    tmp_transnum += station_info[i].transnum

    res1 = ""
    res2 = ""
    res1 = station_info[i].station_id + "\t" + station_info[i].station_name + "\t" + str(tmp_transnum) + "\t"


    if i != station_number - 1:
        res2 += station_info[i+1].station_id + "\t" + str(round(station_info[i+1].station_distance - station_info[i].station_distance, 1)) + "\t"
    if i != 0 :
        res2 += station_info[i-1].station_id + "\t" + str(round(station_info[i].station_distance - station_info[i-1].station_distance, 1)) + "\t"

    for j in range(station_info[i].transnum):
        res2 += "ADD" + "\t" + "0.0"+ "\t"


    f.write(res1 + res2+"\n")


f.close()

