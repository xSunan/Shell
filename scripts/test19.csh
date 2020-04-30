#! /bin/csh -f

# set tmpFile = `mktemp`
#echo tmpFile

set tmpFile = fout
set resultDir = results
set test = 19

echo "./pr 2 ${resultDir}/${tmpFile} & ./pr 1 ${resultDir}/${tmpFile} & ./pr 0 ${resultDir}/${tmpFile}" > in.txt
echo "./pr 2 ${resultDir}/${tmpFile} & ./pr 1 ${resultDir}/${tmpFile} & ./pr 0 ${resultDir}/${tmpFile}" > ${resultDir}/${test}.out

rm -f ${resultDir}/${tmpFile}
rm -f ${resultDir}/${test}.${tmpFile}

./pr 0 ${resultDir}/${test}.${tmpFile}
./pr 1 ${resultDir}/${test}.${tmpFile}
./pr 2 ${resultDir}/${test}.${tmpFile}


rm -f ${resultDir}/${test}.err
touch ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
