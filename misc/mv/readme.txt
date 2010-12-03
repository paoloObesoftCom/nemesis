MV EPG Plugin For Dreambox

MORE INFO:

http://mv.dinglisch.net/
http://sourceforge.net/projects/mv-plugin/

FILES:

mv.so/mv.cfg:           MV Standard version
mv-lite.so/mv-lite.cfg: MV Lite version (no config)
multiepg		MHW and OTV decoder binary
src/			MV source code
readme.txt:		this file
build.txt:		instructions for building the source
changes.txt:            main changes between versions
imagemaker.txt:		some info for image makers on using MV
lang.txt.en		File of english screen texts
icons/			channel icons
icons.esml 		map from channel names to channel icons

UPGRADING/INSTALL:

ftp epg_inst.sh and inst_mv.tgz to /tmp on dreambox.
login to dreambox, then :

cd /tmp
sh ./epg_inst.sh

run MV
   configure inputs :
       menu --> Input Config
   load datas :
       menu --> Load Epg Data
   map channels :
	menu --> Alias Manager
quit MV, then run again.
