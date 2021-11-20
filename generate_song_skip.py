"""
Write out a random song for integration with the PatternPlayer_debian_RtAudio_sdl12 executable, and read by picoloop/PatternPlayer.cpp
"""
import numpy as np
import argparse
import os
from dotenv import load_dotenv
load_dotenv()
thisdir = os.getenv("REPO_BASE", os.getcwd())

if not os.path.isdir("data"):
    os.makedirs("data")


def shuffle_note_array(note_arr):
    """
    This shuffles the initial seed points with some offset
    otherwise, we would have the same rhythmic patterns
    once we get some data we can remove these crutches.

    Example: for a given base pattern on channel 0
    0 0 0 21 0 25
    this would change that to 
    0 ->25 0 0 0 ->21
    and then add/subtract some random value
    0 ->22 0 0 0 ->23
    and finally potentially set a few values to 0
    0 22->0 0 0 0 ->23
    """
    offset = np.random.randint(-3, 3)
    offset2 = np.random.randint(-3, 3)
    note_arr[1, :8] = np.roll(note_arr[0, :8], offset)
    note_arr[1, 8:] = np.roll(note_arr[0, 8:], offset2)
    note_arr[1, np.where(note_arr[1, :] > 0)[0]] += np.random.randint(-5, 5)
    choices = np.random.choice(np.arange(16), 3, replace=False)
    note_arr[1, choices] = 0
    return note_arr


def run(seed):
    """
    Writes out a random song to the notes_array.csv and bars_threads.csv files.
    The shape is hard coded (20 bars, up to 20 tracks)
    """
    print("seed", seed)
    np.random.seed(seed)
    bars_header = "4,20"
    notes_header = "20,16"
    note_arr = np.genfromtxt(
        "note_array_orig.csv", skip_header=1, delimiter=',', dtype=int)
    m_arr = np.genfromtxt("bars_threads_orig.csv", skip_header=1,
                          delimiter=',', dtype=int)

    # TODO: this is a workaround to get off the ground without
    # too much variation.
    # We restrict the note range to between 20 and 60
    # and only allow 4 notes per thread to be non zero
    # In practice the gaps between notes are more common than
    # filling the song with constant notes.

    max_tracks = 20

    def track(mintrack=0, maxtrack=max_tracks): return np.random.randint(
        mintrack, maxtrack)
    m_arr[0, :6] = [track() for _ in range(6)]
    m_arr[1, :6] = [track() for _ in range(6)]
    m_arr[2, :6] = [track() for _ in range(6)]
    m_arr[3, :6] = [track() for _ in range(6)]

    # note range is fixed between 20 and 60
    max_range = 60
    min_range = 20
    for i in range(max_tracks):
        note_arr[i, :] = [np.random.randint(
            min_range, max_range) for _ in range(16)]
        choices = np.random.choice(np.arange(16), 12, replace=False)
        note_arr[i, choices] = 0

    note_arr = shuffle_note_array(note_arr)
    np.savetxt("picoloop/note_array.csv", note_arr, header=notes_header,
               comments='', fmt='%d', delimiter=',')
    np.savetxt("picoloop/bars_threads.csv", m_arr, header=bars_header,
               comments='', fmt='%d', delimiter=',')

    # generate audioout.wav
    audioout_cmd = f'cd picoloop && export DUMP_AUDIO=1 && ./PatternPlayer_debian_RtAudio_sdl12 {seed}'
    print(audioout_cmd)
    ou = os.system(audioout_cmd)
    if ou != 0:
        raise Exception('Picoloop failed')

    # store raw files on file system for retrieval later
    note_array_copy_cmd = f'cp {os.path.abspath(thisdir)}/picoloop/note_array.csv {os.path.abspath(thisdir)}/data/note_array{seed}.csv'
    bars_array_copy_cmd = f'cp {os.path.abspath(thisdir)}/picoloop/bars_threads.csv {os.path.abspath(thisdir)}/data/bars_threads{seed}.csv'
    audio_out_copy_cmd = f'cp {os.path.abspath(thisdir)}/picoloop/audioout.wav {os.path.abspath(thisdir)}/data/audio{seed}.wav'
    print(note_array_copy_cmd)
    print(bars_array_copy_cmd)
    print(audio_out_copy_cmd)
    os.system(note_array_copy_cmd)
    os.system(bars_array_copy_cmd)
    os.system(audio_out_copy_cmd)


if __name__ == "__main__":
    parser = argparse.ArgumentParser('')
    parser.add_argument('--seed', dest='seed', default=0)
    args = parser.parse_args()
    seed = int(args.seed)
    run(seed)
