import plotly.express as px
import pandas as pd
import sys

fname = sys.argv[1]

df = pd.read_csv(fname, header=0, delimiter=';')

fig = px.line(df, x="Iteration", y="gap")
fig.write_html(fname.replace('.csv', '.html'))