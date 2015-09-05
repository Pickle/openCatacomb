#!/bin/sh

copyfiles()
{       
    find "$1" -path '*/*'|while read file; do
    
        # Removes the source folder (default)
        output=$2${file##$1}
        
        if [ -d "$file" ]; then
            echo " Creating directory: $output"
            mkdir "$output"
        else
            if [ -f "$output" ]; then
                echo " File exists NO COPY: $output"
            else
                echo " Copying: $file to $output"
                cp "$file" "$output"
            fi
        fi
    done
}

echo "Checking default opk files"
copyfiles default ${HOME}

./opencatacomb
