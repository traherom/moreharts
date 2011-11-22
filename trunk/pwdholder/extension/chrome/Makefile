all: unpacked

crx: packed
packed: pwdholder.crx
pwdholder.crx: unpacked
	cp ~/Dropbox/conf/pwdholder.pem ..
	cd .. && google-chrome --pack-extension=chrome --pack-extension-key=pwdholder.pem --no-message-box
	rm ../pwdholder.pem
	mv ../chrome.crx pwdholder.crx
	
clean:
	rm -f jquery.min.js pwdholder.js *.crx *.pem ../*.crx

unpacked: jquery.min.js pwdholder.js

pwdholder.js: ../../static/pwdholder.js
	cp ../../static/pwdholder.js .

jquery.min.js: 
	wget http://ajax.googleapis.com/ajax/libs/jquery/1/jquery.min.js

