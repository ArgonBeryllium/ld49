rm out_win -rf
mkdir out_win
cp res/ out_win/res -r
cp bin/win/test.exe out_win/mom.exe
cp bin/win/*.dll out_win/
zip out_win.zip -r out_win/
