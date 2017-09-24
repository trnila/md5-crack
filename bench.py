import random
import string
import hashlib
import subprocess
import timeit
import time

def test(params):
    proc = subprocess.Popen(params)
    proc.communicate()


f = open("data.csv", "w")
for i in range(1, 4):
    s = ''.join(random.choice(string.ascii_letters) for i in range(i))
    print(s)
    m = hashlib.md5()
    m.update(s.encode('utf-8'))
    print(m.hexdigest())

    start = time.time()
    test(["./crack", m.hexdigest(), str(i)])
    elapsed = time.time() - start

    f.write("{}, {}\n".format(i, elapsed))

f.close()
    
