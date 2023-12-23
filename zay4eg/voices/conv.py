import os

for pack in os.listdir('ogg'):
	print(pack)
	os.makedirs('wav/'+pack, exist_ok=True)
	for f in os.listdir('ogg/'+pack):
		#print(pack+'/'+f)
		if f[-3:] != 'ogg':
			continue
		path = pack+'/'+f
		tpath = path[:-3]+'wav'
		if os.path.isfile('wav/'+tpath):
			print('Skipping '+path)
			continue
		cmd = 'ffmpeg -i "ogg/'+path+'" -acodec pcm_u8 -ac 1 -ar 16000 -filter:a "atempo=1.2" "wav/'+tpath+'"'
		print(cmd)
		os.system(cmd)
		
