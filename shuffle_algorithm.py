#http://keyj.emphy.de/balanced-shuffle/

import random
import itertools
import sys
import codecs
from typing import List
from collections import OrderedDict
from collections import defaultdict





def shuffle(play: List, artist: List):
    list_ans = []
    num_size = len(play)
    mydict = defaultdict(list)
    dummyname = "_"
    list_ans_art = []

    bad_case_play = []
    bad_case_art = []

    for i in range(num_size):
        if artist[i] in mydict:
            mydict.get(artist[i]).append(play[i])
        else:
            mydict[artist[i]] = [play[i]]

    num_artist = len(mydict.keys())

    for key, value in mydict.items():
        num_diff = num_size - len(mydict[key])
        if num_diff > 0:
            for _ in range(num_diff):
                mydict.get(key).append(dummyname)

        random.shuffle(mydict[key])
    
    for ch_idx in range(num_size):
        temp_chunk = []
        # record the list of artists
        artist_list = []

        for key, value in mydict.items():
            current_key = mydict.get(key)[ch_idx]
            # exclude dummy here
            if current_key != dummyname:
                temp_chunk.append(mydict.get(key)[ch_idx]+"\t")
                # record artists too (not dummy)
                artist_list.append(key)
        
        if len(list_ans) == 0:
            list_ans.extend(temp_chunk)
            list_ans_art.extend(artist_list)

        # has item to add
        elif len(artist_list) > 0:
            # not same
            if list_ans_art[-1] != artist_list[0]:
                list_ans.extend(temp_chunk)
                # update art too.
                list_ans_art.extend(artist_list)

            elif list_ans_art[-1] == artist_list[0]:
                # BAD
                if len(artist_list) == 1:
                    # both have one item.
                    bad_case_art.append(artist_list[0])
                    bad_case_play.append(temp_chunk[0])
                    # skip adding to the ans
                    continue

                # both
                temp_chunk.append(temp_chunk.pop(0))
                artist_list.append(artist_list.pop(0))

                list_ans.extend(temp_chunk)
                list_ans_art.extend(artist_list)

    print(bad_case_art)
    print(bad_case_play)

    # BAD ones
    bad_len = len(bad_case_art)
    for bad_idx in range(bad_len):
        print(bad_idx)
        for ans_idx in range(len(list_ans)-1,-1,-1):
            print(ans_idx)
            # not same on both side
            if bad_case_art[bad_idx] != list_ans_art[ans_idx] and bad_case_art[bad_idx] != list_ans_art[ans_idx-1]:
                list_ans.insert(ans_idx, bad_case_play[bad_idx])
                list_ans_art.insert(ans_idx, bad_case_art[bad_idx])
                # break and next BAD
                break

    #list_ans.extend(bad_case_play)

    ans = "".join(list_ans)+"\n"

    #print(ans)
    print(len(ans))





test_count = int(1)

plist = ['lzctmi', 'qupe', 'Don`t Look Back In Anger', 'The Reasonnrsilt', 'tgyjbs', 'Power Of Love (3rd Coast Remix)', 'Dreaming', 'xlvkyt', 'We Are Never Ever Getting Back Together', 'We Are Never Ever Getting Back Together', 'Another Brick In The Wall, (Part 2)', 'Another Brick In The Wall, (Part 2)', 'Dirty Pawsfeue', 'Dirty Paws', 'Sing Sing Sing']
alist = ['Oasis', 'Oasis', 'Oasis', 'Hoobastank', 'Hoobastank', 'FreeTEMPO', 'FreeTEMPO', 'TaylorSwift', 'TaylorSwift', 'TaylorSwift', 'Pink', 'Pink', 'OfMonstersAndMen', 'OfMonstersAndMen', 'BennyGoodman']

print(len(plist))

print("---")


for c in range(test_count):
    shuffle(plist,alist)
