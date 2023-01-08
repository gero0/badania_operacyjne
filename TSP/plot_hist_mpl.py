import matplotlib.pyplot as plt
import pandas as pd
from os import listdir, path

PATH = "./hist"
DST_PATH = "./plots"

files = [f for f in listdir(PATH) if ".csv" in f]

for f in files:
    df = pd.read_csv(path.join(PATH, f), header=0, delimiter=';')
    df.plot(x="Iteration", y="gap", figsize=(15, 4), linewidth=0.3, legend=False)
    plt.savefig(path.join(DST_PATH, f.replace('.csv', '.png')), bbox_inches='tight')
