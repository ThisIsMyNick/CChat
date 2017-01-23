#!/bin/bash

head -n "$(grep -nr "## Documentation" README.md | cut -d : -f 1)" README.md > README.md.tmp
mv README.md.tmp README.md

last=$(git rev-parse HEAD)
short=$(git log --pretty=format:'%h' -n 1)
echo -e "Documentation is taken directly from the source. Run `./generate_documentation.sh` to generate the documentation." >> README.md

for file in src/client/*.c; do
    {
        echo "${file}:"
        echo "\`\`\`C"
        sed -n -e '/\/\*/,/{/p' "$file" | sed "s/{//g" | sed "s/)/);/"
        echo "\`\`\`"
    } >> README.md
done
