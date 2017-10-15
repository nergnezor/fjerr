from kodipydent import Kodi
import json
kodi = Kodi('192.168.0.101', port=8080, username='kodi', password=None)
k = str(kodi)
# print kodi
for line in k.splitlines():
    if 'GetRecentlyAddedEpisodes' in line:
        print(line)
# eps = kodi.VideoLibrary.GetRecentlyAddedEpisodes( properties=["title", "showtitle", "tvshowid"])
eps = kodi.VideoLibrary.GetRecentlyAddedEpisodes(limits=5, properties=["showtitle", "thumbnail", 'playcount', 'firstaired'])
# print(eps)
print json.dumps(eps, indent=4, separators=(',', ': '))
# print(kodi.VideoLibrary.GetTVShowDetails('f855001943844f259f7045b449c695d0'))
