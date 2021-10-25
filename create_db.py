import sqlite3
con = sqlite3.connect("songs.db")
cur = con.cursor()
cur.execute(
    '''create table if not exists songs (seed int primary key, song_score real, rhythm_score real, location text)''')
con.commit()
con.close()
