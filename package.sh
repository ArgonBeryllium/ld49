rm out -rf
mkdir out
cp /usr/lib/libcumt.so out/
cp /usr/lib/libfizzyx.so out/
cp res/ out/res -r
cp bin/test out/mom
echo "LD_LIBRARY_PATH=. ./mom" >> out/run.sh
chmod a=rwx out/run.sh
rm out.zip
zip out.zip -r out/
