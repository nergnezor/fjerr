from kodipydent import Kodi
kodi = Kodi('192.168.0.101', port=8080, username='kodi', password=None)
k = str(kodi)
for line in k.splitlines():
    if 'Recent' in line:
        print(line)
eps = kodi.VideoLibrary.GetRecentlyAddedEpisodes()
print eps
