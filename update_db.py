"""
Add a line to the db song database using base SQL commands
"""
import pdb
import argparse
import sqlite3


parser = argparse.ArgumentParser('')
parser.add_argument('--seed', required=True, dest='seed')
parser.add_argument('--r', required=False, dest='rhythm_score', default=None)
parser.add_argument('--s', required=False, dest='song_score', default=None)
parser.add_argument('--l', required=False, dest='song_location', default='')

args = parser.parse_args()
con = sqlite3.connect("songs.db")
cur = con.cursor()
kw = {}
if args.song_score is not None:
    kw.update({"song_score": args.song_score})
if args.rhythm_score is not None:
    kw.update({"rhythm_score": args.rhythm_score})
if args.song_location != '' and args.song_location is not None:
    kw.update({"location": "'%s'" % args.song_location})

song_score = args.song_score
rhythm_score = args.rhythm_score
if args.song_score is None:
    song_score = 'NULL'
if args.rhythm_score is None:
    rhythm_score = 'NULL'

try:
    cur.execute(
        f'''insert into songs values ({args.seed}, {song_score}, {rhythm_score}, '{args.song_location}')  ''')
except sqlite3.IntegrityError:
    # primary key violation
    cmt = []
    for key in kw:
        cmt.append('%s = %s' % (key, kw[key]))

    cmt = ','.join(cmt)
    cmd = f'update songs set {cmt} where seed={args.seed}'
    print(cmd)
    cur.execute(cmd)


con.commit()
