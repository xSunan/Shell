#! /bin/csh -f

echo "An ERROR has occurred" > results/1.err
touch results/1.out
echo 1 > results/1.status

./mysh /this/file/is/inaccessible
