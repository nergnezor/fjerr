import pychromecast

cast = pychromecast.Chromecast('192.168.0.6')
cast.wait()
mc = cast.media_controller
mc.block_until_active()
print(mc.status)