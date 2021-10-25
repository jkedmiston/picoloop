import numpy as np
import sqlite3
from generate_song_skip import run
seeds = np.arange(20, 200)
con = sqlite3.connect("songs.db")
cur = con.cursor()

for _, s in enumerate(seeds):
    hits = list(cur.execute(
        f'select * from songs s where s.seed={s} and s.rhythm_score not null and s.song_score not null'))
    print("hits", list(hits))
    if len(hits) > 0:
        print("continuing due to hits")
        continue

    run(s)
