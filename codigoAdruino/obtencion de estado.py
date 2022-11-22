import requests
import pandas as pd

url = 'http://192.168.100.174/'
html = requests.get(url).content
df_list = pd.read_html(html)

df = df_list[-1]
print(df)
df.to_csv('output.csv')