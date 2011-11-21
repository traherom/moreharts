all: unpacked

crx: packed
packed: pwdholder.crx
pwdholder.crx: clean unpacked
	cp ~/Dropbox/conf/pwdholder.pem ..
	google-chrome --pack-extension=. --pack-extension-key=../pwdholder.pem
	rm ../pwdholder.pem
	mv ../chrome.crx pwdholder.crx

clean:
	rm -f jquery.min.js pwdholder.js pwdholder.crx

unpacked: jquery.min.js pwdholder.js

pwdholder.js: ../../static/pwdholder.js
	cp ../../static/pwdholder.js .

jquery.min.js: 
	wget http://ajax.googleapis.com/ajax/libs/jquery/1/jquery.min.js

