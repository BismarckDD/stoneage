set -x
echo "to be replaced:"$1
echo "new content:"$2
find . -name "*.h" -o -name "*.cpp" | xargs sed -i 's/$1/$2/g'
