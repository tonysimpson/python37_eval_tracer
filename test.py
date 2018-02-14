import requests
from eval_tracer import *

F = enable()
requests.get('https://httpbin.org/get').json()
disable()

from collections import defaultdict, Counter
a = defaultdict(list)
for f in visit_frames(F):
    a[f.frame.f_code].append(f)
co = Counter(f.frame.f_code for f in visit_frames(F))

