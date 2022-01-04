from bs4 import BeautifulSoup
import requests
import pandas as pd
from argparse import ArgumentParser

def parse_args():
    parser = ArgumentParser()
    parser.add_argument("-f", "--filename", type=str, action='store')
    parser.add_argument("-p", "--prefix", type=str, action='store')
    parser.add_argument("-o", "--output", type=str, action='store')
    args = parser.parse_args()
    return args


#r= requests.get("http://ftp.itec.aau.at/datasets/DASHDataset2014/BigBuckBunny/1sec/bunny_4726737bps/")
#page = r.text
#
#with open('bbb.html', 'w') as fh:
#    fh.write(r.text)
#    fh.close()
def isfloat(value):
    try:
        float(value)
        return True
    except ValueError:
        return False


def parse_file(filename, prefix, output):
    with open(filename) as fh:
        soup = BeautifulSoup(fh, "html.parser")
    
    #prefix = 'BigBuckBunny/1sec/bunny_4726737bps/'
    segments = []
    for tr in soup.find_all("tr"):
        tds = tr.find_all("td")
        if len(tds) >= 5:
            if len(tds[1].find_all("a")) > 0:
            
                atags = tds[1].find_all("a")
                segment = atags[0]["href"]
                size = tds[3].text.strip()
                print(segment, size)
                if segment is None or size is None or not isfloat(size[:len(size)-2]):
                    continue
                size_int = float(size[:len(size)-1])
                if 'M' in size:
                    size_int *= 1e6
                elif 'K' in size:
                    size_int *= 1e3
                segments.append({'id':prefix + segment, 'size': size_int})
    df = pd.DataFrame(segments)
    print(df)
    df.to_csv(output, index=False)

args = parse_args()
parse_file(args.filename, args.prefix, args.output)
