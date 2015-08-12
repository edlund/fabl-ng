
#!/bin/bash

touch cscope-tags.txt
find | grep "\.c$" >> cscope-tags.txt
find | grep "\.cpp$" >> cscope-tags.txt
find | grep "\.h$" >> cscope-tags.txt
find | grep "\.hpp$" >> cscope-tags.txt
cscope -i cscope-tags.txt

exit 0

