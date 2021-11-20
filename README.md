# Fork of Picoloop

* `cd picoloop && make -f Makefile.PatternPlayer_debian_RtAudio_sdl12 -j8`
* `export DUMP_AUDIO=1 && ./PatternPlayer_debian_RtAudio_sdl12 18`
* Produces `audioout.wav` based on `picoloop/note_array.csv` and `picoloop/bars_threads.csv`. 


## Python
Python generates the note and bar array files at `note_array.csv` and `bars_threads.csv`.
To generate a random music sample, use
* `python generate_song_skip.py --seed=10`

## Database
* To generate the song score database, `python create_db.py`
* To update the song score database, `python update_db.py --seed=10 --r=0.32` would update the rhythm score to 0.32 for song seeded by 10.

